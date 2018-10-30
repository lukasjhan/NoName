# file              /BootLoader/BootLoader.asm
# date              2018.10.30
# last edit date    2018.10.30
# author            NO.00[UNKNOWN]
# breif             Bootloader source code file

[ORG 0x00]          ; CODE START ADDRESS: 0x00
[BITS 16]           ; 16bit CODE

SECTION .text       ; define text segment

jmp 0x07C0:START    ; copy 0x07C0 to CS segment register, move START lable

START:
    mov ax, 0x07C0  ; move start address:0x07C0 to segment register
    mov ds, ax      ; set DS segment register
    mov ax, 0xB800  ; move start address of video memory:0xB800 to segment register
    mov es, ax      ; set ES segment register

    mov si,     0   ; initialize SI register

.SCREENCLEARLOOP:                   ; loop for clear screen
    mov byte [ es: si ], 0          ; copy 0 to video memory text address to remove text
    mov byte [ es: si + 1], 0x0F    ; copy 0x0F:white to video memory property address to set text color (All color value is in README.md)

    add si, 2                       ; move to next position on screen

    cmp si, 80 * 25 * 2             ; compare si register with screen size:80*25

    jl .SCREENCLEARLOOP             ; if less, jmp .SCREENCLEARLOOP

    mov si, 0                       ; initialize si register
    mov di, 0                       ; initialize di register

.MESSAGELOOP:                       ; loop for print messages
    mov cl, byte [ si + MESSAGE1]   ; copy address of MESSAGE1 + si register to cl register

    cmp cl, 0                       ; compare cl register with 0
    je .MESSAGEEND                  ; if 0, message printing end

    mov byte [ es: di ], cl         ; if not 0, print message to 0xB800:di

    add si, 1                       ; move to next text of message
    add di, 2                       ; move to next address of video memory

    jmp .MESSAGELOOP                ; keep print messages

.MESSAGEEND:
    jmp $               ; infinite loop

MESSAGE1:      db 'BOOT START!!', 0     ; message defined
                                        ; last bit is 0 to identify its end

times 510 - ( $ - $$ )    db    0x00    ; $: address in current line
                                        ; $$: current section address(.text)
                                        ; $ - $$: offset based on current section
                                        ; 510 - ( $ - $$ ): current address to address 510
                                        ; db 0x00: declare 1byte 0x00
                                        ; time: loop
                                        ; current address to address 510(unused memory) will fill with 0x00

db 0x55             ; declare 1byte with 0x55
db 0xAA             ; declare 1byte with 0xAA
                    ; address 511,512 fill with 0x55, 0xAA to declare it is a bootsector.
