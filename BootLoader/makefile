# filename          /BootLoader/makefile
# date              2018.10.29
# last edit date    2018.10.29
# author            NO.00[UNKNOWN]
# brief             Bootloader makefile

all: BootLoader.bin

BootLoader.bin: BootLoader.asm
	nasm -o BootLoader.bin BootLoader.asm

clean:
	rm -f BootLoader.bin