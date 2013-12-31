//----------------------------------------------------------------------------------
//
//                               DGRAPHIC.H
//
//                                  Dvorka
//                                1996-1998
//
//		     Special clone for Naaga's editor
//
//----------------------------------------------------------------------------------

#ifndef __DGRAPHIC_H
 #define __DGRAPHIC_H

// #define USE_DGRAPHIC

 #include <dos.h>
 #include <conio.h>
 #include <fstream.h>

  #ifndef USE_DGRAPHIC
   #include <graphics.h>
  #endif

 #include <math.h>
 #include <mem.h>

 #include "defs.h"

 #ifdef USE_DGRAPHIC
  //  Doesn't use bgi => standart VGA modes ( TECH )

  // colors
  #define BLACK		0
  #define BLUE          1
  #define GREEN         2
  #define CYAN          3
  #define RED           4
  #define MAGENTA       5
  #define BROWN         6
  #define LIGHTGRAY     7
  #define DARKGRAY      8
  #define LIGHTBLUE     9
  #define LIGHTGREEN    10
  #define LIGHTCYAN     11
  #define LIGHTRED      12
  #define LIGHTMAGENTA  13
  #define YELLOW        14
  #define WHITE         15

  #define EMPTY_FILL    0
  #define SOLID_FILL    1

  #define LEFT_TEXT 	0
  #define CENTER_TEXT   1
  #define RIGHT_TEXT    2
  #define TOP_TEXT      2

  // constants for putimage and setwritemode
  #define COPY_PUT  0
  #define XOR_PUT   1
  #define OR_PUT    2
  #define AND_PUT   3
  #define NOT_PUT   4

  // functions
  void InitChain4( void );

   void setwritemode( int mode );
   void setcolor( int color );
   byte getcolor( void );
   void setfillcolor( int color );
   byte getfillcolor( void );

   void putpixel( int x, int y, byte color );
   byte getpixel( int x, int y );

   void line     ( int x1, int y1, int x2, int y2 );
   void rectangle( int left, int top, int right, int bottom );
   void bar      ( int left, int top, int right, int bottom);

   void getimage( int left, int top, int right, int bottom, void far *bitmap);
   void putimage( int left, int top, int right, int bottom, void far *bitmap, int operation );

   void outtextxy(int x, int y, char far *textstring);

   // pseudo
   void setfillstyle(int pattern, int color);
   int  imagesize( int left, int top, int right, int bottom );
   int  getmaxx( void );
   int  getmaxy( void );
   void cleardevice( void );
   void settextjustify( int horiz, int vert );

  void closegraph();

 #else

  // - Make sure that options/linker/libraries/graphic_library is set

  #define DGRAPH_SCREEN_MEM_SIZE         64000u


  class DGraphics
  {
   private:

   public:
    enum _GraphModes { SVGA320x200x256 = 0, SVGA640x400x256 = 1,
		      SVGA800x600x256 = 3, SVGA1024x768x256 = 5 };

    int  Init( int gmode=SVGA320x200x256 );
    void Close( void );

    void PutFrom0x0y( unsigned char far *VirtualScreen,
		     unsigned int Size=DGRAPH_SCREEN_MEM_SIZE );
    void ANDPutImage( unsigned char far *VirtualScreen,
		  int x=0, int y=0, int width=640, int height=200 );
    void ORPutImage( unsigned char far *VirtualScreen, int ignoredColor,
		  int x=0, int y=0, int width=640, int height=200 );
  };


 #endif

#endif
