/* filename          /Kernel64/Source/InterruptHandler.c
 * date              2018.11.22
 * last edit date    2018.11.22
 * author            NO.00[UNKNOWN]
 * brief             functions for handling interrupts
*/

#include "InterruptHandler.h"
#include "PIC.h"

/**
 *  function name : kCommonExceptionHandler
 *  Parameters    : iVectorNumber(int)
 *                  qwErrorCode(QWORD)
 *  return value  : void
 *  brief         : common exception handler
 */
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode )
{
    char vcBuffer[ 3 ] = { 0, };

    vcBuffer[ 0 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 1 ] = '0' + iVectorNumber % 10;

    kPrintString( 0, 0, "====================================================" );
    kPrintString( 0, 1, "                 Exception Occur~!!!!               " );
    kPrintString( 0, 2, "                    Vector:                         " );
    kPrintString( 27, 2, vcBuffer );
    kPrintString( 0, 3, "====================================================" );

    while( 1 ) ;
}

/**
 *  function name : kCommonInterruptHandler
 *  Parameters    : iVectorNumber(int)
 *  return value  : void
 *  brief         : common interrupt handler
 */
void kCommonInterruptHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iCommonInterruptCount = 0;

    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    vcBuffer[ 8 ] = '0' + g_iCommonInterruptCount;
    g_iCommonInterruptCount = ( g_iCommonInterruptCount + 1 ) % 10;
    kPrintString( 70, 0, vcBuffer );

    // send EOI
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}

/**
 *  function name : kKeyboardHandler
 *  Parameters    : iVectorNumber(int)
 *  return value  : void
 *  brief         : interrupt handler for keyboard
 */
void kKeyboardHandler( int iVectorNumber )
{
    char vcBuffer[] = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;

    vcBuffer[ 5 ] = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ] = '0' + iVectorNumber % 10;
    vcBuffer[ 8 ] = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintString( 0, 0, vcBuffer );

    // send EOI
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}