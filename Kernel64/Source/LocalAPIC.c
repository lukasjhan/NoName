/* filename          /Kernel64/Source/LocalAPIC.c
 * date              2018.12.18
 * last edit date    2018.12.18
 * author            NO.00[UNKNOWN]
 * brief             source code for Local APIC
*/

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