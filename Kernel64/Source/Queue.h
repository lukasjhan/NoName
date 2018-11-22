/* filename          /Kernel64/Source/Keyboard.h
 * date              2018.11.22
 * last edit date    2018.11.22
 * author            NO.00[UNKNOWN]
 * brief             header file for Queue.c
*/

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "Types.h"

#pragma pack( push, 1 )

// Struct for Queue
typedef struct kQueueManagerStruct
{
    // data size and number
    int iDataSize;
    int iMaxDataCount;

    // Queue buffer insert/remove index
    void* pvQueueArray;
    int iPutIndex;
    int iGetIndex;

    // check last action is insert 
    BOOL bLastOperationPut;
} QUEUE;

#pragma pack( pop )

void kInitializeQueue( QUEUE* pstQueue, void* pvQueueBuffer, int iMaxDataCount, int iDataSize );
BOOL kIsQueueFull( const QUEUE* pstQueue );
BOOL kIsQueueEmpty( const QUEUE* pstQueue );
BOOL kPutQueue( QUEUE* pstQueue, const void* pvData );
BOOL kGetQueue( QUEUE* pstQueue, void* pvData );

#endif /*__QUEUE_H__*/