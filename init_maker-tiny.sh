# Start fresh
#rm -rf rootfs

# Create the directory structure
#mkdir -p rootfs/bin rootfs/sbin rootfs/dev rootfs/proc rootfs/sys

# Copy your NEW init program and link it
cp init rootfs-tiny/bin/
ln -s /bin/init rootfs-tiny/sbin/init

# <<< --- NEW PART: COPY BUSYBOX --- >>>
# Copy the compiled busybox binary into the bin directory
#cp busybox rootfs/bin/

# Create the device nodes
mknod -m 666 rootfs-tiny/dev/null c 1 3
mknod -m 666 rootfs-tiny/dev/tty c 5 0
#mknod rootfs/dev/fb0 c 29 0
mknod -m 666 rootfs-tiny/dev/console c 5 1

# Package the initramfs
cd rootfs-tiny
find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz
cd ..
