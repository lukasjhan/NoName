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

SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
        { "help", "Show Help", kHelp },
        { "cls", "Clear Screen", kCls },
        { "totalram", "Show Total RAM Size", kShowTotalRAMSize },
        { "strtod", "String To Decial/Hex Convert", kStringToDecimalHexTest },
        { "shutdown", "Shutdown And Reboot OS", kShutdown },
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
void kHelp( const char* pcCommandBuffer )
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
void kCls( const char* pcParameterBuffer )
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
void kShowTotalRAMSize( const char* pcParameterBuffer )
{
    kPrintf( "Total RAM Size = %d MB\n", kGetTotalRAMSize() );
}

/**
 *  function name : kStringToDecimalHexTest
 *  parameters    : pcCommandBuffer(const char*)
 *  return value  : void
 *  brief         : convert text to number
 */
void kStringToDecimalHexTest( const char* pcParameterBuffer )
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
void kShutdown( const char* pcParamegerBuffer )
{
    kPrintf( "System Shutdown Start...\n" );
    
    kPrintf( "Press Any Key To Reboot PC..." );
    kGetCh();
    kReboot();
}