# filename          /Kernel32/makefile
# date              2018.10.31
# last edit date    2018.10.31
# author            NO.00[UNKNOWN]
# brief             makefile for protected mode(32bit) kernel

all: testOS.bin

testOS.bin: testOS.asm
	nasm -o testOS.bin testOS.asm

clean:
	rm -f testOS.bin