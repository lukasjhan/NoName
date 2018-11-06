# filename          /Kernel32/Source/EntryPoint.s
# date              2018.11.05
# last edit date    2018.11.05
# author            NO.00[UNKNOWN]
# brief             define entry point for kernel32 c function

[ORG 0x00]          ; CODE START ADDRESS: 0x00
[BITS 16]           ; 16bit CODE

SECTION .text       ; define text segment

;   CODE    ;
START:
    mov ax, 0x1000  ; segment register set protected mode start address(0x10000)
    mov ds, ax      ; set DS register
    mov es, ax      ; set ES register

    ; A20 gate activate
    mov ax, 0x2401  ; A20 gate activate service setting
    int 0x15        ; BIOS service called

    jc A20GATEERROR ; check A20 gate activation success
    jmp A20GATESUCCESS

A20GATEERROR:
    ; if error, use system control port
    in al, 0x92     ; read 1 byte from system control port(0x92)
    or al, 0x02     ; A20 gate bit set 1
    and al, 0xFE    ; set bit 0 to 0
    out 0x92, al    ; set system control port

A20GATESUCCESS:

    cli             ; set no interrupts
    lgdt [ GDTR ]   ; load GDT table

    ; Entering Protected mode
    ; Disable Paging, Disable Cache, Internal FPU, Disable Align Check,
    ; Enable Protected mode

    mov eax, 0x4000003B ; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
    mov cr0, eax        ; Flags is set on CR0 control register

    ; SC segment selector : EIP, reset by 0x00
    jmp dword 0x08: ( PROTECTEDMODE - $$ + 0x10000 )

; Enter Protected mode
[BITS 32]                   ; 16bit CODE
PROTECTEDMODE:
    mov ax, 0x10            ; data segment descriptor stored in ax register
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax              ; segment selector initialized

    ; init stack
    mov ss, ax
    mov esp, 0xFFFE
    mov ebp, 0xFFFE

    ; print message on screen
    push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000 )
    push 2
    push 0
    call PRINTMESSAGE
    add esp, 12

    jmp dword 0x08: 0x10200  ; jump to C kernel main entry point

;   FUNCTION    ;
PRINTMESSAGE:
    push ebp
    mov ebp, esp
    push esi
    push edi
    push eax
    push ecx
    push edx

    mov eax, dword [ ebp + 12 ]
    mov esi, 160
    mul esi
    mov edi, eax
    
    mov eax, dword [ ebp + 8 ]
    mov esi, 2
    mul esi
    add edi, eax

    mov esi, dword [ ebp + 16 ]

MESSAGELOOP:               
    mov cl, byte [ esi ]

    cmp cl, 0
    je MESSAGEEND

    mov byte [ edi + 0xB8000 ], cl
    
    add esi, 1
    add edi, 2

    jmp MESSAGELOOP

MESSAGEEND:
    pop edx
    pop ecx
    pop eax
    pop edi
    pop esi
    pop ebp
    ret

;   DATA  ;
; align data by 8byte
align 8, db 0

; end of GDTR align by 8byte
dw 0x0000

GDTR:
    dw GDTEND - GDT - 1         ; size of GDT table
    dd ( GDT - $$ + 0x10000 )   ; start address of GDT table

GDT:
    ; NULL descriptor must init 0
    NULLDescriptor:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00
    
    ; Code segment descriptor for protected mode
    CODEDESCRIPTOR:     
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x9A         ; P=1, DPL=0, Code Segment, Execute/Read
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]
    
    ; data segment descriptor for protected mode
    DATADESCRIPTOR:
        dw 0xFFFF       ; Limit [15:0]
        dw 0x0000       ; Base [15:0]
        db 0x00         ; Base [23:16]
        db 0x92         ; P=1, DPL=0, Data Segment, Read/Write
        db 0xCF         ; G=1, D=1, L=0, Limit[19:16]
        db 0x00         ; Base [31:24]
GDTEND:

SWITCHSUCCESSMESSAGE: db 'Switch to Protected mode success!!', 0

; rest of memory fill with 0
times 512 - ( $ - $$ ) db 0x00