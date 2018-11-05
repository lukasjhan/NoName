# filename          /Kernel32/makefile
# date              2018.10.31
# last edit date    2018.11.05
# author            NO.00[UNKNOWN]
# brief             makefile for protected mode(32bit) kernel

# Build Environment Setting
# Define Compiler & Linker
NASM32 = nasm
GCC32 = x86_64-pc-linux-gcc.exe -c -m32 -ffreestanding
LD32 = x86_64-pc-linux-ld.exe -melf_i386 -T ../elf_i386.x -nostdlib -e Main -Ttext 0x10200

# Define OBJCOPY OPTION For Binary Image
OBJCOPY32 = x86_64-pc-linux-objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

# Define Directory
OBJECTDIRECTORY = Temp
SOURCEDIRECTORY	= Source

# Build
all: prepare Kernel32.bin

# Object File Location
prepare:
	mkdir -p $(OBJECTDIRECTORY)

# Build Kernel Entry Point
$(OBJECTDIRECTORY)/EntryPoint.bin: $(SOURCEDIRECTORY)/EntryPoint.s
	$(NASM32) -o $@ $<

# Make Dependancy Info
dep:
	@echo === Make Dependancy File ===
	make -C $(OBJECTDIRECTORY) -f ../makefile InternalDependency
	@echo === Dependancy Search Complete ===

ExecuteInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../makefile Kernel32.elf

# Kernel Image to Binary File
$(OBJECTDIRECTORY)/Kernel32.elf.bin: ExecuteInternalBuild
	$(OBJCOPY32) $(OBJECTDIRECTORY)/Kernel32.elf $@

# Kernel + Entry Point
Kernel32.bin: $(OBJECTDIRECTORY)/EntryPoint.bin $(OBJECTDIRECTORY)/Kernel32.elf.bin
	cat $^ > $@

clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*


# TEMP DIRECTORY WORK
CENTRYPOINTOBJECTFILE = Main.o
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFILES = $(subst Main.o, , $(notdir $(patsubst %.c,%.o,$(CSOURCEFILES))))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

# Define Rule for .c -> .o
%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC32) -c $<

# Define Rule for .asm -> .o
%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM32) -f elf32 -o $@ $<

InternalDependency:
	$(GCC32) -MM $(CSOURCEFILES) > Dependency.dep

# Build Kernel Image
Kernel32.elf: $(CENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD32) -o $@ $^

# depandancy file will include into makefile
ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif