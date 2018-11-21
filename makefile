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

Kernel64:
	@echo 
	@echo Building 64bit Kernel STRAT!!!

	make -C Kernel64

	@echo Building 64bit Kernel COMPLETE!!!
	@echo

Disk.img: BootLoader/BootLoader.bin Kernel32/Kernel32.bin Kernel64/Kernel64.bin
	@echo
	@echo OS image Building START!!!

	./ImageBuilder $^

	@echo OS image Building COMPLETE!!!


clean:
	make -C BootLoader clean
	make -C Kernel32 clean
	make -C Kernel64 clean
	rm -f Disk.img