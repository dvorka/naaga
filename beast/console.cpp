//------------------------------------------------------------------------------
//
//		                Keyb & comp.
//
//                                 Dvorka
//                                  1997
//
//------------------------------------------------------------------------------//------------------------------------------------------------------------------//------------------------------------------------------------------------------//------------------------------------------------------------------------------

#include "console.h"

//----------------------------------------------------------------------------

// variable arg lng
#ifdef __cplusplus
    #define __CPPARGS ...
#else
    #define __CPPARGS
#endif

//----------------------------------------------------------------------------

#define INTR_KEYB	0x09

//----------------------------------------------------------------------------

byte keyBoard[128]; // array of keys. If byte is 1, key is down
			   // else, key is up

//----------------------------------------------------------------------------

// declaring variable oldKeybHandler, will be used for storing old handler vec
void interrupt ( *oldKeybHandler)(__CPPARGS);

//----------------------------------------------------------------------------

byte key;	// variable for interrupt handler

// my handler of keyboard
void interrupt naagaKeybHandler(__CPPARGS)
{
 // data must be declared outside

 // read key from port
 asm in  al,0x60
 asm mov key,al

 if( key<128 ) 		// key down
  keyBoard[key]=1;
 else                   // key up
  keyBoard[key-128]=0;

 // call the old routine
 oldKeybHandler();
}

//----------------------------------------------------------------------------

void ConsoleConstructor( void )
// installs handler of keyboard and resets key array
{
 int i;
 for(i=0;i<128;i++) keyBoard[i]=0;

 // save the old interrupt vector for int 0x09
 oldKeybHandler = getvect(INTR_KEYB);

 // install the new interrupt handler
 setvect(INTR_KEYB, naagaKeybHandler);
}

//----------------------------------------------------------------------------

void ConsoleDestructor( void )
// reset the old interrupt handler
{
 setvect(INTR_KEYB, oldKeybHandler);
}

//----------------------------------------------------------------------------

void KeybSetFastest( void )
{
 outportb( 0x64, 0xf3 ); // typematic rate will be set
 outportb( 0x64, 0x00 ); // value
}

//----------------------------------------------------------------------------

void KeybWait( void )
{
 while( kbhit() ); getch();
 getch();
}

//----------------------------------------------------------------------------

void KeybClear( void )
{
 while( kbhit() ) getch();
}

//----------------------------------------------------------------------------

void beep( void )
{
 sound(200);
 delay(300);
 nosound();
}

//- EOF ----------------------------------------------------------------------
