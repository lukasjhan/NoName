/* filename          /Kernel64/Source/AssemblyUtility.h
 * date              2018.11.20
 * last edit date    2018.11.20
 * author            NO.00[UNKNOWN]
 * brief             header file for AssemblyUtility.asm
*/

#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

#include "Types.h"
#include "Task.h"

BYTE kInPortByte( WORD wPort );
void kOutPortByte( WORD wPort, BYTE bData );
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

#endif /*__ASSEMBLYUTILITY_H__*/