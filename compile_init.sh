#gcc -static -o init init.c -Wl,--build-id=none
x86_64-linux-gnu-gcc -static init.c -o init
