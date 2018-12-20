/* filename          /Kernel64/Source/AssemblyUtility.h
 * date              2018.11.20
 * last edit date    2018.11.28
 * author            NO.00[UNKNOWN]
 * brief             header file for AssemblyUtility.asm
*/

#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

#include "Types.h"
#include "Task.h"

BYTE kInPortByte( WORD wPort );
void kOutPortByte( WORD wPort, BYTE bData );
WORD kInPortWord( WORD wPort );
void kOutPortWord( WORD wPort, WORD wData );
void kLoadGDTR( QWORD gwGDTAddress );
void kLoadTR( WORD wTSSSegmentOffset );
void kLoadIDTR( QWORD qwIDTAddress );
void kEnableInterrupt( void );
void kDisableInterrupt( void );
QWORD kReadRFLAGS( void );
QWORD kReadTSC( void );
void kSwitchContext( CONTEXT* pstCurrentContext, CONTEXT* pstNextContext );
void kHlt( void );
BOOL kTestAndSet( volatile BYTE* pbDestination, BYTE bCompare, BYTE bSource );
void kInitializeFPU( void );
void kSaveFPUContext( void* pvFPUContext );
void kLoadFPUContext( void* pvFPUContext );
void kSetTS( void );
void kClearTS( void );
void kEnableGlobalLocalAPIC( void );
void kPause( void );

#endif /*__ASSEMBLYUTILITY_H__*/