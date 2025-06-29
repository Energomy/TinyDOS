# TinyDOS

A minimalist "Linux From Scratch" style OS with a custom-written, nostalgic DOS-like shell.

**[https://github.com/minhmc2007/TinyDOS](https://github.com/minhmc2007/TinyDOS)**

![Build Status](https://img.shields.io/badge/build-passing-success.svg)
![Kernel](https://img.shields.io/badge/Linux%20Kernel-6.15.2--tinyDOS-blue.svg)
![Userspace](https://img.shields.io/badge/Userspace-BusyBox-yellow.svg)
![Bootloader](https://img.shields.io/badge/Bootloader-ISOLINUX-green.svg)
![License](https://img.shields.io/badge/License-GPLv3-lightgrey.svg)

---

## 🤔 What is TinyDOS?

Imagine booting a modern Linux kernel directly into a nostalgic MS-DOS-like environment. **That's TinyDOS.**

This is an educational project that explores the fundamentals of a Linux system. By stripping away the complexities of modern distributions, we are left with the bare essentials: a bootloader, a kernel, a minimal userspace, and our own shell.

The heart of the project is the custom `init.c` shell, which provides a classic command-line experience on top of a powerful, modern Linux kernel.

## 📸 A Glimpse into TinyDOS

Here's what a typical session looks like. Notice how you can use both built-in DOS commands and standard Linux utilities from BusyBox.

```
MyTinyDOS v0.0.1 - (c) 2025

C:\> ver
MyTinyDOS Shell [Version 0.0.1 (2025)]
Running on Linux 6.15.2-tinyDOS (x86_64)

C:\> dir
 Directory of C:\

01/01/1970  12:00 AM   <DIR>          .
01/01/1970  12:00 AM   <DIR>          ..
01/01/1970  12:00 AM   <DIR>          bin
01/01/1970  12:00 AM   <DIR>          dev
01/01/1970  12:00 AM   <DIR>          etc
...

C:\> md my_stuff

C:\> cd my_stuff

C:\MY_STUFF> echo Hello from TinyDOS > test.txt

C:\MY_STUFF> type test.txt
Hello from TinyDOS

C:\MY_STUFF> ls -l
-rw-r--r--    1 root     root            21 Jan  1 12:02 test.txt

C:\MY_STUFF> exit
Shutting down system...
[  OK  ] Reached target Power-Off.
```

## ✨ Features

### System Features
*   **Custom Linux Kernel**: Runs on a custom-configured `6.15.2-tinyDOS` kernel.
*   **BusyBox Userspace**: Provides a rich set of essential Unix utilities (`ls`, `vi`, `mount`, `free`, etc.) in a single binary.
*   **DOS-like `init` Shell**: The entire user experience is managed by `init.c`, a custom C program that acts as both the system's `init` process (PID 1) and the user's shell.
*   **ISOLINUX Bootloader**: Boots from a simple, standard ISO file.
*   **Automated Build**: A single script handles the entire build process.

### Built-in Shell Commands
The custom shell implements many classic commands internally. For anything else, it passes the command to the underlying BusyBox environment.

| Command             | Description                                          |
| ------------------- | ---------------------------------------------------- |
| `?` / `HELP`        | Shows this command reference.                         |
| `ABOUT`             | Shows author information.                            |
| `VER`               | Shows TinyDOS and Linux kernel version information.  |
| `CLS`               | Clears the screen.                                   |
| `ECHO [msg]`        | Displays a message to the screen.                    |
| `DIR [path]`        | Lists the contents of a directory in DOS format.     |
| `CD`/`CHDIR [path]` | Changes or displays the current directory.           |
| `MD`/`MKDIR [path]` | Creates a new directory.                             |
| `RD`/`RMDIR [path]` | Removes an empty directory.                          |
| `TYPE [file]`       | Displays the content of a text file.                 |
| `COPY [src] [dst]`  | Copies a single file.                                |
| `XCOPY [src] [dst]` | Recursively copies files and directories.            |
| `DEL`/`ERASE [file]`| Deletes a file.                                      |
| `REN`/`MOVE [src]`  | Renames or moves a file or directory.                |
| `EDIT [file]`       | Opens the file in MiniEdit, TinyDOS's C text editor. |
| `REBOOT`            | Restarts the system.                                 |
| `EXIT`/`SHUTDOWN`   | Powers off the system.                               |

---

## 📝 The `edit` Command and MiniEdit

TinyDOS includes an `edit` command, which launches the built-in text editor called **MiniEdit**.

### About MiniEdit

- `MiniEdit` is a C text editor inspired by the best features of both Vim and Nano.
- You can think of it as: **miniedit = Vim + Nano**.
- It’s simple to use for beginners (like Nano), but also provides efficient navigation and editing shortcuts (like Vim).
- For a detailed look at its implementation and features, see [`miniedit.c`](./miniedit.c).

### Usage

To open MiniEdit, simply run:
```shell
edit filename
```
This opens or creates the file `filename` in MiniEdit.

Explore and enjoy efficient text editing in TinyDOS!

---

## 🚀 Quick Start: Build & Run

Building and running TinyDOS is incredibly simple thanks to the automated build script.

### Step 1: Install Prerequisites

You'll need a standard Linux build environment and QEMU. On Debian/Ubuntu, you can install them with:

```bash
sudo apt-get update
sudo apt-get install build-essential git qemu-system-x86 xorriso bc libelf-dev libssl-dev
```

### Step 2: Clone and Build

The `build.sh` script does everything for you: it downloads sources, compiles the kernel and BusyBox, builds the `init` program, and generates the final bootable ISO.

```bash
git clone https://github.com/minhmc2007/TinyDOS.git
cd TinyDOS
bash build.sh
```

### Step 3: Run!

The script will create `bootable.iso`. You can run it with QEMU:

```bash
qemu-system-x86_64 -cdrom bootable.iso
```

## 🔧 Architecture & The Build Process

Understanding the architecture is key to the project's purpose.

1.  **Boot**: `ISOLINUX` boots the system from the ISO.
2.  **Kernel**: It loads the custom `bzImage` Linux kernel into memory.
3.  **Initramfs**: The kernel mounts a minimal initial RAM disk which contains the BusyBox binary and our custom `init` program.
4.  **Init**: The kernel executes `/sbin/init` (our C program) as the first process (PID 1).
5.  **Shell**: Our `init` program starts, prints the welcome message, and gives you the `C:\>` prompt.

The `build.sh` script automates the entire "Linux From Scratch" style workflow:
*   It compiles `init.c` into a static binary.
*   It constructs an `initramfs` (initial RAM filesystem) containing the necessary directory structure (`/bin`, `/sbin`, `/dev`, etc.) and populates it with BusyBox and our `init` program.
*   Finally, it packages the kernel and the `initramfs` into a bootable `tinydos.iso` using `xorriso`.

## Contributing

Contributions make the open-source community an amazing place to learn and create. Any contributions you make are **greatly appreciated**. Please fork the repo and create a pull request!

1.  Fork the Project
2.  Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3.  Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4.  Push to the Branch (`git push origin feature/AmazingFeature`)
5.  Open a Pull Request

## License

Distributed under the GNU GENERAL PUBLIC License 3. See the `LICENSE` file for more information.

## Acknowledgements

*   The **Linux From Scratch** project for being the ultimate guide.
*   The developers of **BusyBox**, the **Linux Kernel**, and **SYSLINUX**.
*   Everyone who loves the nostalgia of the command line.
