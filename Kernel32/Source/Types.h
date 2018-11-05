/* filename          /Kernel32/Source/Types.h
 * date              2018.11.05
 * last edit date    2018.11.05
 * author            NO.00[UNKNOWN]
 * brief             define entry point for kernel32 c function
*/

#ifndef __TYPES_H__
#define __TYPES_H__

#define BYTE    unsigned char
#define WORD    unsigned short
#define DWORD   unsigned int
#define QWORD   unsigned long
#define BOOL    unsigned char

#define TRUE    1
#define FALSE   0
#define NULL    0


//      STRUCT      //
#pragma pack( push, 1 )

// data for text video screen
typedef struct kCharactorStruct
{
    BYTE bCharactor;
    BYTE bAttribute;
} CHARACTER;

#pragma pack( pop )

#endif /*__TYPES_H__*/