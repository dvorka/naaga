//----------------------------------------------------------------------------------
//			         DGRAPHIC.H
//				   Dvorka
//				    1996-1997
//----------------------------------------------------------------------------------

// - Make sure that options/linker/libraries/graphic_library is set
// - For using this library is necessary SVGA256.BGI by Jordan Hargraphix

#ifndef __DGRAPHIC_H
 #define __DGRAPHIC_H

 #include <dos.h>
 #include <conio.h>
 #include <fstream.h>
 #include <graphics.h>
 #include <mem.h>

 #include "ddefs.h"

 #define DGRAPH_SCREEN_MEM_SIZE 	64000

 class DGraphics
 {
  public:
   enum _GraphModes { SVGA320x200x256 = 0, SVGA640x400x256 = 1,
		      SVGA800x600x256 = 3, SVGA1024x768x256 = 5 };
   int ActiveMode;

   int  Init( int gmode=SVGA320x200x256 );
   void Close( void );
   // puts block of bytes from position  0, 0 sequentialy
   void PutFrom0x0y( unsigned char far *VirtualScreen,
		     unsigned int Size=DGRAPH_SCREEN_MEM_SIZE );
   void ANDPutImage( unsigned char far *VirtualScreen,
		  int x=0, int y=0, int width=640, int height=200 );
   void ORPutImage( unsigned char far *VirtualScreen, int ignoredColor,
		  int x=0, int y=0, int width=640, int height=200 );
 };

#endif
