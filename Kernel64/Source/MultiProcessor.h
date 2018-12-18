/* filename          /Kernel64/Source/MultiProcessor.h
 * date              2018.12.18
 * last edit date    2018.12.18
 * author            NO.00[UNKNOWN]
 * brief             header file for MultiProcessor.c
*/

#ifndef __MULTIPROCESSOR_H__
#define __MULTIPROCESSOR_H__

#include "Types.h"


#define BOOTSTRAPPROCESSOR_FLAGADDRESS      0x7C09
#define MAXPROCESSORCOUNT                   16


BOOL kStartUpApplicationProcessor( void );
BYTE kGetAPICID( void );
static BOOL kWakeUpApplicationProcessor( void );

#endif /*__MULTIPROCESSOR_H__*/