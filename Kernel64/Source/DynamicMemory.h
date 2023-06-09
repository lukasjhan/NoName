/* filename          /Kernel64/Source/DynamicMemory.h
 * date              2018.12.04
 * last edit date    2018.12.04
 * author            NO.00[UNKNOWN]
 * brief             header file for DynamicMemory.c
*/

#ifndef __DYNAMICMEMORY_H__
#define __DYNAMICMEMORY_H__

#include "Types.h"
#include "Synchronization.h"

// start address of dynamic memory
#define DYNAMICMEMORY_START_ADDRESS     ( ( TASK_STACKPOOLADDRESS + ( TASK_STACKSIZE * TASK_MAXCOUNT ) + 0xfffff ) & 0xfffffffffff00000 )
// minimal size of buddy block
#define DYNAMICMEMORY_MIN_SIZE          ( 1 * 1024 )

#define DYNAMICMEMORY_EXIST             0x01
#define DYNAMICMEMORY_EMPTY             0x00

typedef struct kBitmapStruct
{
    BYTE* pbBitmap;
    QWORD qwExistBitCount;
} BITMAP;

typedef struct kDynamicMemoryManagerStruct
{
    SPINLOCK stSpinLock;

    int iMaxLevelCount;
    int iBlockCountOfSmallestBlock;
    QWORD qwUsedSize;
    
    QWORD qwStartAddress;
    QWORD qwEndAddress;
    
    BYTE* pbAllocatedBlockListIndex;
    BITMAP* pstBitmapOfLevel;
} DYNAMICMEMORY;

void kInitializeDynamicMemory( void );
void* kAllocateMemory( QWORD qwSize );
BOOL kFreeMemory( void* pvAddress );
void kGetDynamicMemoryInformation( QWORD* pqwDynamicMemoryStartAddress, QWORD* pqwDynamicMemoryTotalSize, QWORD* pqwMetaDataSize, QWORD* pqwUsedMemorySize ); 
DYNAMICMEMORY* kGetDynamicMemoryManager( void );

static QWORD kCalculateDynamicMemorySize( void );
static int kCalculateMetaBlockCount( QWORD qwDynamicRAMSize );
static int kAllocationBuddyBlock( QWORD qwAlignedSize );
static QWORD kGetBuddyBlockSize( QWORD qwSize );
static int kGetBlockListIndexOfMatchSize( QWORD qwAlignedSize );
static int kFindFreeBlockInBitmap( int iBlockListIndex );
static void kSetFlagInBitmap( int iBlockListIndex, int iOffset, BYTE bFlag );
static BOOL kFreeBuddyBlock( int iBlockListIndex, int iBlockOffset );
static BYTE kGetFlagInBitmap( int iBlockListIndex, int iOffset );

#endif /*__DYNAMICMEMORY_H__*/