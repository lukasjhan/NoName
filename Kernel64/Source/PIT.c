/* filename          /Kernel64/Source/PIT.c
 * date              2018.11.27
 * last edit date    2018.11.27
 * author            NO.00[UNKNOWN]
 * brief             source code for PIT controller
*/

#include "PIT.h"

/**
 *  function name : kInitializePIT
 *  Parameters    : wCount(WORD)
 *                  bPeriodic(BOOL)
 *  return value  : void
 *  brief         : init PIT controller
 */
void kInitializePIT( WORD wCount, BOOL bPeriodic )
{
    // init PIT control register(Port 0x43) to stop counting
    kOutPortByte( PIT_PORT_CONTROL, PIT_COUNTER0_ONCE );
    
    // Mode 2
    if ( bPeriodic == TRUE )
        kOutPortByte( PIT_PORT_CONTROL, PIT_COUNTER0_PERIODIC );
    
    // init counter 0(Port 0x40) LSB -> MSB
    kOutPortByte( PIT_PORT_COUNTER0, wCount );
    kOutPortByte( PIT_PORT_COUNTER0, wCount >> 8 );
}

/**
 *  function name : kReadCounter0
 *  Parameters    : void
 *  return value  : WORD
 *  brief         : return counter 0's current value
 */
WORD kReadCounter0( void )
{
    BYTE bHighByte, bLowByte;
    WORD wTemp = 0;
    
    // Read current value
    kOutPortByte( PIT_PORT_CONTROL, PIT_COUNTER0_LATCH );
    
    // counter 0(port 0x40) LSB -> MSB 
    bLowByte = kInPortByte( PIT_PORT_COUNTER0 );
    bHighByte = kInPortByte( PIT_PORT_COUNTER0 );

    // value to 16bit
    wTemp = bHighByte;
    wTemp = ( wTemp << 8 ) | bLowByte;
    return wTemp;
}

/**
 *  function name : kWaitUsingDirectPIT
 *  Parameters    : wCount(WORD)
 *  return value  : void
 *  brief         : wait counting
 */
void kWaitUsingDirectPIT( WORD wCount )
{
    WORD wLastCounter0;
    WORD wCurrentCounter0;
    
    // make PIT controller repeat counting (0~0xFFFF)
    kInitializePIT( 0, TRUE );
    
    // wait
    wLastCounter0 = kReadCounter0();
    while ( 1 )
    {
        wCurrentCounter0 = kReadCounter0();
        if ( ( ( wLastCounter0 - wCurrentCounter0 ) & 0xFFFF ) >= wCount )
            break;
    }
}