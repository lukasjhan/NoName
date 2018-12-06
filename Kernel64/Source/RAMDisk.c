/* filename          /Kernel64/Source/RAMDisk.c
 * date              2018.12.06
 * last edit date    2018.12.06
 * author            NO.00[UNKNOWN]
 * brief             source code for ram disk
*/

#include "RAMDisk.h"
#include "Utility.h"
#include "DynamicMemory.h"

static RDDMANAGER gs_stRDDManager;

/**
 *  function name : kInitializeRDD
 *  parameters    : dwTotalSectorCount(DWORD)
 *  return value  : BOOL
 *  brief         : init RAM disk manager
 */
BOOL kInitializeRDD( DWORD dwTotalSectorCount )
{
    kMemSet( &gs_stRDDManager, 0, sizeof( gs_stRDDManager ) );
    
    gs_stRDDManager.pbBuffer = ( BYTE* ) kAllocateMemory( dwTotalSectorCount * 512 );

    if ( gs_stRDDManager.pbBuffer == NULL )
        return FALSE;
    
    gs_stRDDManager.dwTotalSectorCount = dwTotalSectorCount;
    kInitializeMutex( &( gs_stRDDManager.stMutex ) );

    return TRUE;
}

/**
 *  function name : kInitializeRDD
 *  parameters    : bPrimary(BOOL)
 *                  bMaster(BOOL)
 *                  pstHDDInformation(HDDINFORMATION*)
 *  return value  : BOOL
 *  brief         : read RAM disk info
 */
BOOL kReadRDDInformation( BOOL bPrimary, BOOL bMaster, HDDINFORMATION* pstHDDInformation )
{
    kMemSet( pstHDDInformation, 0, sizeof( HDDINFORMATION ) );
    
    pstHDDInformation->dwTotalSectors = gs_stRDDManager.dwTotalSectorCount;
    kMemCpy( pstHDDInformation->vwSerialNumber, "0000-0000", 9 );
    kMemCpy( pstHDDInformation->vwModelNumber, "MINT RAM Disk v1.0", 18 ); 

    return TRUE;
}

/**
 *  function name : kReadRDDSector
 *  parameters    : bPrimary(BOOL)
 *                  bMaster(BOOL)
 *                  dwLBA(DWORD)
 *                  iSectorCount(int)
 *                  pcBuffer(char*)
 *  return value  : int
 *  brief         : read RAM disk sector
 */
int kReadRDDSector( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount, char* pcBuffer )
{
    int iRealReadCount;
    
    iRealReadCount = MIN( gs_stRDDManager.dwTotalSectorCount - dwLBA, iSectorCount );

    kMemCpy( pcBuffer, gs_stRDDManager.pbBuffer + ( dwLBA * 512 ), iRealReadCount * 512 );
    
    return iRealReadCount;
}

/**
 *  function name : kWriteRDDSector
 *  parameters    : bPrimary(BOOL)
 *                  bMaster(BOOL)
 *                  dwLBA(DWORD)
 *                  iSectorCount(int)
 *                  pcBuffer(char*)
 *  return value  : int
 *  brief         : write RAM disk sector
 */
int kWriteRDDSector( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount, char* pcBuffer )
{
    int iRealWriteCount;

    iRealWriteCount = MIN( gs_stRDDManager.dwTotalSectorCount - dwLBA, iSectorCount );

    kMemCpy( gs_stRDDManager.pbBuffer + ( dwLBA * 512 ), pcBuffer, iRealWriteCount * 512 );
    
    return iRealWriteCount;    
}