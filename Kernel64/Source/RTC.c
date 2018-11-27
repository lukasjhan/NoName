/* filename          /Kernel64/Source/RTC.c
 * date              2018.11.27
 * last edit date    2018.11.27
 * author            NO.00[UNKNOWN]
 * brief             source code for RTC controller
*/

#include "RTC.h"

/**
 *  function name : kReadRTCTime
 *  parameters    : pbHour(BYTE*)
 *                  pbMinute(BYTE*)
 *                  pbSecond(BYTE*)
 *  return value  : void
 *  brief         : return current time on CMOS RTC controller
 */
void kReadRTCTime( BYTE* pbHour, BYTE* pbMinute, BYTE* pbSecond )
{
    BYTE bData;
    
    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_HOUR );
    
    bData = kInPortByte( RTC_CMOSDATA );
    *pbHour = RTC_BCDTOBINARY( bData );
    
    
    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_MINUTE );
    
    bData = kInPortByte( RTC_CMOSDATA );
    *pbMinute = RTC_BCDTOBINARY( bData );
    
    
    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_SECOND );
    
    bData = kInPortByte( RTC_CMOSDATA );
    *pbSecond = RTC_BCDTOBINARY( bData );
}

/**
 *  function name : kReadRTCDate
 *  parameters    : pwYear(WORD*)
 *                  pbMonth(BYTE*)
 *                  pbDayOfMonth(BYTE*)
 *                  pbDayOfWeek(BYTE*)
 *  return value  : void
 *  brief         : return current time on CMOS RTC controller
 */
void kReadRTCDate( WORD* pwYear, BYTE* pbMonth, BYTE* pbDayOfMonth, BYTE* pbDayOfWeek )
{
    BYTE bData;
    
    
    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_YEAR );
    
    bData = kInPortByte( RTC_CMOSDATA );
    *pwYear = RTC_BCDTOBINARY( bData ) + 2000;
    
    
    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_MONTH );
    
    bData = kInPortByte( RTC_CMOSDATA );
    *pbMonth = RTC_BCDTOBINARY( bData );
    
    
    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFMONTH );
    
    bData = kInPortByte( RTC_CMOSDATA );
    *pbDayOfMonth = RTC_BCDTOBINARY( bData );
    
    
    kOutPortByte( RTC_CMOSADDRESS, RTC_ADDRESS_DAYOFWEEK );
    
    bData = kInPortByte( RTC_CMOSDATA );
    *pbDayOfWeek = RTC_BCDTOBINARY( bData );
}

/**
 *  function name : kConvertDayOfWeekToString
 *  parameters    : pbDayOfWeek(BYTE)
 *  return value  : char*
 *  brief         : convert dayofweek to string
 */
char* kConvertDayOfWeekToString( BYTE bDayOfWeek )
{
    static char* vpcDayOfWeekString[ 8 ] = { "Error", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    
    if ( bDayOfWeek >= 8 )
        return vpcDayOfWeekString[ 0 ];
    
    return vpcDayOfWeekString[ bDayOfWeek ];
}