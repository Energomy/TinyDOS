# Start fresh
#rm -rf rootfs

# Create the directory structure
mkdir -p ../rootfs/bin ../rootfs/sbin ../rootfs/dev ../rootfs/proc ../rootfs/sys

# Copy your NEW init program and link it

ln -s /bin/init ../rootfs/sbin/init



# Create the device nodes
mknod -m 666 ../rootfs/dev/null c 1 3
mknod -m 666 ../rootfs/dev/tty c 5 0
#mknod ../rootfs/dev/fb0 c 29 0
mknod -m 666 ../rootfs/dev/console c 5 1

# Package the initramfs
cd ../rootfs
find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz
cd ../scripts
