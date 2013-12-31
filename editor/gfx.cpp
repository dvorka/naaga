#include "gfx.h"


byte Palette[256][3];
byte FlagPalette[256];


void GFXConstructor( void )
// save original text palette
{
 int  handle;
 word sbytes;

 _dos_creat( "editor.pal", _A_NORMAL, &handle);
//  _dos_write(handle, Palette, 16*3, &sbytes);
  _dos_write(handle, Palette, 256*3, &sbytes);
 _dos_close(handle);
}


void GFXDestructor( void )
// load original text palette
{
 int  handle;
 word sbytes;

 _dos_open( "editor.pal", O_RDONLY, &handle);
//  _dos_read(handle, Palette, 16*3, &sbytes);
  _dos_read(handle, Palette, 256*3, &sbytes);
 _dos_close(handle);

// GFXSetPal( 15 );
 GFXSetPal( 255 );
}


void GFXInit( void )
{
 int i;

 // initilalize Palette
 for( i=0; i<=255; i++ )
 {
  GFXGetPalReg( i, Palette[i][0], Palette[i][1], Palette[i][2] );
  FlagPalette[i]=0;
 }

 FlagPalette[0]=GLASS;
}



void GFXWaitRetrace( void )
// waiting for monitor X-ray, usefull for pallete operations
{
asm		mov DX,0x3DA
	l1:
asm     	in  al,dx
asm     	and al,0x08
asm     	jnz l1
	l2:
asm     	in  al,dx
asm     	and al,0x08
asm     	jz  l2
}


void GFXGetPalReg( byte ColorNo, byte &R, byte &G, byte &B )
{
 outportb(0x3c7, ColorNo );
 R=inport(0x3c9);
 G=inport(0x3c9);
 B=inport(0x3c9);
}


void GFXSetPalReg( byte ColorNo, byte R, byte G, byte B )
{
 asm mov dx, 0x3c8
 asm mov al, ColorNo
 asm out dx, al

 asm mov dx, 0x3c9
 asm mov al, R
 asm out dx, al

 asm mov al, G
 asm out dx, al

 asm mov al, B
 asm out dx, al

 /*
  outportb(0x3c8, ColorNo );
  outportb(0x3c9, R );
  outportb(0x3c9, G );
  outportb(0x3c9, B );
 */
}


void GFXSetPal( int PalWidth )
{
 int i;

 for( i=0; i<=PalWidth; i++ )
  GFXSetPalReg( i, Palette[i][0], Palette[i][1], Palette[i][2] );
}


void GFXGetPal( void )
{
 int i;

 for( i=0; i<=255; i++ )
  GFXGetPalReg( i, Palette[i][0], Palette[i][1], Palette[i][2] );
}


void GFXFadeIn( int PalWidth )
{
 int  finish=TRUE, loop2;
 byte R, G, B;
 byte dif=2;

 while( finish )
 {
  finish=0;
  GFXWaitRetrace(); // waiting for ray

  for( loop2=0; loop2<=PalWidth; loop2++ )
  {
   GFXGetPalReg(loop2,R,G,B);

   if( R<Palette[loop2][0] )
   {
    R+=dif;
    if( R>Palette[loop2][0] ) R=Palette[loop2][0];
    finish++;
   }

   if( G<Palette[loop2][1] )
   {
    G+=dif;
    if( G>Palette[loop2][1] ) G=Palette[loop2][1];
    finish++;
   }

   if( B<Palette[loop2][2] )
   {
    B+=dif;
    if( B>Palette[loop2][2] ) B=Palette[loop2][2];
    finish++;
   }

   GFXSetPalReg((byte)loop2,R,G,B); // set pallette
  }
 }
}


void GFXFadeOut( int PalWidth )
{
 int  change=1, loop2;
 byte R, G, B;
 #define dif 2

 while( change )
 {
  change=0;

  GFXWaitRetrace(); // waiting for ray

  for( loop2=0; loop2<=PalWidth; loop2++ )
  {
   GFXGetPalReg( (byte)loop2, R, G, B );

   if( R>=dif )
    { R-=dif; change++; }
   else
    { if( R ) { R=0; change++; } }

   if( G>=dif )
    { G-=dif; change++; }
   else
    { if( G ) { G=0; change++; } }

   if( B>=dif )
    { B-=dif; change++; }
   else
    { if( B ) { B=0; change++; } }

   GFXSetPalReg( (byte)loop2, R, G, B ); // set pallette
  }

 }

 #undef dif
}


void GFXPalBlack( void )
{
 int i;

 GFXWaitRetrace(); // waiting for ray

 for( i=0; i<=255; i++ ) GFXSetPalReg( i, 0, 0, 0 );
}


