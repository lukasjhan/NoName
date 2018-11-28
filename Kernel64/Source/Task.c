/* filename          /Kernel64/Source/Task.c
 * date              2018.11.27
 * last edit date    2018.11.27
 * author            NO.00[UNKNOWN]
 * brief             source code for defining task
*/

#include "Task.h"
#include "Descriptor.h"

static SCHEDULER gs_stScheduler;
static TCBPOOLMANAGER gs_stTCBPoolManager;

//  TASK

/**
 *  function name : kInitializeTCBPool
 *  parameters    : void
 *  return value  : void
 *  brief         : init taskpool
 */
static void kInitializeTCBPool( void )
{
    int i;
    
    kMemSet( &( gs_stTCBPoolManager ), 0, sizeof( gs_stTCBPoolManager ) );
    
    gs_stTCBPoolManager.pstStartAddress = ( TCB* ) TASK_TCBPOOLADDRESS;
    kMemSet( TASK_TCBPOOLADDRESS, 0, sizeof( TCB ) * TASK_MAXCOUNT );

    for ( i = 0 ; i < TASK_MAXCOUNT ; i++ )
        gs_stTCBPoolManager.pstStartAddress[ i ].stLink.qwID = i;
    
    gs_stTCBPoolManager.iMaxCount = TASK_MAXCOUNT;
    gs_stTCBPoolManager.iAllocatedCount = 1;
}

/**
 *  function name : kAllocateTCB
 *  parameters    : void
 *  return value  : TCB*
 *  brief         : allocate TCB
 */
static TCB* kAllocateTCB( void )
{
    TCB*    pstEmptyTCB;
    int     i;
    
    if ( gs_stTCBPoolManager.iUseCount == gs_stTCBPoolManager.iMaxCount )
        return NULL;
    
    for ( i = 0 ; i < gs_stTCBPoolManager.iMaxCount ; i++ )
    {
        if ( ( gs_stTCBPoolManager.pstStartAddress[ i ].stLink.qwID >> 32 ) == 0 )
        {
            pstEmptyTCB = &( gs_stTCBPoolManager.pstStartAddress[ i ] );
            break;
        }
    }

    pstEmptyTCB->stLink.qwID = ( ( QWORD ) gs_stTCBPoolManager.iAllocatedCount << 32 ) | i;
    gs_stTCBPoolManager.iUseCount++;
    gs_stTCBPoolManager.iAllocatedCount++;

    if ( gs_stTCBPoolManager.iAllocatedCount == 0 )
        gs_stTCBPoolManager.iAllocatedCount = 1;
    
    return pstEmptyTCB;
}

/**
 *  function name : kFreeTCB
 *  parameters    : qwID(QWORD)
 *  return value  : void
 *  brief         : deallocate TCB
 */
static void kFreeTCB( QWORD qwID )
{
    int i;
    // set index
    i = GETTCBOFFSET( qwID );
    
    kMemSet( &( gs_stTCBPoolManager.pstStartAddress[ i ].stContext ), 0, sizeof( CONTEXT ) );
    gs_stTCBPoolManager.pstStartAddress[ i ].stLink.qwID = i;
    
    gs_stTCBPoolManager.iUseCount--;
}

/**
 *  function name : kCreateTask
 *  parameters    : qwFlags(QWORD)
 *                  qwEntryPointAddress(QWORD)
 *  return value  : TCB*
 *  brief         : create task
 */
TCB* kCreateTask( QWORD qwFlags, QWORD qwEntryPointAddress )
{
    TCB*    pstTask;
    void*   pvStackAddress;
    BOOL    bPreviousFlag;
    
    bPreviousFlag = kLockForSystemData();
    pstTask = kAllocateTCB();
    if ( pstTask == NULL )
    {
        kUnlockForSystemData( bPreviousFlag );
        return NULL;
    }
    kUnlockForSystemData( bPreviousFlag );
    
    pvStackAddress = ( void* ) ( TASK_STACKPOOLADDRESS + ( TASK_STACKSIZE * GETTCBOFFSET( pstTask->stLink.qwID ) ) );
    kSetUpTask( pstTask, qwFlags, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE );

    bPreviousFlag = kLockForSystemData();
    kAddTaskToReadyList( pstTask );
    kUnlockForSystemData( bPreviousFlag );
    
    return pstTask;
}

/**
 *  function name : kSetUpTask
 *  parameters    : pstTCB(TCB*)
 *                  qwFlags(QWORD)
 *                  qwEntryPointAddress(QWORD)
 *                  pvStackAddress(void*)
 *                  qwStackSize(QWORD)
 *  return value  : void
 *  brief         : set up TCB
 */
static void kSetUpTask( TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize )
{
    kMemSet( pstTCB->stContext.vqRegister, 0, sizeof( pstTCB->stContext.vqRegister ) );
    
    pstTCB->stContext.vqRegister[ TASK_RSPOFFSET ] = ( QWORD ) pvStackAddress + qwStackSize;
    pstTCB->stContext.vqRegister[ TASK_RBPOFFSET ] = ( QWORD ) pvStackAddress + qwStackSize;

    pstTCB->stContext.vqRegister[ TASK_CSOFFSET ] = GDT_KERNELCODESEGMENT;
    pstTCB->stContext.vqRegister[ TASK_DSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_ESOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_FSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_GSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_SSOFFSET ] = GDT_KERNELDATASEGMENT;
    
    pstTCB->stContext.vqRegister[ TASK_RIPOFFSET ] = qwEntryPointAddress;

    pstTCB->stContext.vqRegister[ TASK_RFLAGSOFFSET ] |= 0x0200;
    
    pstTCB->pvStackAddress  = pvStackAddress;
    pstTCB->qwStackSize     = qwStackSize;
    pstTCB->qwFlags         = qwFlags;
}

//  Scheduler

/**
 *  function name : kInitializeScheduler
 *  parameters    : void
 *  return value  : void
 *  brief         : init scheduler
 */
void kInitializeScheduler( void )
{
    int i;

    kInitializeTCBPool();

    for ( i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ )
    {
        kInitializeList( &( gs_stScheduler.vstReadyList[ i ] ) );
        gs_stScheduler.viExecuteCount[ i ] = 0;
    }    
    kInitializeList( &( gs_stScheduler.stWaitList ) );
    
    gs_stScheduler.pstRunningTask                   = kAllocateTCB();
    gs_stScheduler.pstRunningTask->qwFlags          = TASK_FLAGS_HIGHEST;
    gs_stScheduler.qwSpendProcessorTimeInIdleTask   = 0;
    gs_stScheduler.qwProcessorLoad                  = 0;
}

/**
 *  function name : kSetRunningTask
 *  parameters    : pstTask(TCB*)
 *  return value  : void
 *  brief         : set running task
 */
void kSetRunningTask( TCB* pstTask )
{
    BOOL bPreviousFlag;
    
    bPreviousFlag = kLockForSystemData();

    gs_stScheduler.pstRunningTask = pstTask;

    kUnlockForSystemData( bPreviousFlag );

/**
 *  function name : kGetRunningTask
 *  parameters    : void
 *  return value  : TCB*
 *  brief         : return running task
 */
TCB* kGetRunningTask( void )
{
    BOOL bPreviousFlag;
    TCB* pstRunningTask;
    
    bPreviousFlag = kLockForSystemData();
    
    pstRunningTask = gs_stScheduler.pstRunningTask;
    
    kUnlockForSystemData( bPreviousFlag );

    return pstRunningTask;
}

/**
 *  function name : kGetNextTaskToRun
 *  parameters    : void
 *  return value  : TCB*
 *  brief         : return next running task
 */
static TCB* kGetNextTaskToRun( void )
{
    TCB* pstTarget = NULL;
    int iTaskCount, i, j;
    
    for ( j = 0 ; j < 2 ; j++ )
    {
        for ( i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ )
        {
            iTaskCount = kGetListCount( &( gs_stScheduler.vstReadyList[ i ] ) );
            
            if ( gs_stScheduler.viExecuteCount[ i ] < iTaskCount )
            {
                pstTarget = ( TCB* ) kRemoveListFromHeader( &( gs_stScheduler.vstReadyList[ i ] ) );
                gs_stScheduler.viExecuteCount[ i ]++;
                break;            
            }
            else
                gs_stScheduler.viExecuteCount[ i ] = 0;
        }
        // find next task
        if( pstTarget != NULL )
            break; 
    }    
    return pstTarget;
}

/**
 *  function name : kAddTaskToReadyList
 *  parameters    : pstTask(TCB*)
 *  return value  : BOOL
 *  brief         : add task into list
 */
static BOOL kAddTaskToReadyList( TCB* pstTask )
{
    BYTE bPriority;
    
    bPriority = GETPRIORITY( pstTask->qwFlags );
    if ( bPriority >= TASK_MAXREADYLISTCOUNT )
        return FALSE;
    
    kAddListToTail( &( gs_stScheduler.vstReadyList[ bPriority ] ), pstTask );
    return TRUE;
}

/**
 *  function name : kRemoveTaskFromReadyList
 *  parameters    : qwTaskID(QWORD)
 *  return value  : TCB*
 *  brief         : remove task in ready list
 */
static TCB* kRemoveTaskFromReadyList( QWORD qwTaskID )
{
    TCB* pstTarget;
    BYTE bPriority;
    
    // invaild value
    if ( GETTCBOFFSET( qwTaskID ) >= TASK_MAXCOUNT )
        return NULL;
    
    pstTarget = &( gs_stTCBPoolManager.pstStartAddress[ GETTCBOFFSET( qwTaskID ) ] );
    if ( pstTarget->stLink.qwID != qwTaskID )
        return NULL;
    
    bPriority = GETPRIORITY( pstTarget->qwFlags );
    pstTarget = kRemoveList( &( gs_stScheduler.vstReadyList[ bPriority ]), qwTaskID );
    return pstTarget;
}

/**
 *  function name : kChangePriority
 *  parameters    : qwTaskID(QWORD)
 *  return value  : TCB*
 *  brief         : change priority 
 */
BOOL kChangePriority( QWORD qwTaskID, BYTE bPriority )
{
    TCB* pstTarget;
    BOOL bPreviousFlag;
    
    if ( bPriority > TASK_MAXREADYLISTCOUNT )
        return FALSE;

    bPreviousFlag = kLockForSystemData();

    // currently running task
    pstTarget = gs_stScheduler.pstRunningTask;
    if ( pstTarget->stLink.qwID == qwTaskID )
        SETPRIORITY( pstTarget->qwFlags, bPriority );
    
    else
    {
        pstTarget = kRemoveTaskFromReadyList( qwTaskID );
        if ( pstTarget == NULL )
        {
            pstTarget = kGetTCBInTCBPool( GETTCBOFFSET( qwTaskID ) );
            if ( pstTarget != NULL )
                SETPRIORITY( pstTarget->qwFlags, bPriority ); 
        }
        else
        {
            SETPRIORITY( pstTarget->qwFlags, bPriority );
            kAddTaskToReadyList( pstTarget );
        }
    }
    kUnlockForSystemData( bPreviousFlag );
    return TRUE;    
}

/**
 *  function name : kSchedule
 *  parameters    : void
 *  return value  : void
 *  brief         : schedule function
 *                  DO NOT CALL IT DURING INTERRUPT OR EXCEPTION!!!
 */
void kSchedule( void )
{
    TCB* pstRunningTask, * pstNextTask;
    BOOL bPreviousFlag;
    
    if ( kGetReadyTaskCount() < 1 )
        return ;

    bPreviousFlag = kLockForSystemData();
    
    pstNextTask = kGetNextTaskToRun();

    if ( pstNextTask == NULL )
    {
        kUnlockForSystemData( bPreviousFlag );
        return ;
    }
    
    pstRunningTask = gs_stScheduler.pstRunningTask; 
    gs_stScheduler.pstRunningTask = pstNextTask;
    
    if ( ( pstRunningTask->qwFlags & TASK_FLAGS_IDLE ) == TASK_FLAGS_IDLE )
        gs_stScheduler.qwSpendProcessorTimeInIdleTask += TASK_PROCESSORTIME - gs_stScheduler.iProcessorTime;
    
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;
    
    if ( pstRunningTask->qwFlags & TASK_FLAGS_ENDTASK )
    {
        kAddListToTail( &( gs_stScheduler.stWaitList ), pstRunningTask );
        kSwitchContext( NULL, &( pstNextTask->stContext ) );
    }
    else
    {
        kAddTaskToReadyList( pstRunningTask );
        kSwitchContext( &( pstRunningTask->stContext ), &( pstNextTask->stContext ) );
    }

    kUnlockForSystemData( bPreviousFlag );
}

/**
 *  function name : kScheduleInInterrupt
 *  parameters    : void
 *  return value  : BOOL
 *  brief         : when interrupt occur, switching task
 *                  CALL IT DURING INTERRUPT OR EXCEPTION!!!
 */
BOOL kScheduleInInterrupt( void )
{
    TCB* pstRunningTask, * pstNextTask;
    char* pcContextAddress;
    BOOL bPreviousFlag;

    bPreviousFlag = kLockForSystemData();
    
    pstNextTask = kGetNextTaskToRun();

    if ( pstNextTask == NULL )
    {
        kUnlockForSystemData( bPreviousFlag );
        return FALSE;
    }
    
    pcContextAddress = ( char* ) IST_STARTADDRESS + IST_SIZE - sizeof( CONTEXT );

    // context change
    pstRunningTask = gs_stScheduler.pstRunningTask;
    gs_stScheduler.pstRunningTask = pstNextTask;

    // idle task
    if ( ( pstRunningTask->qwFlags & TASK_FLAGS_IDLE ) == TASK_FLAGS_IDLE )
        gs_stScheduler.qwSpendProcessorTimeInIdleTask += TASK_PROCESSORTIME;
       
    // task end flag
    if ( pstRunningTask->qwFlags & TASK_FLAGS_ENDTASK )
       kAddListToTail( &( gs_stScheduler.stWaitList ), pstRunningTask );
    else
    {
        kMemCpy( &( pstRunningTask->stContext ), pcContextAddress, sizeof( CONTEXT ) );
        kAddTaskToReadyList( pstRunningTask );
    }
    kUnlockForSystemData( bPreviousFlag );
    kMemCpy( pcContextAddress, &( pstNextTask->stContext ), sizeof( CONTEXT ) );
    
    gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;
    return TRUE;
}

/**
 *  function name : kDecreaseProcessorTime
 *  parameters    : void
 *  return value  : void
 *  brief         : decrease processor time
 */
void kDecreaseProcessorTime( void )
{
    if ( gs_stScheduler.iProcessorTime > 0 )
        gs_stScheduler.iProcessorTime--;
}

/**
 *  function name : kIsProcessorTimeExpired
 *  parameters    : void
 *  return value  : void
 *  brief         : return processor time expired
 */
BOOL kIsProcessorTimeExpired( void )
{
    if ( gs_stScheduler.iProcessorTime <= 0 )
        return TRUE;
    
    return FALSE;
}

/**
 *  function name : kEndTask
 *  parameters    : qwTaskID(QWORD)
 *  return value  : BOOL
 *  brief         : end task
 */
BOOL kEndTask( QWORD qwTaskID )
{
    TCB* pstTarget;
    BYTE bPriority;
    BOOL bPreviousFlag;

    bPreviousFlag = kLockForSystemData();
    
    pstTarget = gs_stScheduler.pstRunningTask;
    if ( pstTarget->stLink.qwID == qwTaskID )
    {
        pstTarget->qwFlags |= TASK_FLAGS_ENDTASK;
        SETPRIORITY( pstTarget->qwFlags, TASK_FLAGS_WAIT );

        kUnlockForSystemData( bPreviousFlag );
        
        kSchedule();

        // task switched.. NOT RUN
        while( 1 ) ;
    }
    else
    {
        pstTarget = kRemoveTaskFromReadyList( qwTaskID );
        if ( pstTarget == NULL )
        {
            pstTarget = kGetTCBInTCBPool( GETTCBOFFSET( qwTaskID ) );
            if ( pstTarget != NULL )
            {
                pstTarget->qwFlags |= TASK_FLAGS_ENDTASK;
                SETPRIORITY( pstTarget->qwFlags, TASK_FLAGS_WAIT );
            }
            kUnlockForSystemData( bPreviousFlag );
            return FALSE;
        }
        
        pstTarget->qwFlags |= TASK_FLAGS_ENDTASK;
        SETPRIORITY( pstTarget->qwFlags, TASK_FLAGS_WAIT );
        kAddListToTail( &( gs_stScheduler.stWaitList ), pstTarget );
    }
    kUnlockForSystemData( bPreviousFlag );
    return TRUE;
}

/**
 *  function name : kExitTask
 *  parameters    : void
 *  return value  : void
 *  brief         : exit task
 */
void kExitTask( void )
{
    kEndTask( gs_stScheduler.pstRunningTask->stLink.qwID );
}

/**
 *  function name : kGetReadyTaskCount
 *  parameters    : void
 *  return value  : int
 *  brief         : return task number of readyQueue
 */
int kGetReadyTaskCount( void )
{
    int iTotalCount = 0;
    int i;
    BOOL bPreviousFlag;

    bPreviousFlag = kLockForSystemData();
    
    for ( i = 0 ; i < TASK_MAXREADYLISTCOUNT ; i++ )
        iTotalCount += kGetListCount( &( gs_stScheduler.vstReadyList[ i ] ) );
    
    kUnlockForSystemData( bPreviousFlag );
    return iTotalCount ;
}

/**
 *  function name : kGetTaskCount
 *  parameters    : void
 *  return value  : int
 *  brief         : return number of tasks
 */
int kGetTaskCount( void )
{
    int iTotalCount;
    BOOL bPreviousFlag;
    
    iTotalCount = kGetReadyTaskCount();

    bPreviousFlag = kLockForSystemData();
    iTotalCount += kGetListCount( &( gs_stScheduler.stWaitList ) ) + 1;

    kUnlockForSystemData( bPreviousFlag );
    return iTotalCount;
}

/**
 *  function name : kGetTCBInTCBPool
 *  parameters    : iOffset(int)
 *  return value  : TCB*
 *  brief         : return offset's TCB
 */
TCB* kGetTCBInTCBPool( int iOffset )
{
    if ( ( iOffset < -1 ) || ( iOffset > TASK_MAXCOUNT ) )
        return NULL;
    
    return &( gs_stTCBPoolManager.pstStartAddress[ iOffset ] );
}

/**
 *  function name : kIsTaskExist
 *  parameters    : qwID(QWORD)
 *  return value  : BOOL
 *  brief         : check task
 */
BOOL kIsTaskExist( QWORD qwID )
{
    TCB* pstTCB;
    
    pstTCB = kGetTCBInTCBPool( GETTCBOFFSET( qwID ) );

    if ( ( pstTCB == NULL ) || ( pstTCB->stLink.qwID != qwID ) )
        return FALSE;

    return TRUE;
}

/**
 *  function name : kGetProcessorLoad
 *  parameters    : void
 *  return value  : QWORD
 *  brief         : return load rate of cpu
 */
QWORD kGetProcessorLoad( void )
{
    return gs_stScheduler.qwProcessorLoad;
}

//  IDLE TASK

/**
 *  function name : kIdleTask
 *  parameters    : void
 *  return value  : void
 *  brief         : manage removing task
 */
void kIdleTask( void )
{
    TCB* pstTask;
    QWORD qwLastMeasureTickCount, qwLastSpendTickInIdleTask;
    QWORD qwCurrentMeasureTickCount, qwCurrentSpendTickInIdleTask;
    BOOL bPreviousFlag;
    QWORD qwTaskID;
    
    qwLastSpendTickInIdleTask = gs_stScheduler.qwSpendProcessorTimeInIdleTask;
    qwLastMeasureTickCount = kGetTickCount();
    
    while ( 1 )
    {
        qwCurrentMeasureTickCount = kGetTickCount();
        qwCurrentSpendTickInIdleTask = gs_stScheduler.qwSpendProcessorTimeInIdleTask;
        
        // cal cpu rate
        if ( qwCurrentMeasureTickCount - qwLastMeasureTickCount == 0 )
            gs_stScheduler.qwProcessorLoad = 0;
        else
            gs_stScheduler.qwProcessorLoad = 100 - ( qwCurrentSpendTickInIdleTask - qwLastSpendTickInIdleTask ) * 100 /( qwCurrentMeasureTickCount - qwLastMeasureTickCount );
        
        qwLastMeasureTickCount = qwCurrentMeasureTickCount;
        qwLastSpendTickInIdleTask = qwCurrentSpendTickInIdleTask;

        kHaltProcessorByLoad();
        
        if ( kGetListCount( &( gs_stScheduler.stWaitList ) ) >= 0 )
        {
            while ( 1 )
            {
                bPreviousFlag = kLockForSystemData();
                pstTask = kRemoveListFromHeader( &( gs_stScheduler.stWaitList ) );
                if ( pstTask == NULL )
                {   
                    kUnlockForSystemData( bPreviousFlag );
                    break;
                }
                qwTaskID = pstTask->stLink.qwID;
                kFreeTCB( qwTaskID );
                
                kUnlockForSystemData( bPreviousFlag );
                
                kPrintf( "IDLE: Task ID[0x%q] is completely ended.\n", qwTaskID );
            }
        }
        
        kSchedule();
    }
}

/**
 *  function name : kHaltProcessorByLoad
 *  parameters    : void
 *  return value  : void
 *  brief         : halt cpu
 */
void kHaltProcessorByLoad( void )
{
    if ( gs_stScheduler.qwProcessorLoad < 40 )
    {
        kHlt();
        kHlt();
        kHlt();
    }
    else if ( gs_stScheduler.qwProcessorLoad < 80 )
    {
        kHlt();
        kHlt();
    }
    else if ( gs_stScheduler.qwProcessorLoad < 95 )
    {
        kHlt();
    }
}