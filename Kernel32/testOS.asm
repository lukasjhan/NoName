# filename          /Kernel32/testOS.asm
# date              2018.10.31
# last edit date    2018.10.31
# author            NO.00[UNKNOWN]
# brief             temp os for test bootloader

[ORG 0x00]          ; CODE START ADDRESS: 0x00
[BITS 16]           ; 16bit CODE

SECTION .text       ; define text segment

jmp 0x1000:START    ; copy 0x1000 to CS segment register, move START lable

SECTORCOUNT:        dw  0x0000      ; sector number
TOTALSECTORCOUNT    equ 1024        ; total sectors max 1152

;   CODE    ;
START:
    mov ax, cs                  ; set ax register with cs register
    mov ds, ax                  ; set ds register with ax register
    mov ax, 0xB800              ; video memory address
    mov es, ax                  ; set es register

    %assign i   0               ; variable i
    %rep TOTALSECTORCOUNT       ; loop for TOTALSECTORCOUNT
        %assign i   i + 1       ; add 1 i
    
        mov ax, 2                       ; 2 byte for 1 char
        mul word [ SECTORCOUNT ]        ; ax register * sector number
        mov si, ax                      ; result move in si register

        ; print value in screen
        mov byte [ es: si + ( 160 * 2 ) ], '0' + ( i % 10 )
        add word [ SECTORCOUNT ], 1     ; add 1 sector

        ; if last sector, infinite loop
        ; move to next sector
        %if i == TOTALSECTORCOUNT       ; if last sector
            jmp $                       ; infinite loop
        %else                           ; if not last sector
            jmp ( 0x1000 + i * 0x20 ): 0x0000   ; move to next offset
        %endif                          ; end of if
        
        times ( 512 - ( $ - $$ ) % 512 )    db 0x00    ; rest of memory fill with 0
                            
    %endrep                 ; end of loop