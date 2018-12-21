/* filename          /Kernel64/Source/Synchronization.h
 * date              2018.11.28
 * last edit date    2018.11.28
 * author            NO.00[UNKNOWN]
 * brief             header file for Synchronization.c
*/

#ifndef __SYNCHRONIZATION_H__
#define __SYNCHRONIZATION_H__

#include "Types.h"

#pragma pack( push, 1 )

typedef struct kMutexStruct
{
    volatile QWORD qwTaskID;
    volatile DWORD dwLockCount;

    // lock flag
    volatile BOOL bLockFlag;
    
    // dummy data for align 8 byte
    BYTE vbPadding[ 3 ];
} MUTEX;

typedef struct kSpinLockStruct
{
    volatile DWORD dwLockCount;
    volatile BYTE bAPICID;

    volatile BOOL bLockFlag;
    
    volatile BOOL bInterruptFlag;
    
    BYTE vbPadding[ 1 ];
} SPINLOCK;

#pragma pack( pop )

#if 0
BOOL kLockForSystemData( void );
void kUnlockForSystemData( BOOL bInterruptFlag );
#endif

void kInitializeSpinLock( SPINLOCK* pstSpinLock );
void kLockForSpinLock( SPINLOCK* pstSpinLock );
void kUnlockForSpinLock( SPINLOCK* pstSpinLock );

void kInitializeMutex( MUTEX* pstMutex );
void kLock( MUTEX* pstMutex );
void kUnlock( MUTEX* pstMutex );

#endif /*__SYNCHRONIZATION_H__*/