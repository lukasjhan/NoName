/* filename          /Kernel32/Source/Main.c
 * date              2018.11.05
 * last edit date    2018.11.05
 * author            NO.00[UNKNOWN]
 * brief             start point for C kernel
*/
#include "Types.h"

void kPrintString( int iX, int iY, const char* pcString );

/**
 *  Start Point for C Kernel
 *  MUST BE FIRST PLACE !!!
 */
void Main( void )
{
    kPrintString( 0, 3, "C Language Kernel Started~!!!" );

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
    {
        pstScreen[ i ].bCharactor = pcString[ i ];
    }
}