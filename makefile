all: BootLoader Disk.img

BootLoader:
	@echo 
	@echo ++++++++++++++++++++ Building Boot Loader STRAT!!! ++++++++++++++++++++

	make -C BootLoader

	@echo ++++++++++++++++++++ Building Boot Loader COMPLETE!!! ++++++++++++++++++++
	@echo

Disk.img: BootLoader/BootLoader.Building
	@echo
	@echo ++++++++++++++++++++ OS image Building START!!! ++++++++++++++++++++

	cp BootLoader/BootLoader.bin Disk.img

	@echo ++++++++++++++++++++ OS image Building COMPLETE!!! ++++++++++++++++++++


clean:
	make -C BootLoader clean
	rm -f Disk.img