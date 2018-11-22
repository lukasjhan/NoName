/* filename          /Kernel64/Source/Keyboard.c
 * date              2018.11.22
 * last edit date    2018.11.22
 * author            NO.00[UNKNOWN]
 * brief             source code for Queue
*/

#include "Queue.h"
#include "Utility.h"

/**
 *  function name : kInitializeQueue
 *  parameter     : pstQueue(QUEUE*)
 *                  pvQueueBuffer(void*)
 *                  iMaxDataCount(int)
 *                  iDataSize(int)
 *  return value  : void
 *  brief         : init Queue
 */
void kInitializeQueue( QUEUE* pstQueue, void* pvQueueBuffer, int iMaxDataCount, int iDataSize )
{
    // init Queue
    pstQueue->iMaxDataCount     = iMaxDataCount;
    pstQueue->iDataSize         = iDataSize;
    pstQueue->pvQueueArray      = pvQueueBuffer;

    // empty Queue
    pstQueue->iPutIndex         = 0;
    pstQueue->iGetIndex         = 0;
    pstQueue->bLastOperationPut = FALSE;
}

/**
 *  function name : kIsQueueFull
 *  parameter     : pstQueue(const QUEUE*)
 *  return value  : BOOL
 *  brief         : check Queue is full
 */
BOOL kIsQueueFull( const QUEUE* pstQueue )
{
    if ( ( pstQueue->iGetIndex == pstQueue->iPutIndex ) && ( pstQueue->bLastOperationPut == TRUE ) )
        return TRUE;
    
    return FALSE;
}

/**
 *  function name : kIsQueueEmpty
 *  parameter     : pstQueue(const QUEUE*)
 *  return value  : BOOL
 *  brief         : check Queue is empty
 */
BOOL kIsQueueEmpty( const QUEUE* pstQueue )
{
    if ( ( pstQueue->iGetIndex == pstQueue->iPutIndex ) && ( pstQueue->bLastOperationPut == FALSE ) )
        return TRUE;
    
    return FALSE;
}

/**
 *  function name : kPutQueue
 *  parameter     : pstQueue(const QUEUE*)
 *                  pvData(const void*)
 *  return value  : BOOL
 *  brief         : put data on Queue
 */
BOOL kPutQueue( QUEUE* pstQueue, const void* pvData )
{
    if ( kIsQueueFull( pstQueue ) == TRUE )
        return FALSE;
    
    kMemCpy( ( char* ) pstQueue->pvQueueArray + ( pstQueue->iDataSize * pstQueue->iPutIndex ), pvData, pstQueue->iDataSize );

    pstQueue->iPutIndex         = ( pstQueue->iPutIndex + 1 ) % pstQueue->iMaxDataCount;
    pstQueue->bLastOperationPut = TRUE;
    return TRUE;
}

/**
 *  function name : kGetQueue
 *  parameter     : pstQueue(const QUEUE*)
 *                  pvData(const void*)
 *  return value  : BOOL
 *  brief         : remove data on Queue
 */
BOOL kGetQueue( QUEUE* pstQueue, void* pvData )
{
    if ( kIsQueueEmpty( pstQueue ) == TRUE )
        return FALSE;
    
    kMemCpy( pvData, ( char* ) pstQueue->pvQueueArray + ( pstQueue->iDataSize * pstQueue->iGetIndex ), pstQueue->iDataSize );

    pstQueue->iGetIndex         = ( pstQueue->iGetIndex + 1 ) % pstQueue->iMaxDataCount;
    pstQueue->bLastOperationPut = FALSE;
    return TRUE;
}