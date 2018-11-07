# filename          /makefile
# date              2018.10.29
# last edit date    2018.11.05
# author            NO.00[UNKNOWN]
# brief             root makefile

all: BootLoader Kernel32 Disk.img

BootLoader:
	@echo 
	@echo Building Boot Loader STRAT!!!

	make -C BootLoader

	@echo Building Boot Loader COMPLETE!!!
	@echo

Kernel32:
	@echo 
	@echo Building 32bit Kernel STRAT!!!

	make -C Kernel32

	@echo Building 32bit Kernel COMPLETE!!!
	@echo

Disk.img: BootLoader/BootLoader.bin Kernel32/Kernel32.bin
	@echo
	@echo OS image Building START!!!

	cat BootLoader/BootLoader.bin Kernel32/Kernel32.bin > Disk.img

	@echo OS image Building COMPLETE!!!


clean:
	make -C BootLoader clean
	make -C Kernel32 clean
	rm -f Disk.img