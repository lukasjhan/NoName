/* filename          /Kernel64/Source/Keyboard.c
 * date              2018.11.20
 * last edit date    2018.11.20
 * author            NO.00[UNKNOWN]
 * brief             source code for keyboard input
*/

#include "Types.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"
#include "Queue.h"

/**
 *  function name : kIsOutputBufferFull
 *  return value  : BOOL
 *  brief         : check data in output buffer
 */
BOOL kIsOutputBufferFull( void )
{
    if ( kInPortByte( 0x64 ) & 0x01 )
        return TRUE;
    return FALSE;
}

/**
 *  function name : kIsInputBufferFull
 *  return value  : BOOL
 *  brief         : check data in input buffer
 */
BOOL kIsInputBufferFull( void )
{
    if ( kInPortByte( 0x64 ) & 0x02 )
        return TRUE;
    return FALSE;
}

/**
 *  function name : kWaitForACKAndPutOtherScanCode
 *  return value  : BOOL
 *  brief         : wait for ACK
 */
BOOL kWaitForACKAndPutOtherScanCode( void )
{
    int i, j;
    BYTE bData;
    BOOL bResult = FALSE;

    for ( j = 0 ; j < 100 ; j++ )
    {
        for ( i = 0 ; i < 0xFFFF ; i++ )
        {
            if ( kIsOutputBufferFull() == TRUE )
                break;
        }
        
        bData = kInPortByte( 0x60 );

        if ( bData == 0xFA )
        {
            bResult = TRUE;
            break;
        }
        else
            kConvertScanCodeAndPutQueue( bData );
        
    }
    return bResult;
}

/**
 *  function name : kActivateKeyboard
 *  return value  : BOOL
 *  brief         : activate keyboard device
 */
BOOL kActivateKeyboard( void )
{
    int i, j;
    BOOL bPreviousInterrupt;
    BOOL bResult;

    // disable interrupt
    bPreviousInterrupt = kSetInterruptFlag( FALSE );

    // Activate keyboard device
    kOutPortByte( 0x64, 0xAE );

    // wait until input buffer is empty
    for ( i = 0 ; i < 0xFFFF ; i++ )
    {
        // if input buffer is empty, waiting is over
        if ( kIsInputBufferFull() == FALSE )
            break;
    }
    // 0xF4(keyboard command) is sent
    kOutPortByte( 0x60, 0xF4 );

    // wait for ACK
    bResult = kWaitForACKAndPutOtherScanCode();

    // restore interrupt
    kSetInterruptFlag( bPreviousInterrupt );
    return bResult;
}

/**
 *  function name : kGetKeyboardScanCode
 *  return value  : BYTE
 *  brief         : get data from keyboard
 */
BYTE kGetKeyboardScanCode( void )
{
    // wait until output buffer has data
    while ( kIsOutputBufferFull() == FALSE )
    {
        ; // Waiting
    }
    return kInPortByte( 0x60 );
}

/**
 *  function name : kChangeKeyboardLED
 *  return value  : BOOL
 *  parameters    : bCapsLockOn(BOOL)
 *                  bNumLockOn(BOOL)
 *                  bScrollLockOn(BOOL)
 *  brief         : change keyboard LED status
 */
BOOL kChangeKeyboardLED( BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn )
{
    int i, j;
    BOOL bPreviousInterrupt;
    BOOL bResult;
    BYTE bData;

    bPreviousInterrupt = kSetInterruptFlag( FALSE );

    // wait until output buffer is empty
    for ( i = 0 ; i < 0xFFFF ; i++ )
    {
        if ( kIsInputBufferFull() == FALSE )
            break;
    }

    kOutPortByte( 0x60, 0xED );

    // wait until keyboard take message
    for ( i = 0 ; i < 0xFFFF ; i++ )
    {
        if( kIsInputBufferFull() == FALSE )
            break;
    }

    bResult = kWaitForACKAndPutOtherScanCode();

    if ( bResult == FALSE )
    {
        kSetInterruptFlag( bPreviousInterrupt );
        return FALSE;
    }

    // send LED change message to keyboard
    kOutPortByte( 0x60, ( bCapsLockOn << 2 ) | ( bNumLockOn << 1 ) | bScrollLockOn );
    for ( i = 0 ; i < 0xFFFF ; i++ )
    {
        if ( kIsInputBufferFull() == FALSE )
            break;
    }

    bResult = kWaitForACKAndPutOtherScanCode();

    kSetInterruptFlag( bPreviousInterrupt );
    return bResult;
}

/**
 *  function name : kEnableA20Gate
 *  return value  : void
 *  parameters    : void
 *  brief         : Enable A20 gate
 */
void kEnableA20Gate( void )
{
    BYTE bOutputPortData;
    int i;

    kOutPortByte( 0x64, 0xD0 );

    for ( i = 0 ; i < 0xFFFF ; i++ )
    {
        if ( kIsOutputBufferFull() == TRUE )
            break;
    }

    bOutputPortData = kInPortByte( 0x60 );

    // A20 gate bit setting
    bOutputPortData |= 0x01;

    for ( i = 0 ; i < 0xFFFF ; i++ )
    {
        if ( kIsInputBufferFull() == FALSE )
            break;
    }

    kOutPortByte( 0x64, 0xD1 );
    kOutPortByte( 0x60, bOutputPortData );
}

/**
 *  function name : kReboot
 *  return value  : void
 *  parameters    : void
 *  brief         : reset CPU
 */
void kReboot( void )
{
    int i;

    for( i = 0 ; i < 0xFFFF ; i++ )
    {
        if( kIsInputBufferFull() == FALSE )
            break;
    }

    kOutPortByte( 0x64, 0xD1 );

    // send 0 to input buffer to reset CPU
    kOutPortByte( 0x60, 0x00 );

    while( 1 )
    {
        ; // DO NOTHING
    }
}

// KEYBOARD SCAN CODE TO ASCII CODE

// keyboard status manager
static KEYBOARDMANAGER gs_stKeyboardManager = { 0, };

// Queue for key
static QUEUE gs_stKeyQueue;
static KEYDATA gs_vstKeyQueueBuffer[ KEY_MAXQUEUECOUNT ];

// scan code to ascii code table
static KEYMAPPINGENTRY gs_vstKeyMappingTable[ KEY_MAPPINGTABLEMAXCOUNT ] =
{
    /*  0   */  {   KEY_NONE        ,   KEY_NONE        },
    /*  1   */  {   KEY_ESC         ,   KEY_ESC         },
    /*  2   */  {   '1'             ,   '!'             },
    /*  3   */  {   '2'             ,   '@'             },
    /*  4   */  {   '3'             ,   '#'             },
    /*  5   */  {   '4'             ,   '$'             },
    /*  6   */  {   '5'             ,   '%'             },
    /*  7   */  {   '6'             ,   '^'             },
    /*  8   */  {   '7'             ,   '&'             },
    /*  9   */  {   '8'             ,   '*'             },
    /*  10  */  {   '9'             ,   '('             },
    /*  11  */  {   '0'             ,   ')'             },
    /*  12  */  {   '-'             ,   '_'             },
    /*  13  */  {   '='             ,   '+'             },
    /*  14  */  {   KEY_BACKSPACE   ,   KEY_BACKSPACE   },
    /*  15  */  {   KEY_TAB         ,   KEY_TAB         },
    /*  16  */  {   'q'             ,   'Q'             },
    /*  17  */  {   'w'             ,   'W'             },
    /*  18  */  {   'e'             ,   'E'             },
    /*  19  */  {   'r'             ,   'R'             },
    /*  20  */  {   't'             ,   'T'             },
    /*  21  */  {   'y'             ,   'Y'             },
    /*  22  */  {   'u'             ,   'U'             },
    /*  23  */  {   'i'             ,   'I'             },
    /*  24  */  {   'o'             ,   'O'             },
    /*  25  */  {   'p'             ,   'P'             },
    /*  26  */  {   '['             ,   '{'             },
    /*  27  */  {   ']'             ,   '}'             },
    /*  28  */  {   '\n'            ,   '\n'            },
    /*  29  */  {   KEY_CTRL        ,   KEY_CTRL        },
    /*  30  */  {   'a'             ,   'A'             },
    /*  31  */  {   's'             ,   'S'             },
    /*  32  */  {   'd'             ,   'D'             },
    /*  33  */  {   'f'             ,   'F'             },
    /*  34  */  {   'g'             ,   'G'             },
    /*  35  */  {   'h'             ,   'H'             },
    /*  36  */  {   'j'             ,   'J'             },
    /*  37  */  {   'k'             ,   'K'             },
    /*  38  */  {   'l'             ,   'L'             },
    /*  39  */  {   ';'             ,   ':'             },
    /*  40  */  {   '\''            ,   '\"'            },
    /*  41  */  {   '`'             ,   '~'             },
    /*  42  */  {   KEY_LSHIFT      ,   KEY_LSHIFT      },
    /*  43  */  {   '\\'            ,   '|'             },
    /*  44  */  {   'z'             ,   'Z'             },
    /*  45  */  {   'x'             ,   'X'             },
    /*  46  */  {   'c'             ,   'C'             },
    /*  47  */  {   'v'             ,   'V'             },
    /*  48  */  {   'b'             ,   'B'             },
    /*  49  */  {   'n'             ,   'N'             },
    /*  50  */  {   'm'             ,   'M'             },
    /*  51  */  {   ','             ,   '<'             },
    /*  52  */  {   '.'             ,   '>'             },
    /*  53  */  {   '/'             ,   '?'             },
    /*  54  */  {   KEY_RSHIFT      ,   KEY_RSHIFT      },
    /*  55  */  {   '*'             ,   '*'             },
    /*  56  */  {   KEY_LALT        ,   KEY_LALT        },
    /*  57  */  {   ' '             ,   ' '             },
    /*  58  */  {   KEY_CAPSLOCK    ,   KEY_CAPSLOCK    },
    /*  59  */  {   KEY_F1          ,   KEY_F1          },
    /*  60  */  {   KEY_F2          ,   KEY_F2          },
    /*  61  */  {   KEY_F3          ,   KEY_F3          },
    /*  62  */  {   KEY_F4          ,   KEY_F4          },
    /*  63  */  {   KEY_F5          ,   KEY_F5          },
    /*  64  */  {   KEY_F6          ,   KEY_F6          },
    /*  65  */  {   KEY_F7          ,   KEY_F7          },
    /*  66  */  {   KEY_F8          ,   KEY_F8          },
    /*  67  */  {   KEY_F9          ,   KEY_F9          },
    /*  68  */  {   KEY_F10         ,   KEY_F10         },
    /*  69  */  {   KEY_NUMLOCK     ,   KEY_NUMLOCK     },
    /*  70  */  {   KEY_SCROLLLOCK  ,   KEY_SCROLLLOCK  },

    /*  71  */  {   KEY_HOME        ,   '7'             },
    /*  72  */  {   KEY_UP          ,   '8'             },
    /*  73  */  {   KEY_PAGEUP      ,   '9'             },
    /*  74  */  {   '-'             ,   '-'             },
    /*  75  */  {   KEY_LEFT        ,   '4'             },
    /*  76  */  {   KEY_CENTER      ,   '5'             },
    /*  77  */  {   KEY_RIGHT       ,   '6'             },
    /*  78  */  {   '+'             ,   '+'             },
    /*  79  */  {   KEY_END         ,   '1'             },
    /*  80  */  {   KEY_DOWN        ,   '2'             },
    /*  81  */  {   KEY_PAGEDOWN    ,   '3'             },
    /*  82  */  {   KEY_INS         ,   '0'             },
    /*  83  */  {   KEY_DEL         ,   '.'             },
    /*  84  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  85  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  86  */  {   KEY_NONE        ,   KEY_NONE        },
    /*  87  */  {   KEY_F11         ,   KEY_F11         },
    /*  88  */  {   KEY_F12         ,   KEY_F12         }
};

/**
 *  function name : kIsAlphabetScanCode
 *  return value  : BOOL
 *  parameters    : bScanCode(BYTE)
 *  brief         : check input scan code is alphabet
 */
BOOL kIsAlphabetScanCode( BYTE bScanCode )
{
    if ( ( 'a' <= gs_vstKeyMappingTable[ bScanCode ].bNormalCode ) && ( gs_vstKeyMappingTable[ bScanCode ].bNormalCode <= 'z' ) )
        return TRUE;
    
    return FALSE;
}

/**
 *  function name : kIsNumberOrSymbolScanCode
 *  return value  : BOOL
 *  parameters    : bScanCode(BYTE)
 *  brief         : check input scan code is number of symbol
 */
BOOL kIsNumberOrSymbolScanCode( BYTE bScanCode )
{
    if ( ( 2 <= bScanCode ) && ( bScanCode <= 53 ) && ( kIsAlphabetScanCode( bScanCode ) == FALSE ) )
        return TRUE;

    return FALSE;
}

/**
 *  function name : kIsNumberPadScanCode
 *  return value  : BOOL
 *  parameters    : bScanCode(BYTE)
 *  brief         : check input scan code is number pad
 */
BOOL kIsNumberPadScanCode( BYTE bScanCode )
{
    // numberpad scan code is 71 ~ 83
    if ( ( 71 <= bScanCode ) && ( bScanCode <= 83 ) )
        return TRUE;
    
    return FALSE;
}

/**
 *  function name : kIsUseCombinedCode
 *  return value  : BOOL
 *  parameters    : bScanCode(BYTE)
 *  brief         : check input scan code is combined key
 */
BOOL kIsUseCombinedCode( BYTE bScanCode )
{
    BYTE bDownScanCode;
    BOOL bUseCombinedKey;

    bDownScanCode = bScanCode & 0x7F;

    // shift & capslock affect alphabet
    if ( kIsAlphabetScanCode( bDownScanCode ) == TRUE )
    {
        if ( gs_stKeyboardManager.bShiftDown ^ gs_stKeyboardManager.bCapsLockOn )
            bUseCombinedKey = TRUE;
        else
            bUseCombinedKey = FALSE;
    }
    // shift affect number or symbol
    else if ( kIsNumberOrSymbolScanCode( bDownScanCode ) == TRUE )
    {
        if ( gs_stKeyboardManager.bShiftDown == TRUE )
            bUseCombinedKey = TRUE;
        else
            bUseCombinedKey = FALSE;
    }
    // num lock affect numberpad key
    // except 0xE0, extended key is same with numberpad combined key
    // without extended key code, use combined key
    else if ( ( kIsNumberPadScanCode( bDownScanCode ) == TRUE ) && ( gs_stKeyboardManager.bExtendedCodeIn == FALSE ) )
    {
        if ( gs_stKeyboardManager.bNumLockOn == TRUE )
            bUseCombinedKey = TRUE;
        else
            bUseCombinedKey = FALSE;
    }

    return bUseCombinedKey;
}

/**
 *  function name : UpdateCombinationKeyStatusAndLED
 *  return value  : void
 *  parameters    : bScanCode(BYTE)
 *  brief         : update combined key status
 */
void UpdateCombinationKeyStatusAndLED( BYTE bScanCode )
{
    BOOL bDown;
    BYTE bDownScanCode;
    BOOL bLEDStatusChanged = FALSE;

    if ( bScanCode & 0x80 )
    {
        bDown = FALSE;
        bDownScanCode = bScanCode & 0x7F;
    }
    else
    {
        bDown = TRUE;
        bDownScanCode = bScanCode;
    }

    // check shift key
    if ( ( bDownScanCode == 42 ) || ( bDownScanCode == 54 ) )
    {
        gs_stKeyboardManager.bShiftDown = bDown;
    }
    // check capslock key
    else if ( ( bDownScanCode == 58 ) && ( bDown == TRUE ) )
    {
        gs_stKeyboardManager.bCapsLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    // check numberlock key
    else if ( ( bDownScanCode == 69 ) && ( bDown == TRUE ) )
    {
        gs_stKeyboardManager.bNumLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    // check scrolllock key
    else if ( ( bDownScanCode == 70 ) && ( bDown == TRUE ) )
    {
        gs_stKeyboardManager.bScrollLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }

    // change LED
    if ( bLEDStatusChanged == TRUE )
        kChangeKeyboardLED( gs_stKeyboardManager.bCapsLockOn, gs_stKeyboardManager.bNumLockOn, gs_stKeyboardManager.bScrollLockOn );
}

/**
 *  function name : kConvertScanCodeToASCIICode
 *  return value  : BOOL                - check the input print or not
 *  parameters    : bScanCode(BYTE)     - input scan code
 *                  pbASCIICode(BYTE*)  - ascii code memory address
 *                  pbFlags(BYTE*)      - extended code or combined flags
 *  brief         : convert scan code to ascii code
 */
BOOL kConvertScanCodeToASCIICode( BYTE bScanCode, BYTE* pbASCIICode, BOOL* pbFlags )
{
    BOOL bUseCombinedKey;

    // pause will ignore keys
    if ( gs_stKeyboardManager.iSkipCountForPause > 0 )
    {
        gs_stKeyboardManager.iSkipCountForPause--;
        return FALSE;
    }

    // Pause key
    if ( bScanCode == 0xE1 )
    {
        *pbASCIICode = KEY_PAUSE;
        *pbFlags = KEY_FLAGS_DOWN;
        gs_stKeyboardManager.iSkipCountForPause = KEY_SKIPCOUNTFORPAUSE;
        return TRUE;
    }
    // extended key flag setting
    else if ( bScanCode == 0xE0 )
    {
        gs_stKeyboardManager.bExtendedCodeIn = TRUE;
        return FALSE;
    }

    // check combined key
    bUseCombinedKey = kIsUseCombinedCode( bScanCode );

    if ( bUseCombinedKey == TRUE )
        *pbASCIICode = gs_vstKeyMappingTable[ bScanCode & 0x7F ].bCombinedCode;
    else
        *pbASCIICode = gs_vstKeyMappingTable[ bScanCode & 0x7F ].bNormalCode;
    

    // check extended key
    if ( gs_stKeyboardManager.bExtendedCodeIn == TRUE )
    {
        *pbFlags = KEY_FLAGS_EXTENDEDKEY;
        gs_stKeyboardManager.bExtendedCodeIn = FALSE;
    }
    else
        *pbFlags = 0;
    
    // check press
    if ( ( bScanCode & 0x80 ) == 0 )
        *pbFlags |= KEY_FLAGS_DOWN;
    
    UpdateCombinationKeyStatusAndLED( bScanCode );
    return TRUE;
}

/**
 *  function name : kInitializeKeyboard
 *  return value  : BOOL
 *  brief         : init keyboard
 */
BOOL kInitializeKeyboard( void )
{
    // init Queue
    kInitializeQueue( &gs_stKeyQueue, gs_vstKeyQueueBuffer, KEY_MAXQUEUECOUNT, sizeof( KEYDATA ) );
    return kActivateKeyboard();
}

/**
 *  function name : kConvertScanCodeAndPutQueue
 *  return value  : BOOL
 *  parameter     : bScanCode(BYTE)
 *  brief         : convert scan code to Queue
 */
BOOL kConvertScanCodeAndPutQueue( BYTE bScanCode )
{
    KEYDATA stData;
    BOOL bResult = FALSE;
    BOOL bPreviousInterrupt;

    stData.bScanCode = bScanCode;
    
    if ( kConvertScanCodeToASCIICode( bScanCode, &( stData.bASCIICode ), &( stData.bFlags ) ) == TRUE )
    {
        // disable interrupt
        bPreviousInterrupt = kSetInterruptFlag( FALSE );
        bResult            = kPutQueue( &gs_stKeyQueue, &stData );
        // restore interrupt
        kSetInterruptFlag( bPreviousInterrupt );
    }
    return bResult;
}

/**
 *  function name : kGetKeyFromKeyQueue
 *  return value  : BOOL
 *  brief         : get key from Queue
 */
BOOL kGetKeyFromKeyQueue( KEYDATA* pstData )
{
    BOOL bResult;
    BOOL bPreviousInterrupt;

    // 큐가 비었으면 키 데이터를 꺼낼 수 없음
    if( kIsQueueEmpty( &gs_stKeyQueue ) == TRUE )
        return FALSE;
    
    // disable interrupt
    bPreviousInterrupt = kSetInterruptFlag( FALSE );
    // remove key from Queue
    bResult            = kGetQueue( &gs_stKeyQueue, pstData );
    // restore interrupt
    kSetInterruptFlag( bPreviousInterrupt );
    return bResult;
}