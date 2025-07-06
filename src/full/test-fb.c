#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
    int fb = open("/dev/fb0", O_RDWR);
    if (fb == -1) {
        perror("Failed to open framebuffer");
        return 1;
    }

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    if (ioctl(fb, FBIOGET_FSCREENINFO, &finfo)) {
        perror("Error reading fixed info");
        return 2;
    }
    if (ioctl(fb, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable info");
        return 3;
    }

    long screensize = vinfo.yres_virtual * finfo.line_length;
    uint8_t *fbp = (uint8_t *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
    if ((intptr_t)fbp == -1) {
        perror("Failed to mmap framebuffer");
        return 4;
    }

    // Draw a full blue screen
    for (int y = 0; y < vinfo.yres; y++) {
        for (int x = 0; x < vinfo.xres; x++) {
            long location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8)
                          + (y + vinfo.yoffset) * finfo.line_length;

            // BGRA (for 32-bit mode)
            fbp[location + 0] = 255; // Blue
            fbp[location + 1] = 0;   // Green
            fbp[location + 2] = 0;   // Red
            fbp[location + 3] = 0;   // Alpha
        }
    }

    sleep(1); // Keep it visible for 1 second

    munmap(fbp, screensize);
    close(fb);
    return 0;
}
