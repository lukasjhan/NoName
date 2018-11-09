# filename          /Kernel64/makefile
# date              2018.11.09
# last edit date    2018.11.09
# author            NO.00[UNKNOWN]
# brief             makefile for IA-32e mode(64bit) kernel

# Build Environment Setting
# Define Compiler & Linker
NASM64 = nasm -f elf64
GCC64 = x86_64-pc-linux-gcc.exe -c -m64 -ffreestanding
LD64 = x86_64-pc-linux-ld.exe -melf_x86_64 -T ../elf_x86_64.x -nostdlib -e Main -Ttext 0x200000

# Define OBJCOPY OPTION For Binary Image
OBJCOPY64 = x86_64-pc-linux-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

# Define Directory
OBJECTDIRECTORY = Temp
SOURCEDIRECTORY	= Source

# Build
all: prepare Kernel64.bin

# Object File Location
prepare:
	mkdir -p $(OBJECTDIRECTORY)

# Make Dependancy Info
dep:
	@echo === Make Dependancy File ===
	make -C $(OBJECTDIRECTORY) -f ../makefile InternalDependency
	@echo === Dependancy Search Complete ===

ExecuteInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile Kernel64.elf

# Kernel + Entry Point
Kernel64.bin: ExecuteInternalBuild
	$(OBJCOPY64) $(OBJECTDIRECTORY)/Kernel64.elf $@

clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*

# TEMP DIRECTORY WORK
ENTRYPOINTSOURCEFILE = ../$(SOURCEDIRECTORY)/EntryPoint.s
ENTRYPOINTOBJECTFILE = EntryPoint.o
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFILES = $(notdir $(patsubst %.c,%.o,$(CSOURCEFILES)))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

$(ENTRYPOINTOBJECTFILE): $(ENTRYPOINTSOURCEFILE)
	$(NASM64) -o $@ $<

# Define Rule for .c -> .o
%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC64) -c $<

# Define Rule for .asm -> .o
%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM64) -o $@ $<

InternalDependency:
	$(GCC64) -MM $(CSOURCEFILES) > Dependency.dep

# Build Kernel Image
Kernel64.elf: $(ENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD64) -o $@ $^

# depandancy file will include into makefile
ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif