cp ../bzImage-big ../iso-root/boot/vmlinuz
cp ../initramfs.cpio.gz ../iso-root/boot/initrd.gz
xorriso -as mkisofs -o ../bootable.iso -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -J -R -V "tinyDOS" ../iso-root/
#isohybrid bootable.iso
