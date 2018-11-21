# filename          /Kernel64/Source/EntryPoint.s
# date              2018.11.09
# last edit date    2018.11.09
# author            NO.00[UNKNOWN]
# brief             entry point for 64bit mode kernel

[BITS 64]           ; 64bit code

SECTION .text

extern Main         ; import Main funcion

START:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; define stack
    mov ss, ax
    mov rsp, 0x6FFFF8
    mov rbp, 0x6FFFF8

    call Main       ; call C Main funcion

    jmp $           ; infinite loop
