# filename          /Kernel64/Source/AssemblyUtility.asm
# date              2018.11.09
# last edit date    2018.11.28
# author            NO.00[UNKNOWN]
# brief             Assembly utility functions

[BITS 64]           

SECTION .text       

global kInPortByte, kOutPortByte, kInPortWord, kOutPortWord
global kLoadGDTR, kLoadTR, kLoadIDTR
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS
global kReadTSC
global kSwitchContext, kHlt, kTestAndSet, kPause
global kInitializeFPU, kSaveFPUContext, kLoadFPUContext, kSetTS, kClearTS
global kEnableGlobalLocalAPIC

; function name : kInPortByte
; parameter     : port number
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
; parameter     : port number, data
; brief         : Write 1 byte into port
kOutPortByte:
    push rdx
    push rax
    
    mov rdx, rdi    ; save 1st parameter(port number) into RDX register
    mov rax, rsi    ; save 1st parameter(data) into RAX register
    out dx, al      ; Write 1 byte that is in AL register into address that dx register point to

    pop rax
    pop rdx
    ret

; function name : kInPortWord
; parameter     : port number, data
; brief         : read 2 byte into port
kInPortWord:
    push rdx        
    
    mov rdx, rdi
    mov rax, 0
    in ax, dx
    
    pop rdx
    ret
    
; function name : kOutPortWord
; parameter     : port number, data
; brief         : Write 2 byte into port
kOutPortWord:
    push rdx
    push rax
    
    mov rdx, rdi
    mov rax, rsi  
    out dx, ax
    
    pop rax
    pop rdx
    ret

; function name : kLoadGDTR
; parameter     : GDT table address
; brief         : GDTR register is set by GDT table
kLoadGDTR:
    lgdt [ rdi ]
    ret

; function name : kLoadTR
; parameter     : TSS segment offset
; brief         : TR register is set by TSS segment
kLoadTR:
    ltr di
    ret

; function name : kLoadIDTR
; parameter     : IDT table address
; brief         : IDTR register is set by IDT table
kLoadIDTR:
    lidt [ rdi ]
    ret

; function name : kEnableInterrupt
; parameter     : void
; brief         : enable interrupt
kEnableInterrupt:
    sti             ; enable interrupt
    ret

; function name : kDisableInterrupt
; parameter     : void
; brief         : disable interrupt
kDisableInterrupt:
    cli             ; disable interrupt
    ret

; function name : kDisableInterrupt
; parameter     : void
; brief         : return RFLAGS register
kReadRFLAGS:
    pushfq                  ; save RFLAGS register into stack
    pop rax                 ; save RFLAGS register int RAX register to set return value
    ret

; function name : kReadTSC
; parameter     : NONE
; brief         : Return Timestamp counter
kReadTSC:
    push rdx
    
    rdtsc                   ; Read Timestamp data and store it into RDX:RAX
    
    shl rdx, 32
    or rax, rdx
    
    pop rdx
    ret

;   Task

; MACRO for saving context and switching selector
%macro KSAVECONTEXT 0   ; No parameter
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    mov ax, ds
    push rax
    mov ax, es
    push rax
    push fs
    push gs 
%endmacro


; macro for restoring context
%macro KLOADCONTEXT 0
    pop gs
    pop fs
    pop rax
    mov es, ax
    pop rax
    mov ds, ax
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp        
%endmacro

; function name : kSwitchContext
; parameter     : Current Context, Next Context
; brief         : save current context, restore next task
kSwitchContext:
    push rbp        
    mov rbp, rsp
    
    ; Current Context = NULL no need to save context
    pushfq
    cmp rdi, 0
    je .LoadContext 
    popfq

    ; save context

    push rax
    
    mov ax, ss                          
    mov qword[ rdi + ( 23 * 8 ) ], rax

    mov rax, rbp                        
    add rax, 16                         
    mov qword[ rdi + ( 22 * 8 ) ], rax
    
    pushfq                              
    pop rax
    mov qword[ rdi + ( 21 * 8 ) ], rax

    mov ax, cs                          
    mov qword[ rdi + ( 20 * 8 ) ], rax
    
    mov rax, qword[ rbp + 8 ]           
    mov qword[ rdi + ( 19 * 8 ) ], rax
    
    pop rax
    pop rbp
    
    add rdi, ( 19 * 8 )
    mov rsp, rdi
    sub rdi, ( 19 * 8 )
    
    KSAVECONTEXT

    ; restore next task

.LoadContext:
    mov rsp, rsi
    
    KLOADCONTEXT
    iretq

; function name : kHlt
; parameter     : NONE
; brief         : hlt cpu
kHlt:
    hlt
    hlt
    ret

; function name : kTestAndSet
; parameter     : Destination address, Compare data, source data
; brief         : test and set
kTestAndSet:
    mov rax, rsi
  
    lock cmpxchg byte [ rdi ], dl   
    je .SUCCESS         

.NOTSAME:               ; Destination != Compare
    mov rax, 0x00
    ret
    
.SUCCESS:               ; Destination = Compare
    mov rax, 0x01
    ret

; FPU

; function name : kInitializeFPU
; parameter     : NONE
; brief         : init FPU
kInitializeFPU:
    finit
    ret

; function name : kSaveFPUContext
; parameter     : Buffer Address
; brief         : save register data in context buffer
kSaveFPUContext:
    fxsave  [ rdi ]
    ret

; function name : kLoadFPUContext
; parameter     : Buffer Address
; brief         : restore register from context buffer
kLoadFPUContext:
    fxrstor [ rdi ]
    ret

; function name : kSetTS
; parameter     : NONE
; brief         : TS bit set 1
kSetTS:
    push rax

    mov rax, cr0
    or rax, 0x08
    mov cr0, rax

    pop rax
    ret
    
; function name : kSetTS
; parameter     : NONE
; brief         : TS bit set 0
kClearTS:
    clts
    ret    

; function name : kEnableGlobalLocalAPIC
; parameter     : NONE
; brief         : enable local APIC
kEnableGlobalLocalAPIC:
    push rax            
    push rcx
    push rdx
    
    mov rcx, 27
    rdmsr               
            
    or eax, 0x0800 
    wrmsr        
        
    pop rdx     
    pop rcx
    pop rax
    ret

kPause:
    pause
    ret