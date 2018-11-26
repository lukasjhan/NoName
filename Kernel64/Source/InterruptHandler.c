/* filename          /Kernel64/Source/InterruptHandler.c
 * date              2018.11.22
 * last edit date    2018.11.23
 * author            NO.00[UNKNOWN]
 * brief             functions for handling interrupts
*/

#include "InterruptHandler.h"
#include "PIC.h"
#include "Keyboard.h"
#include "Console.h"

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

    kPrintStringXY( 0, 0, "====================================================" );
    kPrintStringXY( 0, 1, "                 Exception Occur~!!!!               " );
    kPrintStringXY( 0, 2, "                    Vector:                         " );
    kPrintStringXY( 27, 2, vcBuffer );
    kPrintStringXY( 0, 3, "====================================================" );

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
    kPrintStringXY( 70, 0, vcBuffer );

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
    char vcBuffer[]                      = "[INT:  , ]";
    static int g_iKeyboardInterruptCount = 0;
    BYTE bTemp;

    vcBuffer[ 5 ]                        = '0' + iVectorNumber / 10;
    vcBuffer[ 6 ]                        = '0' + iVectorNumber % 10;
    vcBuffer[ 8 ]                        = '0' + g_iKeyboardInterruptCount;
    g_iKeyboardInterruptCount            = ( g_iKeyboardInterruptCount + 1 ) % 10;
    kPrintStringXY( 0, 0, vcBuffer );

    // put data into Queue
    if ( kIsOutputBufferFull() == TRUE )
    {
        bTemp = kGetKeyboardScanCode();
        kConvertScanCodeAndPutQueue( bTemp );
    }

    // send EOI
    kSendEOIToPIC( iVectorNumber - PIC_IRQSTARTVECTOR );
}