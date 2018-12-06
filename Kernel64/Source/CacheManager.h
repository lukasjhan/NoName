/* filename          /Kernel64/Source/CacheManager.h
 * date              2018.12.06
 * last edit date    2018.12.06
 * author            NO.00[UNKNOWN]
 * brief             header file for CacheManager.c
*/

#ifndef __CACHEMANAGER_H__
#define __CACHEMANAGER_H__

#include "Types.h"

#define CACHE_MAXCLUSTERLINKTABLEAREACOUNT      16
#define CACHE_MAXDATAAREACOUNT                  32
#define CACHE_INVALIDTAG                        0xFFFFFFFF

#define CACHE_MAXCACHETABLEINDEX                2
#define CACHE_CLUSTERLINKTABLEAREA              0
#define CACHE_DATAAREA                          1

typedef struct kCacheBufferStruct
{
    DWORD dwTag;
    DWORD dwAccessTime;
    BOOL bChanged;
    BYTE* pbBuffer;
} CACHEBUFFER;

typedef struct kCacheManagerStruct
{
    DWORD vdwAccessTime[ CACHE_MAXCACHETABLEINDEX ];
    BYTE* vpbBuffer[ CACHE_MAXCACHETABLEINDEX ];
    CACHEBUFFER vvstCacheBuffer[ CACHE_MAXCACHETABLEINDEX ][ CACHE_MAXDATAAREACOUNT ];
    DWORD vdwMaxCount[ CACHE_MAXCACHETABLEINDEX ];
} CACHEMANAGER;

BOOL kInitializeCacheManager( void );
CACHEBUFFER* kAllocateCacheBuffer( int iCacheTableIndex );
CACHEBUFFER* kFindCacheBuffer( int iCacheTableIndex, DWORD dwTag );
CACHEBUFFER* kGetVictimInCacheBuffer( int iCacheTableIndex );
void kDiscardAllCacheBuffer( int iCacheTableIndex );
BOOL kGetCacheBufferAndCount( int iCacheTableIndex, CACHEBUFFER** ppstCacheBuffer, int* piMaxCount );

static void kCutDownAccessTime( int iCacheTableIndex );

#endif /*__CACHEMANAGER_H__*/