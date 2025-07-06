/*
 * TinyDOS All-in-One Installer
 * A single, static binary to install the OS without BusyBox.
 *
 * COMPILE WITH:
 * gcc -static -o installer installer.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <linux/reboot.h>
#include <errno.h>

#define MBR_SIZE 512
#define MBR_BOOT_CODE_SIZE 440 // SYSLINUX MBR uses 440 bytes
#define PARTITION_TABLE_OFFSET 446
#define MBR_SIGNATURE_OFFSET 510
#define MBR_SIGNATURE 0xAA55

struct mbr_partition_entry {
    unsigned char status;
    unsigned char chs_start[3];
    unsigned char type;
    unsigned char chs_end[3];
    unsigned int lba_start;
    unsigned int num_sectors;
} __attribute__((packed));

// --- UTILITY FUNCTIONS ---

void die(const char *message) {
    perror(message);
    printf("Installer failed. System halted.\n");
    while (1) sleep(10); // Infinite loop to halt
}

void copy_file(const char *src, const char *dest) {
    char buffer[4096];
    int src_fd = open(src, O_RDONLY);
    if (src_fd < 0) die("copy_file: open source failed");

    int dest_fd = open(dest, O_WRONLY | O_CREAT, 0644);
    if (dest_fd < 0) die("copy_file: open dest failed");

    ssize_t bytes;
    while ((bytes = read(src_fd, buffer, sizeof(buffer))) > 0) {
        if (write(dest_fd, buffer, bytes) != bytes) {
            die("copy_file: write failed");
        }
    }
    if (bytes < 0) die("copy_file: read error");
    close(src_fd);
    close(dest_fd);
}

// --- CORE INSTALLER FUNCTIONS ---

void create_device_nodes() {
    struct dirent *entry;
    DIR *dp = opendir("/sys/block");
    if (!dp) die("Could not open /sys/block");

    mkdir("/dev", 0755);

    printf("--> Creating device nodes in /dev:\n");
    while ((entry = readdir(dp))) {
        // Skip . and .. and non-disks like loop/ram
        if (entry->d_name[0] == '.') continue;
        if (strncmp(entry->d_name, "loop", 4) == 0) continue;
        if (strncmp(entry->d_name, "ram", 3) == 0) continue;

        char path[256], dev_path[512], dev_node[512];
        int major, minor;

        snprintf(path, sizeof(path), "/sys/block/%s/dev", entry->d_name);
        FILE *f = fopen(path, "r");
        if (!f) continue;

        if (fscanf(f, "%d:%d", &major, &minor) == 2) {
            snprintf(dev_node, sizeof(dev_node), "/dev/%s", entry->d_name);
            printf("    Creating %s (Major: %d, Minor: %d)\n", dev_node, major, minor);
            mknod(dev_node, S_IFBLK | 0600, makedev(major, minor));
        }
        fclose(f);
    }
    closedir(dp);
}

void write_mbr_and_partition(const char *disk_path) {
    printf("--> Writing MBR and partition table to %s\n", disk_path);
    int fd = open(disk_path, O_RDWR);
    if (fd < 0) die("Failed to open disk");

    // Get disk size in 512-byte sectors
    unsigned long long size_bytes;
    if (ioctl(fd, BLKGETSIZE64, &size_bytes) < 0) die("Failed to get disk size");
    unsigned int total_sectors = size_bytes / 512;

    unsigned char mbr[MBR_SIZE];
    memset(mbr, 0, MBR_SIZE);

    // Read SYSLINUX MBR boot code from our provided file
    int mbr_bin_fd = open("isolinux/mbr.bin", O_RDONLY);
    if (mbr_bin_fd < 0) die("Failed to open isolinux/mbr.bin");
    if (read(mbr_bin_fd, mbr, MBR_BOOT_CODE_SIZE) < 0) die("Failed to read mbr.bin");
    close(mbr_bin_fd);

    // Define one partition
    struct mbr_partition_entry *part = (struct mbr_partition_entry *)(mbr + PARTITION_TABLE_OFFSET);
    part->status = 0x80; // Bootable
    part->type = 0x83;   // Linux
    part->lba_start = 2048;
    part->num_sectors = total_sectors - 2048;

    *(unsigned short *)(mbr + MBR_SIGNATURE_OFFSET) = MBR_SIGNATURE;

    if (write(fd, mbr, MBR_SIZE) != MBR_SIZE) die("Failed to write MBR");
    
    // Force kernel to re-read the partition table
    ioctl(fd, BLKRRPART);
    close(fd);
    sleep(2); // Give kernel a moment
}

void install_boot_sector(const char *partition_path) {
    printf("--> Installing boot sector to %s\n", partition_path);
    
    int part_fd = open(partition_path, O_WRONLY);
    if (part_fd < 0) die("Failed to open partition for writing");

    int bootsector_fd = open("isolinux/isolinux.bin", O_RDONLY);
    if (bootsector_fd < 0) die("Failed to open isolinux.bin");

    char boot_code[440];
    if (read(bootsector_fd, boot_code, sizeof(boot_code)) != sizeof(boot_code)) {
        die("Failed to read from isolinux.bin");
    }

    if (write(part_fd, boot_code, sizeof(boot_code)) != sizeof(boot_code)) {
        die("Failed to write boot sector to partition");
    }

    close(part_fd);
    close(bootsector_fd);
}


int main(int argc, char *argv[]) {
    if (getuid() != 0) {
        printf("This installer must be run as root.\n");
        return 1;
    }
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <disk_name> (e.g., sda, hda)\n", argv[0]);
        return 1;
    }

    printf("--- TinyDOS Zero-Dependency Installer ---\n");
    mount("proc", "/proc", "proc", 0, NULL);
    mount("sysfs", "/sys", "sysfs", 0, NULL);

    create_device_nodes();
    
    char disk_path[256], part_path[256];
    snprintf(disk_path, sizeof(disk_path), "/dev/%s", argv[1]);
    snprintf(part_path, sizeof(part_path), "/dev/%s1", argv[1]);

    printf("\nTarget disk is: %s\n", disk_path);
    printf("WARNING: ALL DATA ON THIS DISK WILL BE ERASED!\n");
    printf("Press Enter to continue, or Ctrl+C to abort...");
    getchar();

    // 1. Partition
    write_mbr_and_partition(disk_path);

    // 2. Format
    printf("--> Formatting %s with ext4...\n", part_path);
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execl("./mkfs.ext4", "mkfs.ext4", "-F", part_path, NULL);
        // execl only returns if there is an error
        die("Failed to execute mkfs.ext4");
    }
    wait(NULL); // Wait for mkfs.ext4 to finish

    // 3. Mount and Install Files
    printf("--> Mounting and copying files...\n");
    mkdir("/mnt/target", 0755);
    if (mount(part_path, "/mnt/target", "ext4", 0, "") != 0) {
        die("Failed to mount new filesystem");
    }
    mkdir("/mnt/target/boot", 0755);
    mkdir("/mnt/target/boot/syslinux", 0755);
    copy_file("boot/vmlinuz", "/mnt/target/boot/vmlinuz");
    copy_file("boot/initrd.gz", "/mnt/target/boot/initrd.gz");
    copy_file("isolinux/ldlinux.c32", "/mnt/target/boot/syslinux/ldlinux.c32");
    copy_file("isolinux/isolinux.cfg", "/mnt/target/boot/syslinux/syslinux.cfg");

    // 4. Install Bootloader
    install_boot_sector(part_path);
    
    // 5. Cleanup
    printf("--> Finalizing...\n");
    umount("/mnt/target");
    sync();

    printf("\n--- INSTALLATION COMPLETE ---\n");
    printf("System will reboot in 5 seconds.\n");
    sleep(5);
    reboot(LINUX_REBOOT_CMD_RESTART);

    return 0; // Should not be reached
}
