#ifndef __KEYDEFS_H_
#define __KEYDEFS_H_

#include <Arduino.h>

// defines for key codes to normalize them all to 5 chars
#define __1__ KEY_1
#define __2__ KEY_2
#define __3__ KEY_3
#define __4__ KEY_4
#define __5__ KEY_5
#define __6__ KEY_6
#define __7__ KEY_7
#define __8__ KEY_8
#define __9__ KEY_9
#define __0__ KEY_0
#define _TAB_ KEY_TAB
#define __Q__ KEY_Q
#define __W__ KEY_W
#define __E__ KEY_E
#define __R__ KEY_R
#define __T__ KEY_T
#define __Y__ KEY_Y
#define __U__ KEY_U
#define __I__ KEY_I
#define __O__ KEY_O
#define __P__ KEY_P
#define __A__ KEY_A
#define __S__ KEY_S
#define __D__ KEY_D
#define __F__ KEY_F
#define __G__ KEY_G
#define __H__ KEY_H
#define __J__ KEY_J
#define __K__ KEY_K
#define __L__ KEY_L
#define __Z__ KEY_Z
#define __X__ KEY_X
#define __C__ KEY_C
#define __V__ KEY_V
#define __B__ KEY_B
#define __N__ KEY_N
#define __M__ KEY_M
#define L_SFT MODIFIERKEY_SHIFT
#define L_CTL MODIFIERKEY_CTRL
#define L_ALT MODIFIERKEY_ALT
#define L_GUI MODIFIERKEY_GUI
#define R_SFT MODIFIERKEY_RIGHT_SHIFT
#define R_ALT MODIFIERKEY_RIGHT_ALT
#define R_GUI MODIFIERKEY_RIGHT_GUI
#define R_CTL MODIFIERKEY_RIGHT_CTRL
#define _F01_ KEY_F1
#define _F02_ KEY_F2
#define _F03_ KEY_F3
#define _F04_ KEY_F4
#define _F05_ KEY_F5
#define _F06_ KEY_F6
#define _F07_ KEY_F7
#define _F08_ KEY_F8
#define _F09_ KEY_F9
#define _F10_ KEY_F10
#define _F11_ KEY_F11
#define _F12_ KEY_F12
#define TILDE KEY_TILDE
#define MINUS KEY_MINUS
#define EQUAL KEY_EQUAL
#define PG_UP KEY_PAGE_UP
#define PG_DN KEY_PAGE_DOWN
#define _HOME KEY_HOME
#define _END_ KEY_END
#define _TAB_ KEY_TAB
#define L_BRC KEY_LEFT_BRACE
#define R_BRC KEY_RIGHT_BRACE
#define BK_SL KEY_BACKSLASH
#define _ESC_ KEY_ESC
#define SEMIC KEY_SEMICOLON
#define QUOTE KEY_QUOTE
#define ENTER KEY_ENTER
#define COMMA KEY_COMMA
#define PRIOD KEY_PERIOD
#define SLASH KEY_SLASH
#define BK_SP KEY_BACKSPACE
#define SPACE KEY_SPACE
#define _MENU KEY_MENU
#define _UP__ KEY_UP
#define _DOWN KEY_DOWN
#define _LEFT KEY_LEFT
#define RIGHT KEY_RIGHT
#define _INS_ KEY_INSERT
#define _DEL_ KEY_DELETE
#define CAPLK KEY_CAPS_LOCK
#define NUMLK KEY_NUM_LOCK
#define SCRLK KEY_SCROLL_LOCK
#define PRSCR KEY_PRINTSCREEN
#define PAUSE KEY_PAUSE
#define SLEEP KEY_SYSTEM_SLEEP
#define _WAKE KEY_SYSTEM_WAKE_UP
#define _OFF_ KEY_SYSTEM_POWER_DOWN
#define VOLUP KEY_MEDIA_VOLUME_INC
#define VOLDN KEY_MEDIA_VOLUME_DEC
#define _MUTE KEY_MEDIA_MUTE
#define _PLAY KEY_MEDIA_PLAY_PAUSE
#define _PREV KEY_MEDIA_PREV_TRACK
#define _NEXT KEY_MEDIA_NEXT_TRACK
#define _STOP KEY_MEDIA_STOP

// reserved masks:
// 0xE0 - modifiers
// 0xE2 - system
// 0xE4 - media
// 0xF0 - regular

#define MIN_RESERVED_MASK 0xE0
#define FIRST_RESERVED_CODE (MIN_RESERVED_MASK << 8)

// special key codes
#define XXXXX 0x00  // no key - ignore event
#define _____ 0x01  // transparent - look in lower layer for code
#define MBTNL 0x02
#define MBTNM 0x03
#define MBTNR 0x04
#define MBTNB 0x05
#define MBTNF 0x06
#define RAISE 0x07
#define LEDUP 0x08
#define LEDDN 0x09
#define LEDMN 0x0A
#define LEDMX 0x0B
#define LOGUP 0x0C
#define LOGDN 0x0D
#define LOGMX 0x0E
#define LOGMN 0x0F

#endif // __KEYDEFS_H_
