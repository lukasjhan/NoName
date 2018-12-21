/* filename          /Kernel64/Source/MPConfigurationTable.h
 * date              2018.12.11
 * last edit date    2018.12.11
 * author            NO.00[UNKNOWN]
 * brief             header file for MPConfigurationTable.c
*/

#ifndef __MPCONFIGURATIONTABLE__
#define __MPCONFIGURATIONTABLE__

#include "Types.h"

// MP Feature Byte
#define MP_FLOATINGPOINTER_FEATUREBYTE1_USEMPTABLE  0x00
#define MP_FLOATINGPOINTER_FEATUREBYTE2_PICMODE     0x80

// Entry Type
#define MP_ENTRYTYPE_PROCESSOR                  0
#define MP_ENTRYTYPE_BUS                        1
#define MP_ENTRYTYPE_IOAPIC                     2
#define MP_ENTRYTYPE_IOINTERRUPTASSIGNMENT      3
#define MP_ENTRYTYPE_LOCALINTERRUPTASSIGNMENT   4

// cpu flag
#define MP_PROCESSOR_CPUFLAGS_ENABLE            0x01
#define MP_PROCESSOR_CPUFLAGS_BSP               0x02

// Bus Type String
#define MP_BUS_TYPESTRING_ISA                   "ISA"
#define MP_BUS_TYPESTRING_PCI                   "PCI"
#define MP_BUS_TYPESTRING_PCMCIA                "PCMCIA"
#define MP_BUS_TYPESTRING_VESALOCALBUS          "VL"

// Interrupt Type
#define MP_INTERRUPTTYPE_INT                    0
#define MP_INTERRUPTTYPE_NMI                    1
#define MP_INTERRUPTTYPE_SMI                    2
#define MP_INTERRUPTTYPE_EXTINT                 3

// Interrupt Flags
#define MP_INTERRUPT_FLAGS_CONFORMPOLARITY      0x00
#define MP_INTERRUPT_FLAGS_ACTIVEHIGH           0x01
#define MP_INTERRUPT_FLAGS_ACTIVELOW            0x03
#define MP_INTERRUPT_FLAGS_CONFORMTRIGGER       0x00
#define MP_INTERRUPT_FLAGS_EDGETRIGGERED        0x04
#define MP_INTERRUPT_FLAGS_LEVELTRIGGERED       0x0C

#pragma pack( push, 1 )

// MP Floating Pointer Data Structure
typedef struct kMPFloatingPointerStruct
{
    char vcSignature[ 4 ]; 
    DWORD dwMPConfigurationTableAddress;
    BYTE bLength;
    BYTE bRevision;
    BYTE bCheckSum;
    BYTE vbMPFeatureByte[ 5 ];
} MPFLOATINGPOINTER;

// MP Configuration Table Header
typedef struct kMPConfigurationTableHeaderStruct
{
    char vcSignature[ 4 ];
    WORD wBaseTableLength;
    BYTE bRevision;
    BYTE bCheckSum;
    char vcOEMIDString[ 8 ];
    char vcProductIDString[ 12 ];
    DWORD dwOEMTablePointerAddress;
    WORD wOEMTableSize;
    WORD wEntryCount;
    DWORD dwMemoryMapIOAddressOfLocalAPIC;
    WORD wExtendedTableLength;
    BYTE bExtendedTableChecksum;
    BYTE bReserved;
} MPCONFIGURATIONTABLEHEADER;

// Processor Entry
typedef struct kProcessorEntryStruct
{
    BYTE bEntryType;
    BYTE bLocalAPICID;
    BYTE bLocalAPICVersion;
    BYTE bCPUFlags;
    BYTE vbCPUSignature[ 4 ];
    DWORD dwFeatureFlags;
    DWORD vdwReserved[ 2 ];
} PROCESSORENTRY;

// Bus Entry
typedef struct kBusEntryStruct
{
    BYTE bEntryType;
    BYTE bBusID;
    char vcBusTypeString[ 6 ];
} BUSENTRY;

// I/O APIC Entry
typedef struct kIOAPICEntryStruct
{
    BYTE bEntryType;
    BYTE bIOAPICID;
    BYTE bIOAPICVersion;
    BYTE bIOAPICFlags;
    DWORD dwMemoryMapAddress;
} IOAPICENTRY;

// I/O Interrupt Assignment Entry
typedef struct kIOInterruptAssignmentEntryStruct
{
    BYTE bEntryType;
    BYTE bInterruptType;
    WORD wInterruptFlags;
    BYTE bSourceBUSID;
    BYTE bSourceBUSIRQ;
    BYTE bDestinationIOAPICID;
    BYTE bDestinationIOAPICINTIN;
} IOINTERRUPTASSIGNMENTENTRY;

// Local Interrupt Assignment Entry
typedef struct kLocalInterruptEntryStruct
{
    BYTE bEntryType;
    BYTE bInterruptType;
    WORD wInterruptFlags;
    BYTE bSourceBUSID;
    BYTE bSourceBUSIRQ;
    BYTE bDestinationLocalAPICID;
    BYTE bDestinationLocalAPICLINTIN;
} LOCALINTERRUPTASSIGNMENTENTRY;

#pragma pack( pop)

typedef struct kMPConfigurationManagerStruct
{
    MPFLOATINGPOINTER* pstMPFloatingPointer;
    MPCONFIGURATIONTABLEHEADER* pstMPConfigurationTableHeader;
    QWORD qwBaseEntryStartAddress;
    int iProcessorCount;
    BOOL bUsePICMode;
    BYTE bISABusID;
} MPCONFIGRUATIONMANAGER;

BOOL kFindMPFloatingPointerAddress( QWORD* pstAddress );
BOOL kAnalysisMPConfigurationTable( void );
MPCONFIGRUATIONMANAGER* kGetMPConfigurationManager( void );
void kPrintMPConfigurationTable( void );
int kGetProcessorCount( void );
IOAPICENTRY* kFindIOAPICEntryForISA( void );

#endif /*__MPCONFIGURATIONTABLE__*/