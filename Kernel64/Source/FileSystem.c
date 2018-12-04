/* filename          /Kernel64/Source/FileSystem.c
 * date              2018.12.04
 * last edit date    2018.12.04
 * author            NO.00[UNKNOWN]
 * brief             source code for filesystem
*/

#include "FileSystem.h"
#include "HardDisk.h"
#include "DynamicMemory.h"

static FILESYSTEMMANAGER   gs_stFileSystemManager;
static BYTE gs_vbTempBuffer[ FILESYSTEM_SECTORSPERCLUSTER * 512 ];

fReadHDDInformation gs_pfReadHDDInformation = NULL;
fReadHDDSector gs_pfReadHDDSector = NULL;
fWriteHDDSector gs_pfWriteHDDSector = NULL;

/**
 *  파일 시스템을 초기화
 */
/**
 *  function name : kInitializeFileSystem
 *  parameters    : void
 *  return value  : BOOL
 *  brief         : init filesystem
 */
BOOL kInitializeFileSystem( void )
{
    kMemSet( &gs_stFileSystemManager, 0, sizeof( gs_stFileSystemManager ) );
    kInitializeMutex( &( gs_stFileSystemManager.stMutex ) );
    
    if ( kInitializeHDD() == TRUE )
    {
        gs_pfReadHDDInformation = kReadHDDInformation;
        gs_pfReadHDDSector = kReadHDDSector;
        gs_pfWriteHDDSector = kWriteHDDSector;
    }
    else
        return FALSE;
    
    if ( kMount() == FALSE )
        return FALSE;
    
    return TRUE;
}

// LOW LEVEL FUNCTION

/**
 *  function name : kMount
 *  parameters    : void
 *  return value  : BOOL
 *  brief         : read MBR on HDD, check file system is correct
 */
BOOL kMount( void )
{
    MBR* pstMBR;
    
    kLock( &( gs_stFileSystemManager.stMutex ) );

    if ( gs_pfReadHDDSector( TRUE, TRUE, 0, 1, gs_vbTempBuffer ) == FALSE )
    {
        kUnlock( &( gs_stFileSystemManager.stMutex ) );
        return FALSE;
    }
    
    pstMBR = ( MBR* ) gs_vbTempBuffer;
    if ( pstMBR->dwSignature != FILESYSTEM_SIGNATURE )
    {
        kUnlock( &( gs_stFileSystemManager.stMutex ) );
        return FALSE;
    }
    
    gs_stFileSystemManager.bMounted = TRUE;
    
    gs_stFileSystemManager.dwReservedSectorCount = pstMBR->dwReservedSectorCount;
    gs_stFileSystemManager.dwClusterLinkAreaStartAddress = pstMBR->dwReservedSectorCount + 1;
    gs_stFileSystemManager.dwClusterLinkAreaSize = pstMBR->dwClusterLinkSectorCount;
    gs_stFileSystemManager.dwDataAreaStartAddress = pstMBR->dwReservedSectorCount + pstMBR->dwClusterLinkSectorCount + 1;
    gs_stFileSystemManager.dwTotalClusterCount = pstMBR->dwTotalClusterCount;

    kUnlock( &( gs_stFileSystemManager.stMutex ) );
    return TRUE;
}

/**
 *  function name : kFormat
 *  parameters    : void
 *  return value  : BOOL
 *  brief         : format HDD
 */
BOOL kFormat( void )
{
    HDDINFORMATION* pstHDD;
    MBR* pstMBR;
    DWORD dwTotalSectorCount, dwRemainSectorCount;
    DWORD dwMaxClusterCount, dwClsuterCount;
    DWORD dwClusterLinkSectorCount;
    DWORD i;
    
    kLock( &( gs_stFileSystemManager.stMutex ) );

    pstHDD = ( HDDINFORMATION* ) gs_vbTempBuffer;
    if ( gs_pfReadHDDInformation( TRUE, TRUE, pstHDD ) == FALSE )
    {
        kUnlock( &( gs_stFileSystemManager.stMutex ) );
        return FALSE;
    }    
    dwTotalSectorCount = pstHDD->dwTotalSectors;
    
    dwMaxClusterCount = dwTotalSectorCount / FILESYSTEM_SECTORSPERCLUSTER;
    
    dwClusterLinkSectorCount = ( dwMaxClusterCount + 127 ) / 128;
    
    dwRemainSectorCount = dwTotalSectorCount - dwClusterLinkSectorCount - 1;
    dwClsuterCount = dwRemainSectorCount / FILESYSTEM_SECTORSPERCLUSTER;

    dwClusterLinkSectorCount = ( dwClsuterCount + 127 ) / 128;

    // READ MBR
    if ( gs_pfReadHDDSector( TRUE, TRUE, 0, 1, gs_vbTempBuffer ) == FALSE )
    {
        kUnlock( &( gs_stFileSystemManager.stMutex ) );
        return FALSE;
    }        
    
    // partition info  
    pstMBR = ( MBR* ) gs_vbTempBuffer;
    kMemSet( pstMBR->vstPartition, 0, sizeof( pstMBR->vstPartition ) );
    pstMBR->dwSignature = FILESYSTEM_SIGNATURE;
    pstMBR->dwReservedSectorCount = 0;
    pstMBR->dwClusterLinkSectorCount = dwClusterLinkSectorCount;
    pstMBR->dwTotalClusterCount = dwClsuterCount;
    
    if ( gs_pfWriteHDDSector( TRUE, TRUE, 0, 1, gs_vbTempBuffer ) == FALSE )
    {
        kUnlock( &( gs_stFileSystemManager.stMutex ) );
        return FALSE;
    }
    
    kMemSet( gs_vbTempBuffer, 0, 512 );

    for ( i = 0 ; i < ( dwClusterLinkSectorCount + FILESYSTEM_SECTORSPERCLUSTER ); i++ )
    {
        if ( i == 0 )
            ( ( DWORD* ) ( gs_vbTempBuffer ) )[ 0 ] = FILESYSTEM_LASTCLUSTER;
        else
            ( ( DWORD* ) ( gs_vbTempBuffer ) )[ 0 ] = FILESYSTEM_FREECLUSTER;
        
        if ( gs_pfWriteHDDSector( TRUE, TRUE, i + 1, 1, gs_vbTempBuffer ) == FALSE )
        {
            kUnlock( &( gs_stFileSystemManager.stMutex ) );
            return FALSE;
        }
    }    
    
    kUnlock( &( gs_stFileSystemManager.stMutex ) );
    return TRUE;
}

/**
 *  function name : kGetHDDInformation
 *  parameters    : pstInformation(HDDINFORMATION*)
 *  return value  : BOOL
 *  brief         : return HDD info
 */
BOOL kGetHDDInformation( HDDINFORMATION* pstInformation)
{
    BOOL bResult;
    
    kLock( &( gs_stFileSystemManager.stMutex ) );
    
    bResult = gs_pfReadHDDInformation( TRUE, TRUE, pstInformation );
    
    kUnlock( &( gs_stFileSystemManager.stMutex ) );
    
    return bResult;
}

/**
 *  function name : kGetHDDInformation
 *  parameters    : dwOffset(DWORD)
 *                  pbBuffer(BYTE*)
 *  return value  : BOOL
 *  brief         : read cluster in link table
 */
BOOL kReadClusterLinkTable( DWORD dwOffset, BYTE* pbBuffer )
{
    return gs_pfReadHDDSector( TRUE, TRUE, dwOffset + gs_stFileSystemManager.dwClusterLinkAreaStartAddress, 1, pbBuffer );
}

/**
 *  function name : kWriteClusterLinkTable
 *  parameters    : dwOffset(DWORD)
 *                  pbBuffer(BYTE*)
 *  return value  : BOOL
 *  brief         : write cluster in link table
 */
BOOL kWriteClusterLinkTable( DWORD dwOffset, BYTE* pbBuffer )
{
    return gs_pfWriteHDDSector( TRUE, TRUE, dwOffset + gs_stFileSystemManager.dwClusterLinkAreaStartAddress, 1, pbBuffer );
}

/**
 *  function name : kReadCluster
 *  parameters    : dwOffset(DWORD)
 *                  pbBuffer(BYTE*)
 *  return value  : BOOL
 *  brief         : read cluster
 */
BOOL kReadCluster( DWORD dwOffset, BYTE* pbBuffer )
{
    return gs_pfReadHDDSector( TRUE, TRUE, ( dwOffset * FILESYSTEM_SECTORSPERCLUSTER ) + gs_stFileSystemManager.dwDataAreaStartAddress, FILESYSTEM_SECTORSPERCLUSTER, pbBuffer );
}

/**
 *  function name : kWriteCluster
 *  parameters    : dwOffset(DWORD)
 *                  pbBuffer(BYTE*)
 *  return value  : BOOL
 *  brief         : write cluster
 */
BOOL kWriteCluster( DWORD dwOffset, BYTE* pbBuffer )
{
    return gs_pfWriteHDDSector( TRUE, TRUE, ( dwOffset * FILESYSTEM_SECTORSPERCLUSTER ) + gs_stFileSystemManager.dwDataAreaStartAddress, FILESYSTEM_SECTORSPERCLUSTER, pbBuffer );
}

/**
 *  function name : kFindFreeCluster
 *  parameters    : void
 *  return value  : DWORD
 *  brief         : find free cluster
 */
DWORD kFindFreeCluster( void )
{
    DWORD dwLinkCountInSector;
    DWORD dwLastSectorOffset, dwCurrentSectorOffset;
    DWORD i, j;
    
    // filesystem not mounted
    if ( gs_stFileSystemManager.bMounted == FALSE )
        return FILESYSTEM_LASTCLUSTER;
    
    dwLastSectorOffset = gs_stFileSystemManager.dwLastAllocatedClusterLinkSectorOffset;

    for ( i = 0 ; i < gs_stFileSystemManager.dwClusterLinkAreaSize ; i++ )
    {
        if ( ( dwLastSectorOffset + i ) == ( gs_stFileSystemManager.dwClusterLinkAreaSize - 1 ) )
            dwLinkCountInSector = gs_stFileSystemManager.dwTotalClusterCount % 128; 
        else
            dwLinkCountInSector = 128;
        
        dwCurrentSectorOffset = ( dwLastSectorOffset + i ) % gs_stFileSystemManager.dwClusterLinkAreaSize;

        if ( kReadClusterLinkTable( dwCurrentSectorOffset, gs_vbTempBuffer ) == FALSE )
            return FILESYSTEM_LASTCLUSTER;
        
        
        for ( j = 0 ; j < dwLinkCountInSector ; j++ )
        {
            if ( ( ( DWORD* ) gs_vbTempBuffer )[ j ] == FILESYSTEM_FREECLUSTER )
                break;
        }
            
        if ( j != dwLinkCountInSector )
        {
            gs_stFileSystemManager.dwLastAllocatedClusterLinkSectorOffset = dwCurrentSectorOffset;
    
            return ( dwCurrentSectorOffset * 128 ) + j;
        }
    }
    return FILESYSTEM_LASTCLUSTER;
}

/**
 *  function name : kSetClusterLinkData
 *  parameters    : dwClusterIndex(DWORD)
 *                  dwData(DWORD)
 *  return value  : BOOL
 *  brief         : init link data on cluster
 */
BOOL kSetClusterLinkData( DWORD dwClusterIndex, DWORD dwData )
{
    DWORD dwSectorOffset;
    
    if ( gs_stFileSystemManager.bMounted == FALSE )
        return FALSE;
    
    dwSectorOffset = dwClusterIndex / 128;

    if ( kReadClusterLinkTable( dwSectorOffset, gs_vbTempBuffer ) == FALSE )
        return FALSE; 
    
    ( ( DWORD* ) gs_vbTempBuffer )[ dwClusterIndex % 128 ] = dwData;

    if ( kWriteClusterLinkTable( dwSectorOffset, gs_vbTempBuffer ) == FALSE )
        return FALSE;

    return TRUE;
}

/**
 *  function name : kGetClusterLinkData
 *  parameters    : dwClusterIndex(DWORD)
 *                  dwData(DWORD)
 *  return value  : BOOL
 *  brief         : get link data on cluster
 */
BOOL kGetClusterLinkData( DWORD dwClusterIndex, DWORD* pdwData )
{
    DWORD dwSectorOffset;
    
    if ( gs_stFileSystemManager.bMounted == FALSE )
        return FALSE;
    
    dwSectorOffset = dwClusterIndex / 128;
    
    if ( dwSectorOffset > gs_stFileSystemManager.dwClusterLinkAreaSize )
        return FALSE;
    
    if ( kReadClusterLinkTable( dwSectorOffset, gs_vbTempBuffer ) == FALSE )
        return FALSE;
    
    *pdwData = ( ( DWORD* ) gs_vbTempBuffer )[ dwClusterIndex % 128 ];
    return TRUE;
}


/**
 *  function name : kFindFreeDirectoryEntry
 *  parameters    : void
 *  return value  : int
 *  brief         : get empty entry on root directory
 */
int kFindFreeDirectoryEntry( void )
{
    DIRECTORYENTRY* pstEntry;
    int i;

    if( gs_stFileSystemManager.bMounted == FALSE )
        return -1;
    
    if ( kReadCluster( 0, gs_vbTempBuffer ) == FALSE )
        return -1;
    
    pstEntry = ( DIRECTORYENTRY* ) gs_vbTempBuffer;
    for ( i = 0 ; i < FILESYSTEM_MAXDIRECTORYENTRYCOUNT ; i++ )
    {
        if ( pstEntry[ i ].dwStartClusterIndex == 0 )
            return i;
    }
    return -1;
}

/**
 *  function name : kSetDirectoryEntryData
 *  parameters    : iIndex(int)
 *                  pstEntry(DIRECTORYENTRY*)
 *  return value  : BOOL
 *  brief         : set directory entry
 */
BOOL kSetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry )
{
    DIRECTORYENTRY* pstRootEntry;
    
    if( ( gs_stFileSystemManager.bMounted == FALSE ) || ( iIndex < 0 ) || ( iIndex >= FILESYSTEM_MAXDIRECTORYENTRYCOUNT ) )
        return FALSE;
    
    if ( kReadCluster( 0, gs_vbTempBuffer ) == FALSE )
        return FALSE;  
    
    pstRootEntry = ( DIRECTORYENTRY* ) gs_vbTempBuffer;
    kMemCpy( pstRootEntry + iIndex, pstEntry, sizeof( DIRECTORYENTRY ) );

    if ( kWriteCluster( 0, gs_vbTempBuffer ) == FALSE )
        return FALSE;
    
    return TRUE;
}

/**
 *  function name : kGetDirectoryEntryData
 *  parameters    : iIndex(int)
 *                  pstEntry(DIRECTORYENTRY*)
 *  return value  : BOOL
 *  brief         : return directory entry
 */
BOOL kGetDirectoryEntryData( int iIndex, DIRECTORYENTRY* pstEntry )
{
    DIRECTORYENTRY* pstRootEntry;
    
    if( ( gs_stFileSystemManager.bMounted == FALSE ) || ( iIndex < 0 ) || ( iIndex >= FILESYSTEM_MAXDIRECTORYENTRYCOUNT ) )
        return FALSE;
    
    if ( kReadCluster( 0, gs_vbTempBuffer ) == FALSE )
        return FALSE;  
    
    pstRootEntry = ( DIRECTORYENTRY* ) gs_vbTempBuffer;
    kMemCpy( pstEntry, pstRootEntry + iIndex, sizeof( DIRECTORYENTRY ) );

    return TRUE;
}

/**
 *  function name : kFindDirectoryEntry
 *  parameters    : pcFileName(const char*)
 *                  pstEntry(DIRECTORYENTRY*)
 *  return value  : int
 *  brief         : find directory entry
 */
int kFindDirectoryEntry( const char* pcFileName, DIRECTORYENTRY* pstEntry )
{
    DIRECTORYENTRY* pstRootEntry;
    int i;
    int iLength;

    if ( gs_stFileSystemManager.bMounted == FALSE )
        return -1;
    
    if ( kReadCluster( 0, gs_vbTempBuffer ) == FALSE )
        return -1;
    
    
    iLength = kStrLen( pcFileName );
    pstRootEntry = ( DIRECTORYENTRY* ) gs_vbTempBuffer;

    for ( i = 0 ; i < FILESYSTEM_MAXDIRECTORYENTRYCOUNT ; i++ )
    {
        if ( kMemCmp( pstRootEntry[ i ].vcFileName, pcFileName, iLength ) == 0 )
        {
            kMemCpy( pstEntry, pstRootEntry + i, sizeof( DIRECTORYENTRY ) );
            return i;
        }
    }
    return -1;
}

/**
 *  function name : kGetFileSystemInformation
 *  parameters    : pstManager(FILESYSTEMMANAGER*)
 *  return value  : void
 *  brief         : return filesystem info
 */
void kGetFileSystemInformation( FILESYSTEMMANAGER* pstManager )
{
    kMemCpy( pstManager, &gs_stFileSystemManager, sizeof( gs_stFileSystemManager ) );
}