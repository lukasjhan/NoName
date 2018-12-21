# filename          /Kernel64/Source/EntryPoint.s
# date              2018.11.09
# last edit date    2018.12.18
# author            NO.00[UNKNOWN]
# brief             entry point for 64bit mode kernel

[BITS 64]           ; 64bit code

SECTION .text

extern Main         ; import Main funcion
extern g_qwAPICIDAddress, g_iWakeUpApplicationProcessorCount

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

    cmp byte [ 0x7C09 ], 0x01
    je .BOOTSTRAPPROCESSORSTARTPOINT

    mov rax, 0                              
    mov rbx, qword [ g_qwAPICIDAddress ] 
    mov eax, dword [ rbx ] 
    shr rax, 24            
    
    mov rbx, 0x10000       
    mul rbx               
    
    sub rsp, rax  
    sub rbp, rax   

    lock inc dword [ g_iWakeUpApplicationProcessorCount ]
    
.BOOTSTRAPPROCESSORSTARTPOINT:
    call Main      
    
    jmp $