/* filename          /Kernel64/Source/InterruptHandler.h
 * date              2018.11.22
 * last edit date    2018.11.22
 * author            NO.00[UNKNOWN]
 * brief             header file for InterruptHandler.c
*/

#ifndef __INTERRUPTHANDLER_H__
#define __INTERRUPTHANDLER_H__

#include "Types.h"

void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode );
void kCommonInterruptHandler( int iVectorNumber );
void kKeyboardHandler( int iVectorNumber );

#endif /*__INTERRUPTHANDLER_H__*/