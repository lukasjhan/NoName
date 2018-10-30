# file              /makefile
# date              2018.10.29
# last edit date    2018.10.30
# author            NO.00[UNKNOWN]
# breif             global makefile

all: BootLoader Disk.img

BootLoader:
	@echo 
	@echo ++++++++++++++++++++ Building Boot Loader STRAT!!! ++++++++++++++++++++

	make -C BootLoader

	@echo ++++++++++++++++++++ Building Boot Loader COMPLETE!!! ++++++++++++++++++++
	@echo

Disk.img: BootLoader/BootLoader.bin
	@echo
	@echo ++++++++++++++++++++ OS image Building START!!! ++++++++++++++++++++

	cp BootLoader/BootLoader.bin Disk.img

	@echo ++++++++++++++++++++ OS image Building COMPLETE!!! ++++++++++++++++++++


clean:
	make -C BootLoader clean
	rm -f Disk.img