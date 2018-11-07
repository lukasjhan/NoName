/* filename          /Kernel32/Source/ModeSwitch.h
 * date              2018.11.07
 * last edit date    2018.11.07
 * author            NO.00[UNKNOWN]
 * brief             header file for ModeSwitch.asm
*/

#ifndef __MODESWITCH_H__
#define __MODESWITCH_H__

#include "Types.h"

void kReadCPUID( DWORD dwEAX, DWORD* pdwEAX, DWORD* pdwEBX, DWORD* pdwECX, DWORD* pdwEDX );
void kSwitchAndExecute64bitKernel( void );

#endif /*__MODESWITCH_H__*/