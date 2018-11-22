/* filename          /Kernel64/Source/Utility.c
 * date              2018.11.20
 * last edit date    2018.11.20
 * author            NO.00[UNKNOWN]
 * brief             source code of utility functions for OS
*/

#include "Utility.h"
#include "AssemblyUtility.h"

/**
 *  function name : kMemSet
 *  Parameters    : pvDestination(void*)
 *                  bData(BYTE)
 *                  iSize(int)
 *  return value  : void
 *  brief         : set memory using input data
 */
void kMemSet( void* pvDestination, BYTE bData, int iSize )
{
    int i;
    
    for ( i = 0 ; i < iSize ; i++ )
        ( ( char* ) pvDestination )[ i ] = bData;
    
}

/**
 *  function name : kMemCpy
 *  Parameters    : pvDestination(void*)
 *                  pvSource(const void*)
 *                  iSize(int)
 *  return value  : return iSize(int)
 *  brief         : copy memory
 */
int kMemCpy( void* pvDestination, const void* pvSource, int iSize )
{
    int i;
    
    for ( i = 0 ; i < iSize ; i++ )
        ( ( char* ) pvDestination )[ i ] = ( ( char* ) pvSource )[ i ];
    
    return iSize;
}

/**
 *  function name : kMemCpy
 *  Parameters    : pvDestination(void*)
 *                  pvSource(const void*)
 *                  iSize(int)
 *  return value  : 0(int)     - success
 *                  cTemp(int) - fail(error)
 *  brief         : compare memory value
 */
int kMemCmp( const void* pvDestination, const void* pvSource, int iSize )
{
    int i;
    char cTemp;
    
    for ( i = 0 ; i < iSize ; i++ )
    {
        cTemp = ( ( char* ) pvDestination )[ i ] - ( ( char* ) pvSource )[ i ];
        if ( cTemp != 0 )
            return ( int ) cTemp;
    }
    return 0;
}

/**
 *  function name : kSetInterruptFlag
 *  Parameters    : bEnableInterrupt(BOOL)
 *  return value  : BOOL
 *  brief         : change rflag register interrupt flag, return former interrupt flag
 */
BOOL kSetInterruptFlag( BOOL bEnableInterrupt )
{
    QWORD qwRFLAGS;
    
    qwRFLAGS = kReadRFLAGS();

    if ( bEnableInterrupt == TRUE )
        kEnableInterrupt();
    else
        kDisableInterrupt();

    if ( qwRFLAGS & 0x0200 )
        return TRUE;
    
    return FALSE;
}