# file              /BootLoader/BootLoader.asm
# date              2018.10.30
# last edit date    2018.10.30
# author            NO.00[UNKNOWN]
# breif             Bootloader source code file

[ORG 0x00]          ; CODE START ADDRESS: 0x00
[BITS 16]           ; 16bit CODE

SECTION .text       ; text segment define

jmp $               ; infinite loop
                    ; TODO: BOOTSECTOR CODE will be added here!

times 510 - ( $ - $$ )    db    0x00    ; $: address in current line
                                        ; $$: current section address(.text)
                                        ; $ - $$: offset based on current section
                                        ; 510 - ( $ - $$ ): current address to address 510
                                        ; db 0x00: declare 1byte 0x00
                                        ; time: loop
                                        ; current address to address 510 will fill with 0x00

db 0x55             ; declare 1byte with 0x55
db 0xAA             ; declare 1byte with 0xAA
                    ; address 511,512 fill with 0x55, 0xAA to declare it is a bootsector.
