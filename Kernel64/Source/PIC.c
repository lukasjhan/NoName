/* filename          /Kernel64/Source/PIC.c
 * date              2018.11.22
 * last edit date    2018.11.22
 * author            NO.00[UNKNOWN]
 * brief             source code for PIC controller
*/

#include "PIC.h"

/**
 *  function name : kInitializePIC
 *  Parameters    : void
 *  return value  : void
 *  brief         : init PIC
 */
void kInitializePIC( void )
{
    // init master PIC controller
    // ICW1(port 0x20), IC4 bit(bit 0) = 1
    kOutPortByte( PIC_MASTER_PORT1, 0x11 );
    // ICW2(port 0x21), interrupt vector(0x20)
    kOutPortByte( PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR );
    // ICW3(port 0x21), slave PIC controller
    // master PIC controller 0x04(bit 2)
    kOutPortByte( PIC_MASTER_PORT2, 0x04 );
    // ICW4(port 0x21), uPM bit(bit 0) = 1
    kOutPortByte( PIC_MASTER_PORT2, 0x01 );

    // init slave PIC controller
    // ICW1(port 0xA0), IC4 bit(bit 0) = 1
    kOutPortByte( PIC_SLAVE_PORT1, 0x11 );
    // ICW2(port 0xA1), interrupt vector(0x20 + 8)
    kOutPortByte( PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR + 8 );
    // ICW3(port 0xA1), master PIC controller
    // master PIC controller 0x02
    kOutPortByte( PIC_SLAVE_PORT2, 0x02 );
    // ICW4(port 0xA1), uPM bit(bit 0) = 1
    kOutPortByte( PIC_SLAVE_PORT2, 0x01 );
}

/**
 *  function name : kMaskPICInterrupt
 *  Parameters    : wIRQBitmask(WORD)
 *  return value  : void
 *  brief         : mask interrupt
 */
void kMaskPICInterrupt( WORD wIRQBitmask )
{
    // master PIC controller IMR
    // OCW1(port 0x21), IRQ 0~IRQ 7
    kOutPortByte( PIC_MASTER_PORT2, ( BYTE ) wIRQBitmask );
    
    // slave PIC controller IMR 
    // OCW1(port 0xA1), IRQ 8~IRQ 15
    kOutPortByte( PIC_SLAVE_PORT2, ( BYTE ) ( wIRQBitmask >> 8 ) );
}

/**
 *  function name : kSendEOIToPIC
 *  Parameters    : iIRQNumber(int)
 *  return value  : void
 *  brief         : interrupt handling complete(EOI)
 *                  master PIC controller, EOI send to master PIC controller
 *                  slave PIC controller, send to master and slave PIC controller
 */
void kSendEOIToPIC( int iIRQNumber )
{
    // send master PIC controller EOI
    // OCW2(port 0x20), EOI bit(bit 5) = 1
    kOutPortByte( PIC_MASTER_PORT1, 0x20 );

    if ( iIRQNumber >= 8 )
        // OCW2(port 0xA0), EOI bit(bit 5) = 1
        kOutPortByte( PIC_SLAVE_PORT1, 0x20 );
}