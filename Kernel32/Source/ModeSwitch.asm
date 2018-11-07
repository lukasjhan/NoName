# filename          /Kernel32/Source/ModeSwitch.asm
# date              2018.11.07
# last edit date    2018.11.07
# author            NO.00[UNKNOWN]
# brief             define functions to switch to 64bit mode

[BITS 32]

global kReadCPUID, kSwitchAndExecute64bitKernel

SECTION .text

;   Function name: kReadCPUID
;   PARAM: DWORD dwEAX, DWORD* pdwEAX,* pdwEBX,* pdwECX,* pdwEDX
;   Brief: return CPUID
kReadCPUID:
    push ebp        
    mov ebp, esp    
    push eax        
    push ebx        
    push ecx
    push edx
    push esi

    mov eax, dword [ ebp + 8 ]  
    cpuid                       ; command cpuid
    
    ; save return data to parameters
    ; *pdwEAX
    mov esi, dword [ ebp + 12 ] 
    mov dword [ esi ], eax      
                                
    ; *pdwEBX
    mov esi, dword [ ebp + 16 ] 
    mov dword [ esi ], ebx

    ; *pdwECX
    mov esi, dword [ ebp + 20 ] 
    mov dword [ esi ], ecx
                                
    ; *pdwEDX
    mov esi, dword [ ebp + 24 ]
    mov dword [ esi ], edx

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret

;   switch to IA-32e mode
;   PARAM: NONE
kSwitchAndExecute64bitKernel:
    ; set CR4 control register PAE bit to 1
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax
    
    ; address PML4 table and enable cache
    mov eax, 0x100000
    mov cr3, eax        ; save 0x100000(1MB) into CR3 control register
    
    ; IA32_EFER.LME set 1 to enable IA-32e mode
    mov ecx, 0xC0000080
    rdmsr
    
    or eax, 0x0100
    wrmsr

    ; enable cache and paging
    mov eax, cr0
    or eax, 0xE0000000
    xor eax, 0x60000000 
    mov cr0, eax
    
    ; jump to 64bit kernel
    jmp 0x08:0x200000
   
    ; never get here.             
    jmp $