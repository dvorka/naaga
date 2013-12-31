#include "console.h"

void KeybSetFastest( void )
{
 outportb( 0x64, 0xf3 ); // typematic rate will be set
 outportb( 0x64, 0x00 ); // value
}


void KeybWait( void )
{
 while( kbhit() ); getch();
 getch();
}


void KeybClear( void )
{
 while( kbhit() ) getch();
}


void beep( void )
{
 sound(200);
 delay(300);
 nosound();
}
