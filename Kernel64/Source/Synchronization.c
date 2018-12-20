/* filename          /Kernel64/Source/Synchronization.c
 * date              2018.11.28
 * last edit date    2018.11.28
 * author            NO.00[UNKNOWN]
 * brief             source code for mutex
*/

#include "Synchronization.h"
#include "Utility.h"
#include "Task.h"

#if 0
/**
 *  function name : kLockForSystemData
 *  parameters    : void
 *  return value  : BOOL
 *  brief         : global data lock function
 */
BOOL kLockForSystemData( void )
{
    return kSetInterruptFlag( FALSE );
}

/**
 *  function name : kUnlockForSystemData
 *  parameters    : void
 *  return value  : void
 *  brief         : global data unlock function
 */
void kUnlockForSystemData( BOOL bInterruptFlag )
{
    kSetInterruptFlag( bInterruptFlag );
}
#endif

/**
 *  function name : kInitializeMutex
 *  parameters    : pstMutex(MUTEX*)
 *  return value  : void
 *  brief         : init mutex
 */
void kInitializeMutex( MUTEX* pstMutex )
{
    pstMutex->bLockFlag = FALSE;
    pstMutex->dwLockCount = 0;
    pstMutex->qwTaskID = TASK_INVALIDID;
}

/**
 *  function name : kLock
 *  parameters    : pstMutex(MUTEX*)
 *  return value  : void
 *  brief         : lock data
 */
void kLock( MUTEX* pstMutex )
{
    // check already locked
    if( kTestAndSet(&( pstMutex->bLockFlag ), 0, 1 ) == FALSE )
    {
        // check lock from me
        if( pstMutex->qwTaskID == kGetRunningTask()->stLink.qwID ) 
        {
            pstMutex->dwLockCount++;
            return ;
        }
        
        while( kTestAndSet( &( pstMutex->bLockFlag ), 0, 1 ) == FALSE )
            kSchedule();
    }
       
    pstMutex->dwLockCount   = 1;
    pstMutex->qwTaskID      = kGetRunningTask()->stLink.qwID;
}

/**
 *  function name : kUnlock
 *  parameters    : pstMutex(MUTEX*)
 *  return value  : void
 *  brief         : unlock data
 */
void kUnlock( MUTEX* pstMutex )
{
    // not lock from me
    if ( ( pstMutex->bLockFlag == FALSE ) || ( pstMutex->qwTaskID != kGetRunningTask()->stLink.qwID ) )
        return ;
    
    if ( pstMutex->dwLockCount > 1 )
    {
        pstMutex->dwLockCount--;
        return ;
    }
    
    pstMutex->qwTaskID      = TASK_INVALIDID;
    pstMutex->dwLockCount   = 0;
    pstMutex->bLockFlag     = FALSE;
}

void kInitializeSpinLock( SPINLOCK* pstSpinLock )
{
    pstSpinLock->bLockFlag = FALSE;
    pstSpinLock->dwLockCount = 0;
    pstSpinLock->bAPICID = 0xFF;
    pstSpinLock->bInterruptFlag = FALSE;
}

void kLockForSpinLock( SPINLOCK* pstSpinLock )
{
    BOOL bInterruptFlag;
    
    bInterruptFlag = kSetInterruptFlag( FALSE );

    if( kTestAndSet(&( pstSpinLock->bLockFlag ), 0, 1 ) == FALSE )
    {
        if( pstSpinLock->bAPICID == kGetAPICID() )
        {
            pstSpinLock->dwLockCount++;
            return ;
        }
        
        while( kTestAndSet( &( pstSpinLock->bLockFlag ), 0, 1 ) == FALSE )
        {
            while( pstSpinLock->bLockFlag == TRUE )
            {
                kPause();
            }
        }
    }
    
    pstSpinLock->dwLockCount = 1;
    pstSpinLock->bAPICID = kGetAPICID();

    pstSpinLock->bInterruptFlag = bInterruptFlag;
}

void kUnlockForSpinLock( SPINLOCK* pstSpinLock )
{
    BOOL bInterruptFlag;
    
    bInterruptFlag = kSetInterruptFlag( FALSE );

    if( ( pstSpinLock->bLockFlag == FALSE ) ||
        ( pstSpinLock->bAPICID != kGetAPICID() ) )
    {
        kSetInterruptFlag( bInterruptFlag );
        return ;
    }
    
    if( pstSpinLock->dwLockCount > 1 )
    {
        pstSpinLock->dwLockCount--;
        return ;
    }
    
    bInterruptFlag = pstSpinLock->bInterruptFlag;    
    pstSpinLock->bAPICID = 0xFF;
    pstSpinLock->dwLockCount = 0;
    pstSpinLock->bInterruptFlag = FALSE;
    pstSpinLock->bLockFlag = FALSE;  
    
    kSetInterruptFlag( bInterruptFlag );
}