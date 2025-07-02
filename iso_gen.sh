cp bzImage iso-root-beta/boot/vmlinuz
cp initramfs.cpio.gz iso-root-beta/boot/initrd.gz
genisoimage -o TinyDOS-tiny.iso \
    -b isolinux/isolinux.bin \
    -c isolinux/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -J -R -V "tinyDOS" \
    iso-root-beta/
#isohybrid bootable.iso
