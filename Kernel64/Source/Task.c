/* filename          /Kernel64/Source/Task.c
 * date              2018.11.27
 * last edit date    2018.11.27
 * author            NO.00[UNKNOWN]
 * brief             source code for defining task
*/

#include "Task.h"
#include "Descriptor.h"

/**
 *  function name : kSetUpTask
 *  parameters    : pstTCB(TCB*)
 *                  qwID(QWORD)
 *                  qwFlags(QWORD)
 *                  qwEntryPointAddress(QWORD)
 *                  pvStackAddress(void*)
 *                  qwStackSize(QWORD)
 *  return value  : void
 *  brief         : set up TCB
 */
void kSetUpTask( TCB* pstTCB, QWORD qwID, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize )
{
    // init context
    kMemSet( pstTCB->stContext.vqRegister, 0, sizeof( pstTCB->stContext.vqRegister ) );
    
    // set up stack RSP, RBP register
    pstTCB->stContext.vqRegister[ TASK_RSPOFFSET ] = ( QWORD ) pvStackAddress + qwStackSize;
    pstTCB->stContext.vqRegister[ TASK_RBPOFFSET ] = ( QWORD ) pvStackAddress + qwStackSize;

    // set up segment selector
    pstTCB->stContext.vqRegister[ TASK_CSOFFSET ] = GDT_KERNELCODESEGMENT;
    pstTCB->stContext.vqRegister[ TASK_DSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_ESOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_FSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_GSOFFSET ] = GDT_KERNELDATASEGMENT;
    pstTCB->stContext.vqRegister[ TASK_SSOFFSET ] = GDT_KERNELDATASEGMENT;

    // set up RIP register, interrupt flag 
    pstTCB->stContext.vqRegister[ TASK_RIPOFFSET ] = qwEntryPointAddress;

    // RFLAGS register IF bit(bit 9) set 1 to enable interrupt
    pstTCB->stContext.vqRegister[ TASK_RFLAGSOFFSET ] |= 0x0200;
    
    pstTCB->qwID            = qwID;
    pstTCB->pvStackAddress  = pvStackAddress;
    pstTCB->qwStackSize     = qwStackSize;
    pstTCB->qwFlags         = qwFlags;
}