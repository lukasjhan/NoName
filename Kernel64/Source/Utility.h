/* filename          /Kernel64/Source/Utility.h
 * date              2018.11.20
 * last edit date    2018.11.20
 * author            NO.00[UNKNOWN]
 * brief             header file for Utility.c
*/

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "Types.h"

void kMemSet( void* pvDestination, BYTE bData, int iSize );
int kMemCpy( void* pvDestination, const void* pvSource, int iSize );
int kMemCmp( const void* pvDestination, const void* pvSource, int iSize );
BOOL kSetInterruptFlag( BOOL bEnableInterrupt );

#endif /*__UTILITY_H__*/