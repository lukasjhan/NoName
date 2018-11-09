/* filename          /Kernel64/Source/Main.c
 * date              2018.11.09
 * last edit date    2018.11.09
 * author            NO.00[UNKNOWN]
 * brief             start point for 64bit C kernel
*/

#include "Types.h"

void kPrintString( int iX, int iY, const char* pcString );

/**
 *  Start Point for C Kernel
 *  MUST BE FIRST PLACE !!!
 *  Address 0x10200
 */
void Main( void )
{
    kPrintString( 0, 10, "Switch To IA-32e Mode Success!");
    kPrintString( 0, 10, "IA-32e C Kernel Start..............[PASS]");
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