//----------------------------------------------------------------------------------
//
//                               Keyb & comp.
//
//                                 Dvorka
//                                  1997
//
//----------------------------------------------------------------------------------

#ifndef __CONSOLE_H
 #define __CONSOLE_H

 #include <stdio.h>
 #include <conio.h>
 #include <dos.h>

 #include "defs.h"

 enum keyBoardKeys
 {
  KEYB_UP        =72,
  KEYB_DOWN      =80,
  KEYB_LEFT      =75,
  KEYB_RIGHT     =77,

  KEYB_LSHIFT	=42,
  KEYB_RSHIFT    =54,
  KEYB_LALT      =56,
  KEYB_RALT      =56,
  KEYB_LCTRL     =29,
  KEYB_RCTRL     =29
 };

 void ConsoleConstructor( void );
 void ConsoleDestructor( void );
 void KeybSetFastest( void );
 void KeybWait( void );
 void KeybClear( void );
 void beep( void );

#endif
