/* filename          /Kernel64/Source/Descriptor.c
 * date              2018.11.20
 * last edit date    2018.12.18
 * author            NO.00[UNKNOWN]
 * brief             source code for GDT/IDT/Interrupt
*/

#include "Descriptor.h"
#include "Utility.h"
#include "ISR.h"
#include "MultiProcessor.h"

/**
 *  function name : kInitializeGDTTableAndTSS
 *  Parameters    : void
 *  return value  : void
 *  brief         : reset DGT table
 */
void kInitializeGDTTableAndTSS( void )
{
    GDTR*           pstGDTR;
    GDTENTRY8*      pstEntry;
    TSSSEGMENT*     pstTSS;
    int             i;
    
    // GDTR
    pstGDTR                = ( GDTR* ) GDTR_STARTADDRESS;
    pstEntry               = ( GDTENTRY8* ) ( GDTR_STARTADDRESS + sizeof( GDTR ) );
    pstGDTR->wLimit        = GDT_TABLESIZE - 1;
    pstGDTR->qwBaseAddress = ( QWORD ) pstEntry;
    // TSS
    pstTSS                 = ( TSSSEGMENT* ) ( ( QWORD ) pstEntry + GDT_TABLESIZE );

    // NULL, 64bit Code/Data, TSS 4 segment
    kSetGDTEntry8( &( pstEntry[ 0 ] ), 0, 0, 0, 0, 0 );
    kSetGDTEntry8( &( pstEntry[ 1 ] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_CODE, GDT_FLAGS_LOWER_KERNELCODE, GDT_TYPE_CODE );
    kSetGDTEntry8( &( pstEntry[ 2 ] ), 0, 0xFFFFF, GDT_FLAGS_UPPER_DATA, GDT_FLAGS_LOWER_KERNELDATA, GDT_TYPE_DATA );
    
    for ( i = 0 ; i < MAXPROCESSORCOUNT ; i++ )
        kSetGDTEntry16( ( GDTENTRY16* ) &( pstEntry[ GDT_MAXENTRY8COUNT + ( i * 2 ) ] ), ( QWORD ) pstTSS + ( i * sizeof( TSSSEGMENT ) ), sizeof( TSSSEGMENT ) - 1, GDT_FLAGS_UPPER_TSS, GDT_FLAGS_LOWER_TSS, GDT_TYPE_TSS ); 
    
    // TSS init GDT
    kInitializeTSSSegment( pstTSS );
}

/**
 *  function name : kSetGDTEntry8
 *  Parameters    : pstEntry(GDTENTRY8*)
 *                  dwBaseAddress(DWORD)
 *                  dwLimit(DWORD)
 *                  bUpperFlags(BYTE)
 *                  bLowerFlags(BYTE)
 *                  bType(BYTE)
 *  return value  : void
 *  brief         : 8 byte set GDT entry
 */
void kSetGDTEntry8( GDTENTRY8* pstEntry, DWORD dwBaseAddress, DWORD dwLimit, BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType )
{
    pstEntry->wLowerLimit             = dwLimit & 0xFFFF;
    pstEntry->wLowerBaseAddress       = dwBaseAddress & 0xFFFF;
    pstEntry->bUpperBaseAddress1      = ( dwBaseAddress >> 16 ) & 0xFF;
    pstEntry->bTypeAndLowerFlag       = bLowerFlags | bType;
    pstEntry->bUpperLimitAndUpperFlag = ( ( dwLimit >> 16 ) & 0x0F ) | bUpperFlags;
    pstEntry->bUpperBaseAddress2      = ( dwBaseAddress >> 24 ) & 0xFF;
}

/**
 *  function name : kSetGDTEntry16
 *  Parameters    : pstEntry(GDTENTRY8*)
 *                  dwBaseAddress(DWORD)
 *                  dwLimit(DWORD)
 *                  bUpperFlags(BYTE)
 *                  bLowerFlags(BYTE)
 *                  bType(BYTE)
 *  return value  : void
 *  brief         : 16 byte set GDT entry
 */
void kSetGDTEntry16( GDTENTRY16* pstEntry, QWORD qwBaseAddress, DWORD dwLimit, BYTE bUpperFlags, BYTE bLowerFlags, BYTE bType )
{
    pstEntry->wLowerLimit              = dwLimit & 0xFFFF;
    pstEntry->wLowerBaseAddress        = qwBaseAddress & 0xFFFF;
    pstEntry->bMiddleBaseAddress1      = ( qwBaseAddress >> 16 ) & 0xFF;
    pstEntry->bTypeAndLowerFlag        = bLowerFlags | bType;
    pstEntry->bUpperLimitAndUpperFlag  = ( ( dwLimit >> 16 ) & 0xFF ) | bUpperFlags;
    pstEntry->bMiddleBaseAddress2      = ( qwBaseAddress >> 24 ) & 0xFF;
    pstEntry->dwUpperBaseAddress       = qwBaseAddress >> 32;
    pstEntry->dwReserved               = 0;
}

/**
 *  function name : kInitializeTSSSegment
 *  Parameters    : pstTSS(TSSSEGMENT*)
 *  return value  : void
 *  brief         : init TSS segment
 */
void kInitializeTSSSegment( TSSSEGMENT* pstTSS )
{
    int i;
    
    for ( i = 0 ; i < MAXPROCESSORCOUNT ; i++ )
    {
        kMemSet( pstTSS, 0, sizeof( TSSSEGMENT ) );

        pstTSS->qwIST[ 0 ] = IST_STARTADDRESS + IST_SIZE - ( IST_SIZE / MAXPROCESSORCOUNT * i );
        pstTSS->wIOMapBaseAddress = 0xFFFF;

        pstTSS++;
    }
}

/**
 *  function name : kInitializeIDTTables
 *  Parameters    : void
 *  return value  : void
 *  brief         : init IDT table
 */
void kInitializeIDTTables( void )
{
    IDTR*       pstIDTR;
    IDTENTRY*   pstEntry;
    int         i;
        
    // IDTR start address
    pstIDTR                 = ( IDTR* ) IDTR_STARTADDRESS;
    // IDT table info
    pstEntry                = ( IDTENTRY* ) ( IDTR_STARTADDRESS + sizeof( IDTR ) );
    pstIDTR->qwBaseAddress  = ( QWORD ) pstEntry;
    pstIDTR->wLimit         = IDT_TABLESIZE - 1;
    
    // set Exception ISR
    kSetIDTEntry( &( pstEntry[ 0 ] ), kISRDivideError, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 1 ] ), kISRDebug, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 2 ] ), kISRNMI, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 3 ] ), kISRBreakPoint, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 4 ] ), kISROverflow, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 5 ] ), kISRBoundRangeExceeded, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 6 ] ), kISRInvalidOpcode, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 7 ] ), kISRDeviceNotAvailable, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 8 ] ), kISRDoubleFault, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 9 ] ), kISRCoprocessorSegmentOverrun, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 10 ] ), kISRInvalidTSS, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 11 ] ), kISRSegmentNotPresent, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 12 ] ), kISRStackSegmentFault, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 13 ] ), kISRGeneralProtection, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 14 ] ), kISRPageFault, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 15 ] ), kISR15, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 16 ] ), kISRFPUError, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 17 ] ), kISRAlignmentCheck, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 18 ] ), kISRMachineCheck, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 19 ] ), kISRSIMDError, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 20 ] ), kISRETCException, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );

    for( i = 21 ; i < 32 ; i++ )
        kSetIDTEntry( &( pstEntry[ i ] ), kISRETCException, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    
    // set Interrupt ISR
    kSetIDTEntry( &( pstEntry[ 32 ] ), kISRTimer, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 33 ] ), kISRKeyboard, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 34 ] ), kISRSlavePIC, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 35 ] ), kISRSerial2, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 36 ] ), kISRSerial1, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 37 ] ), kISRParallel2, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 38 ] ), kISRFloppy, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 39 ] ), kISRParallel1, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 40 ] ), kISRRTC, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 41 ] ), kISRReserved, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 42 ] ), kISRNotUsed1, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 43 ] ), kISRNotUsed2, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 44 ] ), kISRMouse, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 45 ] ), kISRCoprocessor, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 46 ] ), kISRHDD1, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
    kSetIDTEntry( &( pstEntry[ 47 ] ), kISRHDD2, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );

    for( i = 48 ; i < IDT_MAXENTRYCOUNT ; i++ )
        kSetIDTEntry( &( pstEntry[ i ] ), kISRETCInterrupt, 0x08, IDT_FLAGS_IST1, IDT_FLAGS_KERNEL, IDT_TYPE_INTERRUPT );
}

/**
 *  function name : kSetIDTEntry
 *  Parameters    : pstEntry(GDTENTRY8*)
 *                  pvHandler(void*)
 *                  wSelector(WORD)
 *                  bIST(BYTE)
 *                  bFlags(BYTE)
 *                  bType(BYTE)
 *  return value  : void
 *  brief         : set IDT entry
 */
void kSetIDTEntry( IDTENTRY* pstEntry, void* pvHandler, WORD wSelector, BYTE bIST, BYTE bFlags, BYTE bType )
{
    pstEntry->wLowerBaseAddress     = ( QWORD ) pvHandler & 0xFFFF;
    pstEntry->wSegmentSelector      = wSelector;
    pstEntry->bIST                  = bIST & 0x3;
    pstEntry->bTypeAndFlags         = bType | bFlags;
    pstEntry->wMiddleBaseAddress    = ( ( QWORD ) pvHandler >> 16 ) & 0xFFFF;
    pstEntry->dwUpperBaseAddress    = ( QWORD ) pvHandler >> 32;
    pstEntry->dwReserved            = 0;
}