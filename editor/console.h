//----------------------------------------------------------------------------------
//                               CONSOLE.H
//                                 Dvorka
//                                  1997
//----------------------------------------------------------------------------------

#ifndef __CONSOLE_H
 #define __CONSOLE_H

 #include <conio.h>
 #include <dos.h>

 #include "defs.h"


 void KeybSetFastest( void );
 void KeybWait( void );
 void KeybClear( void );
 void beep( void );

 // keyboard port command
 #define KBDCMD(CODE)	             \
	mov al,CODE                  \
	out 0x64,al

 #define KBDCMDDATA(CODE,DATA)	     \
	mov al,CODE                  \
	out 0x64,al                  \
	mov cx, 0x2000               \
  Delay: loop delay                  \
	mov al, DATA                 \
	out 0x64,al


#endif
