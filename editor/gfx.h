//------------------------------------------------------------------------------
//
//		       Graphics effects library by Dvorka
//
//
//
//------------------------------------------------------------------------------//------------------------------------------------------------------------------//------------------------------------------------------------------------------//------------------------------------------------------------------------------

#ifndef __GFX_H
 #define __GFX_H

 #include <dos.h>
 #include <fcntl.h>

 #include "console.h"
 #include "defs.h"

 void GFXConstructor( void );
 void GFXDestructor( void );
 void GFXInit( void );
 void GFXWaitRetrace( void );
 void GFXGetPalReg( byte ColorNo, byte &R, byte &G, byte &B );
 void GFXSetPalReg( byte ColorNo, byte R, byte G, byte B );
 void GFXFadeOut( int PalWidth=255 );
 void GFXFadeIn( int PalWidth=255 );
 void GFXFadeOut( int PalWidth );
 void GFXPalBlack( void );
 void GFXSetPal( int PalWidth=255 );
 void GFXGetPal( void );

 #define GLASS  0x01


#endif