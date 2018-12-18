# filename          /BootLoader/BootLoader.asm
# date              2018.10.30
# last edit date    2018.12.18
# author            NO.00[UNKNOWN]
# brief             Bootloader source code file

[ORG 0x00]          ; CODE START ADDRESS: 0x00
[BITS 16]           ; 16bit CODE

SECTION .text       ; define text segment

jmp 0x07C0:START    ; copy 0x07C0 to CS segment register, move START lable

;   OS environment value
TOTALSECTORCOUNT:      dw      0x02    ; OS image size except bootloader
                                       ; max 1152 sector
KERNEL32SECTORCOUNT:   dw      0x02    ; protected mode kernel sector
BOOTSTRAPPROCESSOR:    db      0x01

;   CODE   ;
START:
    mov ax, 0x07C0  ; move start address:0x07C0 to segment register
    mov ds, ax      ; set DS segment register
    mov ax, 0xB800  ; move start address of video memory:0xB800 to segment register
    mov es, ax      ; set ES segment register

    ;   STACK will set at 0x0000:0000~0x0000:FFFF (64KB)
    mov ax, 0x0000  ; segment register fill with starting address of stack segment value
    mov ss, ax      ; set SS segment register
    mov sp, 0xFFFE  ; 0xFFFE set SP register
    mov bp, 0xFFFE  ; 0xFFFE set BP register

    mov si,     0   ; initialize SI register

.SCREENCLEARLOOP:                   ; loop for clear screen
    mov byte [ es: si ], 0          ; copy 0 to video memory text address to remove text
    mov byte [ es: si + 1], 0x0F    ; copy 0x0F:white to video memory property address to set text color (All color value is in README.md)

    add si, 2                       ; move to next position on screen

    cmp si, 80 * 25 * 2             ; compare si register with screen size:80*25

    jl .SCREENCLEARLOOP             ; if less, jmp SCREENCLEARLOOP

    push BOOTMESSAGE                ; message address push to stack
    push 0                          ; screen y-coordinate push to stack
    push 0                          ; screen x-coordinate push to stack
    call PRINTMESSAGE               ; function called
    add sp, 6                       ; parameters removed

    push IMAGELOADINGMESSAGE        ; message address push to stack
    push 1                          ; screen y-coordinate push to stack
    push 0                          ; screen x-coordinate push to stack
    call PRINTMESSAGE               ; function called
    add sp, 6                       ; parameters removed

    ; Read OS image from disk
RESETDISK:                          ; Disk reset code

    ; BIOS reset function (service number:0, drive number:0=Floppy)
    mov ax, 0
    mov dl, 0
    int 0x13
    jc  HANDLEDISKERROR             ; if error, jump to error handler

    ; set memory address 0x10000 as copy from disk
    mov si, 0x1000
    mov es, si
    mov bx, 0x0000                  ; set address 0x1000:0000

    mov di, word [ TOTALSECTORCOUNT ] ; set DI register disk image sector number

READDATA:                           ; READ DISK
    ; check every sector
    cmp di, 0                       ; compare sector number with 0
    je  READEND                     ; if 0, ends reading
    sub di, 0x1                     ; sub 1 each copy

    ; BIOS read function called
    mov ah, 0x02                    ; BIOS service number 2
    mov al, 0x1                     ; read setcor number 1
    mov ch, byte [ TRACKNUMBER ]    ; set track number
    mov cl, byte [ SECTORNUMBER ]   ; set sector number
    mov dh, byte [ HEADNUMBER ]     ; set head number
    mov dl, 0x00                    ; set drive number
    int 0x13                        ; interrupt service
    jc HANDLEDISKERROR

    ; calculate address, head, sector address
    add si, 0x0020      ; set register 512(0x200)byte
    mov es, si          ; ES segment register add 1 sector
    
    ; read first sector to last(18) sector
    mov al, byte [ SECTORNUMBER ]       ; set AL register sector number
    add al, 0x01                        ; 
    mov byte [ SECTORNUMBER ], al       ; add 1 to sector number
    cmp al, 19                          ; 
    jl READDATA                         ; read all 18 sectors
    
    ; after reading last sector, head toggle
    xor byte [ HEADNUMBER ], 0x01       ; toggle head number
    mov byte [ SECTORNUMBER ], 0x01     ; reset sector number to 1
    
    ; track number add by 1
    cmp byte [ HEADNUMBER ], 0x00       ; compare head number with 0
    jne READDATA                        
    
    add byte [ TRACKNUMBER ], 0x01      ; add track number by 1
    jmp READDATA                        

READEND:                            ; Read disk ends

    push LOADINGCOMPLETEMESSAGE     ; message printing
    push 1                          
    push 20                         
    call PRINTMESSAGE               
    add  sp, 6                      

    jmp 0x1000:0x0000               ; OS will run loaded by disk!!

;   FUNCTION    ;
HANDLEDISKERROR:
    push DISKERRORMESSAGE   ; print error message
    push 1                  
    push 20                 
    call PRINTMESSAGE       
    
    jmp $                   ; infinite loop

PRINTMESSAGE:               ; message print function
    push bp         ; push base pointer register into stack
    mov bp, sp      ; use base pointer register to access parameters

    push es         
    push si         
    push di         
    push ax
    push cx
    push dx
    
    ; video mode address set on ES segment register
    mov ax, 0xB800              ; video memory address
    mov es, ax                  
    
    ; use x,y-coordinate to calculate video memory
    mov ax, word [ bp + 6 ]     
    mov si, 160                 
    mul si                      
    mov di, ax                  
    
    mov ax, word [ bp + 4 ]     
    mov si, 2                   
    mul si                     
    add di, ax                  
      
    mov si, word [ bp + 8 ]     ; 3rd parameter(address of message)


.MESSAGELOOP:                       ; loop for print messages
    mov cl, byte [ si ]             ; copy address of si register to cl register

    cmp cl, 0                       ; compare cl register with 0
    je .MESSAGEEND                  ; if 0, message printing end

    mov byte [ es: di ], cl         ; if not 0, print message to 0xB800:di

    add si, 1                       ; move to next text of message
    add di, 2                       ; move to next address of video memory

    jmp .MESSAGELOOP                ; keep print messages

.MESSAGEEND:
    pop dx      ; used register poped inverse
    pop cx      
    pop ax      
    pop di      
    pop si      
    pop es
    pop bp      ; recover BP register
    ret         ; return


;   DATA    ;
BOOTMESSAGE:      db 'BOOT START!!', 0                     ; message defined
                                                           ; last bit is 0 to identify its end
DISKERRORMESSAGE:   db  'DISK ERROR!!', 0
IMAGELOADINGMESSAGE:    db  'OS Image Loading...', 0
LOADINGCOMPLETEMESSAGE: db  'Complete~!!', 0

SECTORNUMBER:           db  0x02   
HEADNUMBER:             db  0x00    
TRACKNUMBER:            db  0x00    

; rest of memory will 0
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
