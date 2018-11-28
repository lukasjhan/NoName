/* filename          /Kernel64/Source/Utility.c
 * date              2018.11.20
 * last edit date    2018.11.28
 * author            NO.00[UNKNOWN]
 * brief             source code of utility functions for OS
*/

#include <stdarg.h>

#include "Utility.h"
#include "AssemblyUtility.h"

// counter for PIT controller
volatile QWORD g_qwTickCount = 0;

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

/**
 *  function name : kStrLen
 *  Parameters    : pcBuffer(const char*)
 *  return value  : int
 *  brief         : return string length
 */
int kStrLen( const char* pcBuffer )
{
    int i;
    
    for ( i = 0 ; ; i++ )
    {
        if ( pcBuffer[ i ] == '\0' )
            break;
    }
    return i;
}

// RAM size(MB)
static gs_qwTotalRAMMBSize = 0;

/**
 *  function name : kCheckTotalRAMSize
 *  Parameters    : void
 *  return value  : void
 *  brief         : check RAM from 64MB, CALL ONE TIME ONLY!!
 */
void kCheckTotalRAMSize( void )
{
    DWORD* pdwCurrentAddress;
    DWORD dwPreviousValue;
    
    pdwCurrentAddress = ( DWORD* ) 0x4000000;
    while ( 1 )
    {
        dwPreviousValue = *pdwCurrentAddress;

        *pdwCurrentAddress = 0x12345678;
        if( *pdwCurrentAddress != 0x12345678 )
            break;
        
        *pdwCurrentAddress = dwPreviousValue;
        pdwCurrentAddress += ( 0x400000 / 4 );
    }
    gs_qwTotalRAMMBSize = ( QWORD ) pdwCurrentAddress / 0x100000;
}

/**
 *  function name : kGetTotalRAMSize
 *  Parameters    : void
 *  return value  : QWORD
 *  brief         : return RAM size
 */
QWORD kGetTotalRAMSize( void )
{
    return gs_qwTotalRAMMBSize;
}

/**
 *  function name : kAToI
 *  Parameters    : pcBuffer(cosnt char*)
 *                  iRadix(int)
 *  return value  : long
 *  brief         : atoi function
 */
long kAToI( const char* pcBuffer, int iRadix )
{
    long lReturn;
    
    switch ( iRadix )
    {
    case 16:
        lReturn = kHexStringToQword( pcBuffer );
        break;
        
    case 10:
    default:
        lReturn = kDecimalStringToLong( pcBuffer );
        break;
    }
    return lReturn;
}

/**
 *  function name : kHexStringToQword
 *  Parameters    : pcBuffer(cosnt char*)
 *  return value  : QWORD
 *  brief         : hex to QWORD
 */
QWORD kHexStringToQword( const char* pcBuffer )
{
    QWORD qwValue = 0;
    int i;
    
    for ( i = 0 ; pcBuffer[ i ] != '\0' ; i++ )
    {
        qwValue *= 16;
        if ( ( 'A' <= pcBuffer[ i ] )  && ( pcBuffer[ i ] <= 'Z' ) )
            qwValue += ( pcBuffer[ i ] - 'A' ) + 10;
        else if ( ( 'a' <= pcBuffer[ i ] )  && ( pcBuffer[ i ] <= 'z' ) )
            qwValue += ( pcBuffer[ i ] - 'a' ) + 10;
        else 
            qwValue += pcBuffer[ i ] - '0';
    }
    return qwValue;
}

/**
 *  function name : kDecimalStringToLong
 *  Parameters    : pcBuffer(cosnt char*)
 *  return value  : long
 *  brief         : dec to long
 */
long kDecimalStringToLong( const char* pcBuffer )
{
    long lValue = 0;
    int i;
    
    if ( pcBuffer[ 0 ] == '-' )
        i = 1;
    else
        i = 0;
    
    for ( ; pcBuffer[ i ] != '\0' ; i++ )
    {
        lValue *= 10;
        lValue += pcBuffer[ i ] - '0';
    }
    
    if ( pcBuffer[ 0 ] == '-' )
        lValue = -lValue;
    
    return lValue;
}

/**
 *  function name : kIToA
 *  Parameters    : lValue(long)
 *                  pcBuffer(char*)
 *                  iRadix(int)
 *  return value  : int
 *  brief         : itoa function
 */
int kIToA( long lValue, char* pcBuffer, int iRadix )
{
    int iReturn;
    
    switch ( iRadix )
    {
    case 16:
        iReturn = kHexToString( lValue, pcBuffer );
        break;
        
    case 10:
    default:
        iReturn = kDecimalToString( lValue, pcBuffer );
        break;
    }
    
    return iReturn;
}

/**
 *  function name : kHexToString
 *  Parameters    : qwValue(QWORD)
 *                  pcBuffer(char*)
 *  return value  : int
 *  brief         : hex to string
 */
int kHexToString( QWORD qwValue, char* pcBuffer )
{
    QWORD i;
    QWORD qwCurrentValue;

    if ( qwValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    for ( i = 0 ; qwValue > 0 ; i++ )
    {
        qwCurrentValue = qwValue % 16;

        if ( qwCurrentValue >= 10 )
            pcBuffer[ i ] = 'A' + ( qwCurrentValue - 10 );
        else
            pcBuffer[ i ] = '0' + qwCurrentValue;
    
        qwValue = qwValue / 16;
    }
    pcBuffer[ i ] = '\0';
    
    kReverseString( pcBuffer );
    return i;
}

/**
 *  function name : kDecimalToString
 *  Parameters    : lValue(long)
 *                  pcBuffer(char*)
 *  return value  : int
 *  brief         : dec to string
 */
int kDecimalToString( long lValue, char* pcBuffer )
{
    long i;

    if ( lValue == 0 )
    {
        pcBuffer[ 0 ] = '0';
        pcBuffer[ 1 ] = '\0';
        return 1;
    }
    
    if ( lValue < 0 )
    {
        i = 1;
        pcBuffer[ 0 ] = '-';
        lValue = -lValue;
    }
    else
        i = 0;
    
    for ( ; lValue > 0 ; i++ )
    {
        pcBuffer[ i ] = '0' + lValue % 10;        
        lValue = lValue / 10;
    }
    pcBuffer[ i ] = '\0';
    
    if ( pcBuffer[ 0 ] == '-' )
        kReverseString( &( pcBuffer[ 1 ] ) );
    else
        kReverseString( pcBuffer );
    
    return i;
}

/**
 *  function name : kReverseString
 *  Parameters    : pcBuffer(char*)
 *  return value  : void
 *  brief         : reverse string
 */
void kReverseString( char* pcBuffer )
{
   int iLength;
   int i;
   char cTemp;
   
   iLength = kStrLen( pcBuffer );

   for ( i = 0 ; i < iLength / 2 ; i++ )
   {
       cTemp = pcBuffer[ i ];
       pcBuffer[ i ] = pcBuffer[ iLength - 1 - i ];
       pcBuffer[ iLength - 1 - i ] = cTemp;
   }
}

/**
 *  function name : kSPrintf
 *  Parameters    : pcBuffer(char*)
 *                  pcFormatString, ...(const char*)
 *  return value  : int
 *  brief         : sprintf function
 */
int kSPrintf( char* pcBuffer, const char* pcFormatString, ... )
{
    va_list ap;
    int iReturn;
    
    va_start( ap, pcFormatString );
    iReturn = kVSPrintf( pcBuffer, pcFormatString, ap );
    va_end( ap );
    
    return iReturn;
}

/**
 *  function name : kVSPrintf
 *  Parameters    : pcBuffer(char*)
 *                  pcFormatString(const char*)
 *                  ap(va_list)
 *  return value  : int
 *  brief         : vsprint function
 */
int kVSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap )
{
    QWORD i, j;
    int iBufferIndex = 0;
    int iFormatLength, iCopyLength;
    char* pcCopyString;
    QWORD qwValue;
    int iValue;
    
    iFormatLength = kStrLen( pcFormatString );
    for ( i = 0 ; i < iFormatLength ; i++ ) 
    {
        // format string start with %
        if ( pcFormatString[ i ] == '%' ) 
        {
            // % 다음의 문자로 이동
            i++;
            switch ( pcFormatString[ i ] ) 
            {
            // char  
            case 's':
                pcCopyString = ( char* ) ( va_arg(ap, char* ));
                iCopyLength = kStrLen( pcCopyString );
                kMemCpy( pcBuffer + iBufferIndex, pcCopyString, iCopyLength );
                iBufferIndex += iCopyLength;
                break;
            
            // char
            case 'c':
                pcBuffer[ iBufferIndex ] = ( char ) ( va_arg( ap, int ) );
                iBufferIndex++;
                break;

            // dec
            case 'd':
            case 'i':
                iValue = ( int ) ( va_arg( ap, int ) );
                iBufferIndex += kIToA( iValue, pcBuffer + iBufferIndex, 10 );
                break;
                
            // 4byte hex
            case 'x':
            case 'X':
                qwValue = ( DWORD ) ( va_arg( ap, DWORD ) ) & 0xFFFFFFFF;
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

            // 8byte hex
            case 'q':
            case 'Q':
            case 'p':
                qwValue = ( QWORD ) ( va_arg( ap, QWORD ) );
                iBufferIndex += kIToA( qwValue, pcBuffer + iBufferIndex, 16 );
                break;

            case 'f':
                dValue = ( double) ( va_arg( ap, double ) );
                // Round-off
                dValue += 0.005;

                pcBuffer[ iBufferIndex ] = '0' + ( QWORD ) ( dValue * 100 ) % 10;
                pcBuffer[ iBufferIndex + 1 ] = '0' + ( QWORD ) ( dValue * 10 ) % 10;
                pcBuffer[ iBufferIndex + 2 ] = '.';
                for ( k = 0 ; ; k++ )
                {
                    if ( ( ( QWORD ) dValue == 0 ) && ( k != 0 ) )
                        break;
                    
                    pcBuffer[ iBufferIndex + 3 + k ] = '0' + ( ( QWORD ) dValue % 10 );
                    dValue = dValue / 10;
                }
                pcBuffer[ iBufferIndex + 3 + k ] = '\0';
                kReverseString( pcBuffer + iBufferIndex );
                iBufferIndex += 3 + k;
                break;

            default:
                pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
                iBufferIndex++;
                break;
            }
        } 
        // normal string
        else
        {
            pcBuffer[ iBufferIndex ] = pcFormatString[ i ];
            iBufferIndex++;
        }
    }
    
    // input NULL
    pcBuffer[ iBufferIndex ] = '\0';
    return iBufferIndex;
}

/**
 *  function name : kGetTickCount
 *  Parameters    : void
 *  return value  : QWORD
 *  brief         : return tick count
 */
QWORD kGetTickCount( void )
{
    return g_qwTickCount;
}

/**
 *  function name : kSleep
 *  Parameters    : qwMillisecond(QWORD)
 *  return value  : void
 *  brief         : sleep millisecond
 */
void kSleep( QWORD qwMillisecond )
{
    QWORD qwLastTickCount;
    
    qwLastTickCount = g_qwTickCount;
    
    while ( ( g_qwTickCount - qwLastTickCount ) <= qwMillisecond )
        kSchedule();
}