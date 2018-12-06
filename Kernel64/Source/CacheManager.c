/* filename          /Kernel64/Source/CacheManager.c
 * date              2018.11.23
 * last edit date    2018.11.28
 * author            NO.00[UNKNOWN]
 * brief             source code for caching
*/

#include "CacheManager.h"
#include "FileSystem.h"
#include "DynamicMemory.h"

static CACHEMANAGER gs_stCacheManager;

/**
 *  function name : kInitializeCacheManager
 *  parameters    : void
 *  return value  : BOOL
 *  brief         : init caching manager
 */
BOOL kInitializeCacheManager( void )
{
    int i;
    
    kMemSet( &gs_stCacheManager, 0, sizeof( gs_stCacheManager ) );
    
    gs_stCacheManager.vdwAccessTime[ CACHE_CLUSTERLINKTABLEAREA ] = 0;
    gs_stCacheManager.vdwAccessTime[ CACHE_DATAAREA ] = 0;

    gs_stCacheManager.vdwMaxCount[ CACHE_CLUSTERLINKTABLEAREA ] = CACHE_MAXCLUSTERLINKTABLEAREACOUNT;
    gs_stCacheManager.vdwMaxCount[ CACHE_DATAAREA ] = CACHE_MAXDATAAREACOUNT;
    
    gs_stCacheManager.vpbBuffer[ CACHE_CLUSTERLINKTABLEAREA ] = ( BYTE* ) kAllocateMemory( CACHE_MAXCLUSTERLINKTABLEAREACOUNT * 512 );

    if ( gs_stCacheManager.vpbBuffer[ CACHE_CLUSTERLINKTABLEAREA ] == NULL )
        return FALSE;
    
    for ( i = 0 ; i < CACHE_MAXCLUSTERLINKTABLEAREACOUNT ; i++ )
    {
        gs_stCacheManager.vvstCacheBuffer[ CACHE_CLUSTERLINKTABLEAREA ][ i ].pbBuffer = gs_stCacheManager.vpbBuffer[ CACHE_CLUSTERLINKTABLEAREA ] + ( i * 512 );
        gs_stCacheManager.vvstCacheBuffer[ CACHE_CLUSTERLINKTABLEAREA ][ i ].dwTag = CACHE_INVALIDTAG;
    }
    
    gs_stCacheManager.vpbBuffer[ CACHE_DATAAREA ] = ( BYTE* ) kAllocateMemory( CACHE_MAXDATAAREACOUNT * FILESYSTEM_CLUSTERSIZE );

    if ( gs_stCacheManager.vpbBuffer[ CACHE_DATAAREA ] == NULL )
    {
        kFreeMemory( gs_stCacheManager.vpbBuffer[ CACHE_CLUSTERLINKTABLEAREA ] );
        return FALSE;
    }
    
    for ( i = 0 ; i < CACHE_MAXDATAAREACOUNT ; i++ )
    {
        gs_stCacheManager.vvstCacheBuffer[ CACHE_DATAAREA ][ i ].pbBuffer = gs_stCacheManager.vpbBuffer[ CACHE_DATAAREA ] + ( i * FILESYSTEM_CLUSTERSIZE );
        gs_stCacheManager.vvstCacheBuffer[ CACHE_DATAAREA ][ i ].dwTag = CACHE_INVALIDTAG;
    }
    
    return TRUE;
}

/**
 *  function name : kAllocateCacheBuffer
 *  parameters    : iCacheTableIndex(int)
 *  return value  : CACHEBUFFER*
 *  brief         : get empty cache buffer
 */
CACHEBUFFER* kAllocateCacheBuffer( int iCacheTableIndex )
{
    CACHEBUFFER* pstCacheBuffer;
    int i;
    
    if ( iCacheTableIndex > CACHE_MAXCACHETABLEINDEX )
        return FALSE;
    
    kCutDownAccessTime( iCacheTableIndex );

    pstCacheBuffer = gs_stCacheManager.vvstCacheBuffer[ iCacheTableIndex ];

    for ( i = 0 ; i < gs_stCacheManager.vdwMaxCount[ iCacheTableIndex ] ; i++ )
    {
        if ( pstCacheBuffer[ i ].dwTag == CACHE_INVALIDTAG )
        {
            pstCacheBuffer[ i ].dwTag = CACHE_INVALIDTAG - 1;
            pstCacheBuffer[ i ].dwAccessTime = gs_stCacheManager.vdwAccessTime[ iCacheTableIndex ]++;
            
            return &( pstCacheBuffer[ i ] );
        }
    }
    
    return NULL;
}

/**
 *  function name : kFindCacheBuffer
 *  parameters    : iCacheTableIndex(int)
 *                  dwTag(DWORD)
 *  return value  : CACHEBUFFER*
 *  brief         : find cache buffer
 */
CACHEBUFFER* kFindCacheBuffer( int iCacheTableIndex, DWORD dwTag )
{
    CACHEBUFFER* pstCacheBuffer;
    int i;
    
    if( iCacheTableIndex > CACHE_MAXCACHETABLEINDEX )
        return FALSE;
    
    kCutDownAccessTime( iCacheTableIndex );
    
    pstCacheBuffer = gs_stCacheManager.vvstCacheBuffer[ iCacheTableIndex ];

    for ( i = 0 ; i < gs_stCacheManager.vdwMaxCount[ iCacheTableIndex ] ; i++ )
    {
        if ( pstCacheBuffer[ i ].dwTag == dwTag )
        {
            pstCacheBuffer[ i ].dwAccessTime = gs_stCacheManager.vdwAccessTime[ iCacheTableIndex ]++;
            
            return &( pstCacheBuffer[ i ] );
        }
    }
    
    return NULL;
}

/**
 *  function name : kCutDownAccessTime
 *  parameters    : iCacheTableIndex(int)
 *  return value  : void
 *  brief         : optimization
 */
static void kCutDownAccessTime( int iCacheTableIndex )
{
    CACHEBUFFER stTemp;
    CACHEBUFFER* pstCacheBuffer;
    BOOL bSorted;
    int i, j;

    if ( iCacheTableIndex > CACHE_MAXCACHETABLEINDEX )
        return ;

    if ( gs_stCacheManager.vdwAccessTime[ iCacheTableIndex ] < 0xfffffffe )
        return ;
    
    pstCacheBuffer = gs_stCacheManager.vvstCacheBuffer[ iCacheTableIndex ];

    for ( j = 0 ; j < gs_stCacheManager.vdwMaxCount[ iCacheTableIndex ] - 1 ; j++ )
    {
        bSorted = TRUE;

        for ( i = 0 ; i < gs_stCacheManager.vdwMaxCount[ iCacheTableIndex ] - 1 - j ; i++ )
        {
            if ( pstCacheBuffer[ i ].dwAccessTime > pstCacheBuffer[ i + 1 ].dwAccessTime )
            {
                bSorted = FALSE;

                kMemCpy( &stTemp, &( pstCacheBuffer[ i ] ), sizeof( CACHEBUFFER ) );
                kMemCpy( &( pstCacheBuffer[ i ] ), &( pstCacheBuffer[ i + 1 ] ), sizeof( CACHEBUFFER ) );
                kMemCpy( &( pstCacheBuffer[ i + 1 ] ), &stTemp, sizeof( CACHEBUFFER ) );
            }
        }

        if ( bSorted == TRUE )
            break;
    }

    for( i = 0 ; i < gs_stCacheManager.vdwMaxCount[ iCacheTableIndex ] ; i++ )
        pstCacheBuffer[ i ].dwAccessTime = i;
    
    gs_stCacheManager.vdwAccessTime[ iCacheTableIndex ] = i;
}

/**
 *  function name : kGetVictimInCacheBuffer
 *  parameters    : iCacheTableIndex(int)
 *  return value  : CACHEBUFFER*
 *  brief         : retern cache buffer
 */
CACHEBUFFER* kGetVictimInCacheBuffer( int iCacheTableIndex )
{
    DWORD dwOldTime;
    CACHEBUFFER* pstCacheBuffer;
    int iOldIndex;
    int i;

    if ( iCacheTableIndex > CACHE_MAXCACHETABLEINDEX )
        return FALSE;
    
    iOldIndex = -1;
    dwOldTime = 0xFFFFFFFF;
    pstCacheBuffer = gs_stCacheManager.vvstCacheBuffer[ iCacheTableIndex ];

    for ( i = 0 ; i < gs_stCacheManager.vdwMaxCount[ iCacheTableIndex ] ; i++ )
    {
        if ( pstCacheBuffer[ i ].dwTag == CACHE_INVALIDTAG )
        {
            iOldIndex = i;
            break;
        }

        if ( pstCacheBuffer[ i ].dwAccessTime < dwOldTime )
        {
            dwOldTime = pstCacheBuffer[ i ].dwAccessTime;
            iOldIndex = i;
        }
    }

    if ( iOldIndex == -1 )
    {
        kPrintf( "Cache Buffer Find Error\n" );
        return NULL;
    }

    pstCacheBuffer[ iOldIndex ].dwAccessTime = gs_stCacheManager.vdwAccessTime[ iCacheTableIndex ]++;
    
    return &( pstCacheBuffer[ iOldIndex ] );
}

/**
 *  function name : kDiscardAllCacheBuffer
 *  parameters    : iCacheTableIndex(int)
 *  return value  : void
 *  brief         : empty cache buffer
 */
void kDiscardAllCacheBuffer( int iCacheTableIndex )
{
    CACHEBUFFER* pstCacheBuffer;
    int i;
    
    pstCacheBuffer = gs_stCacheManager.vvstCacheBuffer[ iCacheTableIndex ];

    for ( i = 0 ; i < gs_stCacheManager.vdwMaxCount[ iCacheTableIndex ] ; i++ )
        pstCacheBuffer[ i ].dwTag = CACHE_INVALIDTAG;
    
    gs_stCacheManager.vdwAccessTime[ iCacheTableIndex ] = 0;
}

/**
 *  function name : kGetCacheBufferAndCount
 *  parameters    : iCacheTableIndex(int)
 *                  ppstCacheBuffer(CACHEBUFFER*)
 *                  piMaxCount(int*)
 *  return value  : void
 *  brief         : count cache buffer
 */
BOOL kGetCacheBufferAndCount( int iCacheTableIndex, CACHEBUFFER** ppstCacheBuffer, int* piMaxCount )
{
    if( iCacheTableIndex > CACHE_MAXCACHETABLEINDEX )
        return FALSE;
    
    *ppstCacheBuffer = gs_stCacheManager.vvstCacheBuffer[ iCacheTableIndex ];
    *piMaxCount = gs_stCacheManager.vdwMaxCount[ iCacheTableIndex ];
    
    return TRUE;
}