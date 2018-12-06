/* filename          /Kernel64/Source/RAMDisk.h
 * date              2018.11.23
 * last edit date    2018.11.28
 * author            NO.00[UNKNOWN]
 * brief             header file for RAMDisk.c
*/

#ifndef __RAMDISK_H__
#define __RAMDISK_H__

#include "Types.h"
#include "Synchronization.h"
#include "HardDisk.h"

#define RDD_TOTALSECTORCOUNT        ( 8 * 1024 * 1024 / 512)

#pragma pack( push, 1 )

typedef struct kRDDManagerStruct
{
    BYTE* pbBuffer;
    DWORD dwTotalSectorCount;
    MUTEX stMutex;
} RDDMANAGER;

#pragma pack( pop)

BOOL kInitializeRDD( DWORD dwTotalSectorCount );
BOOL kReadRDDInformation( BOOL bPrimary, BOOL bMaster, HDDINFORMATION* pstHDDInformation );
int kReadRDDSector( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount, char* pcBuffer );
int kWriteRDDSector( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount, char* pcBuffer );

#endif /*__RAMDISK_H__*/