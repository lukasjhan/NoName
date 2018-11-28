/* filename          /Kernel64/Source/Synchronization.c
 * date              2018.11.28
 * last edit date    2018.11.28
 * author            NO.00[UNKNOWN]
 * brief             source code for mutex
*/

#include "Synchronization.h"
#include "Utility.h"
#include "Task.h"

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