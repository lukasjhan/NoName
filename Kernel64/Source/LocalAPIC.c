/* filename          /Kernel64/Source/LocalAPIC.c
 * date              2018.12.18
 * last edit date    2018.12.18
 * author            NO.00[UNKNOWN]
 * brief             source code for Local APIC
*/

#include "LocalAPIC.h"
#include "MPConfigurationTable.h"

#include "LocalAPIC.h"
#include "MPConfigurationTable.h"

QWORD kGetLocalAPICBaseAddress( void )
{
    MPCONFIGURATIONTABLEHEADER* pstMPHeader;
    
    pstMPHeader = kGetMPConfigurationManager()->pstMPConfigurationTableHeader;
    return pstMPHeader->dwMemoryMapIOAddressOfLocalAPIC;
}

void kEnableSoftwareLocalAPIC( void )
{
    QWORD qwLocalAPICBaseAddress;
    
    qwLocalAPICBaseAddress = kGetLocalAPICBaseAddress();
    
    *( DWORD* ) ( qwLocalAPICBaseAddress + APIC_REGISTER_SVR ) |= 0x100;
}

void kSendEOIToLocalAPIC( void )
{
    QWORD qwLocalAPICBaseAddress;
    
    qwLocalAPICBaseAddress = kGetLocalAPICBaseAddress();
    
    *( DWORD* ) ( qwLocalAPICBaseAddress + APIC_REGISTER_EOI ) = 0;
}

void kSetTaskPriority( BYTE bPriority )
{
    QWORD qwLocalAPICBaseAddress;
    
    qwLocalAPICBaseAddress = kGetLocalAPICBaseAddress();
    
    *( DWORD* ) ( qwLocalAPICBaseAddress + APIC_REGISTER_TASKPRIORITY ) = bPriority;
}

void kInitializeLocalVectorTable( void )
{
    QWORD qwLocalAPICBaseAddress;
    DWORD dwTempValue;
    
    qwLocalAPICBaseAddress = kGetLocalAPICBaseAddress();

    *( DWORD* ) ( qwLocalAPICBaseAddress + APIC_REGISTER_TIMER ) |= APIC_INTERRUPT_MASK;
    
    *( DWORD* ) ( qwLocalAPICBaseAddress + APIC_REGISTER_LINT0 ) |= APIC_INTERRUPT_MASK;

    *( DWORD* ) ( qwLocalAPICBaseAddress + APIC_REGISTER_LINT1 ) = APIC_TRIGGERMODE_EDGE | 
        APIC_POLARITY_ACTIVEHIGH | APIC_DELIVERYMODE_NMI;

    *( DWORD* ) ( qwLocalAPICBaseAddress + APIC_REGISTER_ERROR ) |= APIC_INTERRUPT_MASK;

    *( DWORD* ) ( qwLocalAPICBaseAddress + 
            APIC_REGISTER_PERFORMANCEMONITORINGCOUNTER ) |= APIC_INTERRUPT_MASK;

    *( DWORD* ) ( qwLocalAPICBaseAddress + APIC_REGISTER_THERMALSENSOR ) |= 
        APIC_INTERRUPT_MASK;
}