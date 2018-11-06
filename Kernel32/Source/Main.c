/* filename          /Kernel32/Source/Main.c
 * date              2018.11.05
 * last edit date    2018.11.06
 * author            NO.00[UNKNOWN]
 * brief             start point for C kernel
*/
#include "Types.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );

/**
 *  Start Point for C Kernel
 *  MUST BE FIRST PLACE !!!
 *  Address 0x10200
 */
void Main( void )
{
    DWORD i;

    kPrintString( 0, 3, "C Language Kernel Started~!!!" );

    kPrintString( 0, 4, "Minimum Memory Size Check...................[    ]" );
    if ( kIsMemoryEnough() == FALSE )
    {
        kPrintString( 45, 4, "Fail" );
        kPrintString( 0, 5, "Not Enough Memory~!! MINT64 OS Requires Over 64Mbyte Memory~!!" );
        while ( 1 ) ;
    }
    else kPrintString( 45, 4, "Pass" );
    
    kPrintString( 0, 5, "IA-32e Kernel Area Initialize...............[    ]" );
    if ( kInitializeKernel64Area() == FALSE )
    {
        kPrintString( 45, 5, "Fail" );
        kPrintString( 0, 6, "Kernel Area Initialization Fail~!!" );
        while ( 1 ) ;
    }
    kPrintString( 45, 5, "Pass" );

    while( 1 ) ;
}

/**
 *  function name : kPrintString
 *  Parameters    : iX(int) - screen x corr
 *                  iY(int) - screen y corr
 *                  pcStirng(const char*) - string address
 *  brief         : print string to screen
 */
void kPrintString( int iX, int iY, const char* pcString )
{
    CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
    int i;
    
    pstScreen += ( iY * 80 ) + iX;
    
    for ( i = 0 ; pcString[ i ] != 0 ; i++ )
        pstScreen[ i ].bCharactor = pcString[ i ];
    
}

/**
 *  function name : kPrintString
 *  return type   : BOOL
 *  return value  : FALSE - memory error
 *                  TRUE  - init success
 *  brief         : init memory
 */
BOOL kInitializeKernel64Area( void )
{
    DWORD* pdwCurrentAddress;

    // Init start address : 1MB
    pdwCurrentAddress = ( DWORD* ) 0x100000;

    // 1~6MB init by 0
    while ( ( DWORD ) pdwCurrentAddress < 0x600000 )
    {
        *pdwCurrentAddress = 0x00;

        // memory error
        if ( *pdwCurrentAddress != 0 ) return FALSE;

        pdwCurrentAddress++;
    }
    return TRUE;
}

/**
 *  function name : kIsMemoryEnough
 *  return type   : BOOL
 *  return value  : FALSE - memory error
 *                  TRUE  - check success
 *  brief         : check memory
 */
BOOL kIsMemoryEnough( void )
{
    DWORD* pdwCurrentAddress;
   
    // check memory from 0x100000(1MB)
    pdwCurrentAddress = ( DWORD* ) 0x100000;
    
    // to 64MB
    while ( ( DWORD ) pdwCurrentAddress < 0x4000000 )
    {
        // write random value to check memory
        *pdwCurrentAddress = 0x12345678;
        if( *pdwCurrentAddress != 0x12345678 ) return FALSE;
        
        pdwCurrentAddress += ( 0x100000 / 4 );
    }
    return TRUE;
}