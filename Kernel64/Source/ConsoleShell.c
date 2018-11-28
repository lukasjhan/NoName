/* filename          /Kernel64/Source/ConsoleShell.c
 * date              2018.11.23
 * last edit date    2018.11.23
 * author            NO.00[UNKNOWN]
 * brief             source code for shell
*/

#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "RTC.h"
#include "AssemblyUtility.h"
#include "Task.h"

SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", kHelp },
        { "cls", "Clear Screen", kCls },
        { "totalram", "Show Total RAM Size", kShowTotalRAMSize },
        { "strtod", "String To Decial/Hex Convert", kStringToDecimalHexTest },
        { "shutdown", "Shutdown And Reboot OS", kShutdown },
        { "settimer", "Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)", kSetTimer },
        { "wait", "Wait ms Using PIT, ex)wait 100(ms)", kWaitUsingPIT },
        { "rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter },
        { "cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed },
        { "date", "Show Date And Time", kShowDateAndTime },
        { "createtask", "Create Task, ex)createtask 1(type) 10(count)", kCreateTestTask },
        { "changepriority", "Change Task Priority, ex)changepriority 1(ID) 2(Priority)", kChangeTaskPriority },
        { "tasklist", "Show Task List", kShowTaskList },
        { "killtask", "End Task, ex)killtask 1(ID)", kKillTask },
        { "cpuload", "Show Processor Load", kCPULoad },
};                                     

/**
 *  function name : kStartConsoleShell
 *  parameters    : void
 *  return value  : void
 *  brief         : shell main function
 */
void kStartConsoleShell( void )
{
    char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
    int iCommandBufferIndex = 0;
    BYTE bKey;
    int iCursorX, iCursorY;
    
    // print prompt
    kPrintf( CONSOLESHELL_PROMPTMESSAGE );
    
    while ( 1 )
    {
        bKey = kGetCh();

        if ( bKey == KEY_BACKSPACE )
        {
            if ( iCommandBufferIndex > 0 )
            {
                kGetCursor( &iCursorX, &iCursorY );
                kPrintStringXY( iCursorX - 1, iCursorY, " " );
                kSetCursor( iCursorX - 1, iCursorY );
                iCommandBufferIndex--;
            }
        }
    
        else if ( bKey == KEY_ENTER )
        {
            kPrintf( "\n" );
            
            if( iCommandBufferIndex > 0 )
            {
                vcCommandBuffer[ iCommandBufferIndex ] = '\0';
                kExecuteCommand( vcCommandBuffer );
            }
            
            kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE );            
            kMemSet( vcCommandBuffer, '\0', CONSOLESHELL_MAXCOMMANDBUFFERCOUNT );
            iCommandBufferIndex = 0;
        }
        
        else if ( ( bKey == KEY_LSHIFT ) || ( bKey == KEY_RSHIFT ) ||
                 ( bKey == KEY_CAPSLOCK ) || ( bKey == KEY_NUMLOCK ) ||
                 ( bKey == KEY_SCROLLLOCK ) )
        {
            ; // IGNORE
        }
        else
        {
            
            if ( bKey == KEY_TAB )
            {
                bKey = ' ';
            }
            
            if ( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
            {
                vcCommandBuffer[ iCommandBufferIndex++ ] = bKey;
                kPrintf( "%c", bKey );
            }
        }
    }
}

/**
 *  function name : kExecuteCommand
 *  parameters    : pcCommandBuffer(const char*)
 *  return value  : void
 *  brief         : execute command
 */
void kExecuteCommand( const char* pcCommandBuffer )
{
    int i, iSpaceIndex;
    int iCommandBufferLength, iCommandLength;
    int iCount;
    
    iCommandBufferLength = kStrLen( pcCommandBuffer );
    for ( iSpaceIndex = 0 ; iSpaceIndex < iCommandBufferLength ; iSpaceIndex++ )
    {
        if ( pcCommandBuffer[ iSpaceIndex ] == ' ' )
            break;
    }
    
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );
    for ( i = 0 ; i < iCount ; i++ )
    {
        iCommandLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if ( ( iCommandLength == iSpaceIndex ) && ( kMemCmp( gs_vstCommandTable[ i ].pcCommand, pcCommandBuffer, iSpaceIndex ) == 0 ) )
        {
            gs_vstCommandTable[ i ].pfFunction( pcCommandBuffer + iSpaceIndex + 1 );
            break;
        }
    }

    if ( i >= iCount )
        kPrintf( "'%s' is not found.\n", pcCommandBuffer );
}

/**
 *  function name : kInitializeParameter
 *  parameters    : pstList(PARAMETERLIST*)
 *                  pcParameter(const char*)
 *  return value  : void
 *  brief         : init param
 */
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
    pstList->pcBuffer = pcParameter;
    pstList->iLength = kStrLen( pcParameter );
    pstList->iCurrentPosition = 0;
}

/**
 *  function name : kGetNextParameter
 *  parameters    : pstList(PARAMETERLIST*)
 *                  pcParameter(char*)
 *  return value  : void
 *  brief         : get next param
 */
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
    int i;
    int iLength;

    if ( pstList->iLength <= pstList->iCurrentPosition )
        return 0;
    
    for ( i = pstList->iCurrentPosition ; i < pstList->iLength ; i++ )
    {
        if ( pstList->pcBuffer[ i ] == ' ' )
            break;
    }
    
    kMemCpy( pcParameter, pstList->pcBuffer + pstList->iCurrentPosition, i );
    iLength                 = i - pstList->iCurrentPosition;
    pcParameter[ iLength ]  = '\0';

    pstList->iCurrentPosition += iLength + 1;
    return iLength;
}
    
// COMMANDS

/**
 *  function name : kHelp
 *  parameters    : pcCommandBuffer(const char*)
 *  return value  : void
 *  brief         : help command
 */
static void kHelp( const char* pcCommandBuffer )
{
    int i;
    int iCount;
    int iCursorX, iCursorY;
    int iLength, iMaxCommandLength = 0;
    
    kPrintf( "=========================================================\n" );
    kPrintf( "                    NONAME Shell Help                    \n" );
    kPrintf( "=========================================================\n" );
    
    iCount = sizeof( gs_vstCommandTable ) / sizeof( SHELLCOMMANDENTRY );

    for ( i = 0 ; i < iCount ; i++ )
    {
        iLength = kStrLen( gs_vstCommandTable[ i ].pcCommand );
        if ( iLength > iMaxCommandLength )
            iMaxCommandLength = iLength;
    }
    
    for ( i = 0 ; i < iCount ; i++ )
    {
        kPrintf( "%s", gs_vstCommandTable[ i ].pcCommand );
        kGetCursor( &iCursorX, &iCursorY );
        kSetCursor( iMaxCommandLength, iCursorY );
        kPrintf( "  - %s\n", gs_vstCommandTable[ i ].pcHelp );
    }
}

/**
 *  function name : kCls
 *  parameters    : pcCommandBuffer(const char*)
 *  return value  : void
 *  brief         : clear screen
 */
static void kCls( const char* pcParameterBuffer )
{
    kClearScreen();
    kSetCursor( 0, 1 );
}

/**
 *  function name : kShowTotalRAMSize
 *  parameters    : pcCommandBuffer(const char*)
 *  return value  : void
 *  brief         : show RAM size
 */
static void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  function name : kStringToDecimalHexTest
 *  parameters    : pcCommandBuffer(const char*)
 *  return value  : void
 *  brief         : convert text to number
 */
static void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
    char vcParameter[ 100 ];
    int iLength;
    PARAMETERLIST stList;
    int iCount = 0;
    long lValue;
    
    kInitializeParameter( &stList, pcParameterBuffer );
    
    while ( 1 )
    {
        iLength = kGetNextParameter( &stList, vcParameter );
        if ( iLength == 0 )
            break;
        
        kPrintf( "Param %d = '%s', Length = %d, ", iCount + 1, vcParameter, iLength );

        if ( kMemCmp( vcParameter, "0x", 2 ) == 0 )
        {
            lValue = kAToI( vcParameter + 2, 16 );
            kPrintf( "HEX Value = %q\n", lValue );
        }
        else
        {
            lValue = kAToI( vcParameter, 10 );
            kPrintf( "Decimal Value = %d\n", lValue );
        }
        
        iCount++;
    }
}

/**
 *  function name : kShutdown
 *  parameters    : pcCommandBuffer(const char*)
 *  return value  : void
 *  brief         : reboot PC
 */
static void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );
    
    kPrintf( "Press Any Key To Reboot PC..." );
    kGetCh();
    kReboot();
}

/**
 *  function name : kSetTimer
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : set counter 0 on PIT controller
 */
static void kSetTimer( const char* pcParameterBuffer )
{
    char            vcParameter[ 100 ];
    PARAMETERLIST   stList;
    long            lValue;
    BOOL            bPeriodic;

    kInitializeParameter( &stList, pcParameterBuffer );
    
    // calculate milesec
    if ( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
        return ;
    }
    lValue = kAToI( vcParameter, 10 );

    // Periodic
    if ( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)settimer 10(ms) 1(periodic)\n" );
        return ;
    }    
    bPeriodic = kAToI( vcParameter, 10 );
    
    kInitializePIT( MSTOCOUNT( lValue ), bPeriodic );
    kPrintf( "Time = %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic );
}

/**
 *  function name : kWaitUsingPIT
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : wait ms second
 */
static void kWaitUsingPIT( const char* pcParameterBuffer )
{
    char            vcParameter[ 100 ];
    int             iLength;
    PARAMETERLIST   stList;
    long            lMillisecond;
    int             i;
    
    kInitializeParameter( &stList, pcParameterBuffer );

    if ( kGetNextParameter( &stList, vcParameter ) == 0 )
    {
        kPrintf( "ex)wait 100(ms)\n" );
        return ;
    }
    
    lMillisecond = kAToI( pcParameterBuffer, 10 );
    kPrintf( "%d ms Sleep Start...\n", lMillisecond );
    
    // interrupt disable
    kDisableInterrupt();

    for ( i = 0 ; i < lMillisecond / 30 ; i++ )
    {
        kWaitUsingDirectPIT( MSTOCOUNT( 30 ) );
    }

    kWaitUsingDirectPIT( MSTOCOUNT( lMillisecond % 30 ) );   
    kEnableInterrupt();
    kPrintf( "%d ms Sleep Complete\n", lMillisecond );
    
    // resotre timer
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );
}

/**
 *  function name : kReadTimeStampCounter
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : read timestamp on counter
 */
static void kReadTimeStampCounter( const char* pcParameterBuffer )
{
    QWORD qwTSC;
    
    qwTSC = kReadTSC();
    kPrintf( "Time Stamp Counter = %q\n", qwTSC );
}

/**
 *  function name : kMeasureProcessorSpeed
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : calculate processor clock
 */
static void kMeasureProcessorSpeed( const char* pcParameterBuffer )
{
    int     i;
    QWORD   qwLastTSC, qwTotalTSC = 0;
        
    kPrintf( "Now Measuring." );
    
    kDisableInterrupt();

    for ( i = 0 ; i < 200 ; i++ )
    {
        qwLastTSC = kReadTSC();
        kWaitUsingDirectPIT( MSTOCOUNT( 50 ) );
        qwTotalTSC += kReadTSC() - qwLastTSC;

        kPrintf( "." );
    }
    
    kInitializePIT( MSTOCOUNT( 1 ), TRUE );    
    kEnableInterrupt();
    
    kPrintf( "\nCPU Speed = %d MHz\n", qwTotalTSC / 10 / 1000 / 1000 );
}

/**
 *  function name : kShowDateAndTime
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : print date and time
 */
static void kShowDateAndTime( const char* pcParameterBuffer )
{
    BYTE    bSecond, bMinute, bHour;
    BYTE    bDayOfWeek, bDayOfMonth, bMonth;
    WORD    wYear;

    kReadRTCTime( &bHour, &bMinute, &bSecond );
    kReadRTCDate( &wYear, &bMonth, &bDayOfMonth, &bDayOfWeek );
    
    kPrintf( "Date: %d/%d/%d %s, ", wYear, bMonth, bDayOfMonth, kConvertDayOfWeekToString( bDayOfWeek ) );
    kPrintf( "Time: %d:%d:%d\n", bHour, bMinute, bSecond );
}

// test task1
static void kTestTask1( void )
{
    BYTE bData;
    int i = 0, iX = 0, iY = 0, iMargin, j;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    
    pstRunningTask = kGetRunningTask();
    iMargin = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) % 10;
    
    for( j = 0 ; j < 20000 ; j++ )
    {
        switch ( i )
        {
        case 0:
            iX++;
            if ( iX >= ( CONSOLE_WIDTH - iMargin ) )
                i = 1;
            break;
            
        case 1:
            iY++;
            if ( iY >= ( CONSOLE_HEIGHT - iMargin ) )
                i = 2;
            break;
            
        case 2:
            iX--;
            if ( iX < iMargin )
                i = 3;
            break;
            
        case 3:
            iY--;
            if ( iY < iMargin )
                i = 0;
            break;
        }
        
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bCharactor = bData;
        pstScreen[ iY * CONSOLE_WIDTH + iX ].bAttribute = bData & 0x0F;
        bData++;
        
        //kSchedule();
    }

    kExitTask();
}

// test task2
static void kTestTask2( void )
{
    int i = 0, iOffset;
    CHARACTER* pstScreen = ( CHARACTER* ) CONSOLE_VIDEOMEMORYADDRESS;
    TCB* pstRunningTask;
    char vcData[ 4 ] = { '-', '\\', '|', '/' };
    
    pstRunningTask = kGetRunningTask();
    iOffset = ( pstRunningTask->stLink.qwID & 0xFFFFFFFF ) * 2;
    iOffset = CONSOLE_WIDTH * CONSOLE_HEIGHT - ( iOffset % ( CONSOLE_WIDTH * CONSOLE_HEIGHT ) );

    while ( 1 )
    {
        pstScreen[ iOffset ].bCharactor = vcData[ i % 4 ];
        pstScreen[ iOffset ].bAttribute = ( iOffset % 15 ) + 1;
        i++;
    
        kSchedule();
    }
}

/**
 *  function name : kCreateTestTask
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : create task
 */
static void kCreateTestTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcType[ 30 ];
    char vcCount[ 30 ];
    int i;
    
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcType );
    kGetNextParameter( &stList, vcCount );

    switch( kAToI( vcType, 10 ) )
    {
    case 1:
        for ( i = 0 ; i < kAToI( vcCount, 10 ) ; i++ )
        {    
            if ( kCreateTask( TASK_FLAGS_LOW, ( QWORD ) kTestTask1 ) == NULL )
                break;
        }
        
        kPrintf( "Task1 %d Created\n", i );
        break;
        
    case 2:
    default:
        for ( i = 0 ; i < kAToI( vcCount, 10 ) ; i++ )
        {    
            if ( kCreateTask( TASK_FLAGS_LOW, ( QWORD ) kTestTask2 ) == NULL )
                break;
        }
        
        kPrintf( "Task2 %d Created\n", i );
        break;
    }    
}   

/**
 *  function name : kChangeTaskPriority
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : change task priority
 */
static void kChangeTaskPriority( const char* pcParameterBuffer )
{
    PARAMETERLIST   stList;
    char            vcID[ 30 ];
    char            vcPriority[ 30 ];
    QWORD           qwID;
    BYTE            bPriority;
    
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    kGetNextParameter( &stList, vcPriority );
    
    if ( kMemCmp( vcID, "0x", 2 ) == 0 )
        qwID = kAToI( vcID + 2, 16 );
    else
        qwID = kAToI( vcID, 10 );
    
    bPriority = kAToI( vcPriority, 10 );
    
    kPrintf( "Change Task Priority ID [0x%q] Priority[%d] ", qwID, bPriority );
    if ( kChangePriority( qwID, bPriority ) == TRUE )
        kPrintf( "Success\n" );
    else
        kPrintf( "Fail\n" );
}

/**
 *  function name : kShowTaskList
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : show tasks
 */
static void kShowTaskList( const char* pcParameterBuffer )
{
    int i;
    TCB* pstTCB;
    int iCount = 0;
    
    kPrintf( "=========== Task Total Count [%d] ===========\n", kGetTaskCount() );
    for ( i = 0 ; i < TASK_MAXCOUNT ; i++ )
    {
        pstTCB = kGetTCBInTCBPool( i );
        if ( ( pstTCB->stLink.qwID >> 32 ) != 0 )
        {
            if ( ( iCount != 0 ) && ( ( iCount % 10 ) == 0 ) )
            {
                kPrintf( "Press any key to continue... ('q' is exit) : " );
                if ( kGetCh() == 'q' )
                {
                    kPrintf( "\n" );
                    break;
                }
                kPrintf( "\n" );
            }
            
            kPrintf( "[%d] Task ID[0x%Q], Priority[%d], Flags[0x%Q]\n", 1 + iCount++, pstTCB->stLink.qwID, GETPRIORITY( pstTCB->qwFlags ), pstTCB->qwFlags);
        }
    }
}

/**
 *  function name : kKillTask
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : remove task
 */
static void kKillTask( const char* pcParameterBuffer )
{
    PARAMETERLIST stList;
    char vcID[ 30 ];
    QWORD qwID;
    
    kInitializeParameter( &stList, pcParameterBuffer );
    kGetNextParameter( &stList, vcID );
    
    if ( kMemCmp( vcID, "0x", 2 ) == 0 )
        qwID = kAToI( vcID + 2, 16 );
    else
        qwID = kAToI( vcID, 10 );
    
    kPrintf( "Kill Task ID [0x%q] ", qwID );
    if ( kEndTask( qwID ) == TRUE )
        kPrintf( "Success\n" );
    else
        kPrintf( "Fail\n" );
}

/**
 *  function name : kCPULoad
 *  parameters    : pcParameterBuffer(const char*)
 *  return value  : void
 *  brief         : show cpu load
 */
static void kCPULoad( const char* pcParameterBuffer )
{
    kPrintf( "Processor Load : %d%%\n", kGetProcessorLoad() );
}