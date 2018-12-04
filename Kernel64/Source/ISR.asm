# filename          /Kernel64/Source/ISR.asm
# date              2018.11.09
# last edit date    2018.11.28
# author            NO.00[UNKNOWN]
# brief             Assembly handler functions

[BITS 64]           

SECTION .text       

; Import
extern kCommonExceptionHandler, kCommonInterruptHandler, kKeyboardHandler
extern kTimerHandler, kDeviceNotAvailableHandler, kHDDHandler

; Exception ISR
global kISRDivideError, kISRDebug, kISRNMI, kISRBreakPoint, kISROverflow
global kISRBoundRangeExceeded, kISRInvalidOpcode, kISRDeviceNotAvailable, kISRDoubleFault,
global kISRCoprocessorSegmentOverrun, kISRInvalidTSS, kISRSegmentNotPresent
global kISRStackSegmentFault, kISRGeneralProtection, kISRPageFault, kISR15
global kISRFPUError, kISRAlignmentCheck, kISRMachineCheck, kISRSIMDError, kISRETCException

; Interrupt ISR
global kISRTimer, kISRKeyboard, kISRSlavePIC, kISRSerial2, kISRSerial1, kISRParallel2
global kISRFloppy, kISRParallel1, kISRRTC, kISRReserved, kISRNotUsed1, kISRNotUsed2
global kISRMouse, kISRCoprocessor, kISRHDD1, kISRHDD2, kISRETCInterrupt

; macro for saving context and replace selector
%macro KSAVECONTEXT 0       ; NO PARAM
    ; push RBP register ~ GS segment selector into stack
    push rbp
    mov rbp, rsp
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

    ; replace segment selector
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
%endmacro


; restore context
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

; Exception Handler

; #0, Divide Error ISR
kISRDivideError:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 0
    call kCommonExceptionHandler

    KLOADCONTEXT
    iretq           ; restore

; #1, Debug ISR
kISRDebug:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 1
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #2, NMI ISR
kISRNMI:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 2
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #3, BreakPoint ISR
kISRBreakPoint:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 3
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #4, Overflow ISR
kISROverflow:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 4
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #5, Bound Range Exceeded ISR
kISRBoundRangeExceeded:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 5
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #6, Invalid Opcode ISR
kISRInvalidOpcode:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 6
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #7, Device Not Available ISR
kISRDeviceNotAvailable:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 7
    call kDeviceNotAvailableHandler

    KLOADCONTEXT    
    iretq           ; restore

; #8, Double Fault ISR
kISRDoubleFault:
    KSAVECONTEXT    ; save context

    ; insert exception number and error code into handler and call it
    mov rdi, 8
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    
    add rsp, 8      ; remove error code from stack
    iretq           ; restore

; #9, Coprocessor Segment Overrun ISR
kISRCoprocessorSegmentOverrun:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 9
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #10, Invalid TSS ISR
kISRInvalidTSS:
    KSAVECONTEXT    ; save context

    ; insert exception number and error code into handler and call it
    mov rdi, 10
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    
    add rsp, 8      ; remove error code from stack
    iretq           ; restore

; #11, Segment Not Present ISR
kISRSegmentNotPresent:
    KSAVECONTEXT    ; save context

    ; insert exception number and error code into handler and call it
    mov rdi, 11
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    
    add rsp, 8      ; remove error code from stack
    iretq           ; restore

; #12, Stack Segment Fault ISR
kISRStackSegmentFault:
    KSAVECONTEXT    ; save context

    ; insert exception number and error code into handler and call it
    mov rdi, 12
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    
    add rsp, 8      ; remove error code from stack
    iretq           ; restore

; #13, General Protection ISR
kISRGeneralProtection:
    KSAVECONTEXT    ; save context

    ; insert exception number and error code into handler and call it
    mov rdi, 13
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    
    add rsp, 8      ; remove error code from stack
    iretq           ; restore

; #14, Page Fault ISR
kISRPageFault:
    KSAVECONTEXT    ; save context

    ; insert exception number and error code into handler and call it
    mov rdi, 14
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    
    add rsp, 8      ; remove error code from stack
    iretq           ; restore

; #15, Reserved ISR
kISR15:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 15
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #16, FPU Error ISR
kISRFPUError:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 16
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #17, Alignment Check ISR
kISRAlignmentCheck:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 17
    mov rsi, qword [ rbp + 8 ]
    call kCommonExceptionHandler

    KLOADCONTEXT    
    add rsp, 8      ; remove error code from stack
    iretq           ; restore

; #18, Machine Check ISR
kISRMachineCheck:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 18
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #19, SIMD Floating Point Exception ISR
kISRSIMDError:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 19
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore

; #20~#31, Reserved ISR
kISRETCException:
    KSAVECONTEXT    ; save context

    ; insert exception number into handler and call it
    mov rdi, 20
    call kCommonExceptionHandler

    KLOADCONTEXT    
    iretq           ; restore


; Interrupt Handler

; #32, Timer ISR
kISRTimer:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 32
    call kTimerHandler

    KLOADCONTEXT    
    iretq           ; restore

; #33, Keyboard ISR
kISRKeyboard:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 33
    call kKeyboardHandler

    KLOADCONTEXT    
    iretq           ; restore

; #34, slave PIC ISR
kISRSlavePIC:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 34
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #35, serial 2 ISR
kISRSerial2:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 35
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #36, serial port 1 ISR
kISRSerial1:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 36
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #37, parallel port 2 ISR
kISRParallel2:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 37
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #38, floppy disk controller ISR
kISRFloppy:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 38
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #39, parallel port 1 ISR
kISRParallel1:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 39
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #40, RTC ISR
kISRRTC:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 40
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #41, reserved interrupt ISR
kISRReserved:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 41
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #42, NOT USE
kISRNotUsed1:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 42
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #43, NOT USE
kISRNotUsed2:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 43
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #44, mouse ISR
kISRMouse:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 44
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #45, coprocessor ISR
kISRCoprocessor:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 45
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore

; #46, HDD 1 ISR
kISRHDD1:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 46
    call kHDDHandler

    KLOADCONTEXT    
    iretq           ; restore

; #47, HDD 2 ISR
kISRHDD2:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 47
    call kHDDHandler

    KLOADCONTEXT    
    iretq           ; restore

; #48 rest of interrupt ISR
kISRETCInterrupt:
    KSAVECONTEXT    ; save context

    ; insert Interrupt number into handler and call it
    mov rdi, 48
    call kCommonInterruptHandler

    KLOADCONTEXT    
    iretq           ; restore