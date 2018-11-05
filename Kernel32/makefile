# filename          /Kernel32/makefile
# date              2018.10.31
# last edit date    2018.11.05
# author            NO.00[UNKNOWN]
# brief             makefile for protected mode(32bit) kernel

all: Kernel32.bin

Kernel32.bin: src/EntryPoint.s
	nasm -o Kernel32.bin $<

clean:
	rm -f Kernel32.bin