# filename          /Kernel64/Source/AssemblyUtility.asm
# date              2018.11.09
# last edit date    2018.11.09
# author            NO.00[UNKNOWN]
# brief             Assembly utility functions

[BITS 64]           

SECTION .text       

global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR

; function name : kInPortByte
; parametor     : port number
; brief         : Read 1 byte from port
kInPortByte:
    push rdx

    mov rdx, rdi    ; save parameter(port number) into RDX register
    mov rax, 0      ; reset RAX register
    in al, dx       ; read 1 byte from address that DX register point to
                    ; save into AL register for return value

    pop rdx
    ret
    
; function name : kInPortByte
; parametor     : port number, data
; brief         : Write 1 byte into port
kOutPortByte:
    push rdx
    push rax
    
    mov rdx, rdi    ; save 1st parametor(port number) into RDX register
    mov rax, rsi    ; save 1st parametor(data) into RAX register
    out dx, al      ; Write 1 byte that is in AL register into address that dx register point to

    pop rax
    pop rdx
    ret

; function name : kLoadGDTR
; parametor     : GDT table address
; brief         : GDTR register is set by GDT table
kLoadGDTR:
    lgdt [ rdi ]
    ret

; function name : kLoadTR
; parametor     : TSS segment offset
; brief         : TR register is set by TSS segment
kLoadTR:
    ltr di
    ret

; function name : kLoadIDTR
; parametor     : IDT table address
; brief         : IDTR register is set by IDT table
kLoadIDTR:
    lidt [ rdi ]
    ret