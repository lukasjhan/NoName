/* filename          /Kernel64/Source/List.h
 * date              2018.11.27
 * last edit date    2018.11.27
 * author            NO.00[UNKNOWN]
 * brief             header file for List.c
*/

#ifndef __LIST_H__
#define __LIST_H__

#include "Types.h"

#pragma pack( push, 1 )

// MUST BE FIRST on data list
typedef struct kListLinkStruct
{
    void* pvNext;
    QWORD qwID;
} LISTLINK;

/*
// data struct
struct kListItemExampleStruct
{
    LISTLINK stLink;
   
    int iData1;
    char cData2;
};
*/

typedef struct kListManagerStruct
{
    int iItemCount;

    void* pvHeader;
    void* pvTail;
} LIST;

#pragma pack( pop )

void kInitializeList( LIST* pstList );
int kGetListCount( const LIST* pstList );
void kAddListToTail( LIST* pstList, void* pvItem );
void kAddListToHeader( LIST* pstList, void* pvItem );
void* kRemoveList( LIST* pstList, QWORD qwID );
void* kRemoveListFromHeader( LIST* pstList );
void* kRemoveListFromTail( LIST* pstList );
void* kFindList( const LIST* pstList, QWORD qwID );
void* kGetHeaderFromList( const LIST* pstList );
void* kGetTailFromList( const LIST* pstList );
void* kGetNextFromList( const LIST* pstList, void* pstCurrent );

#endif /*__LIST_H__*/