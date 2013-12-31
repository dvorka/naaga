//----------------------------------------------------------------------------------
//			          DMOUSE.H
//				   Dvorka
//				  1996-1997
//----------------------------------------------------------------------------------

#ifndef __DMOUSE_H
 #define __DMOUSE_H

 #include <conio.h>
 #include <dos.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <iostream.h>

 #include "defs.h"
 #include "dgraphic.h"
 #include "gui.h"


 typedef unsigned char byte;


 enum MouseState  { DEAD, LEFT, RIGHT, UP, DOWN };
 enum MouseAction { LBUTTON, MBUTTON, RBUTTON, LRBUTTON };

 class Mouse
  {
   public:
    enum   { CROSS, ARROW, PEN };
    union  REGS registr;
    char   StringX[20],
	   StringY[20];

    int    OldX,OldY,
	   ButtonNumb,
	   diference,
	   color,
	   ex, ey,
	   Visible,         // TRUE if mouse shown
	   Emulate,         // TRUE if mouse pointer emulated
	   EWriteMode;      // write mode which is set after drawing pointer

   public:

    Mouse( int iButtonNumb = 2 );
    char Init( int idiference = 2, int emulate = FALSE );
    void Show( void );
    void Hide( void );
    int  LeftEar( void );
    int  Nose( void );
    int  RightEar( void );
    void Pos ( int &x,int &y );
    int  PosX ( void );
    int  PosY ( void );
    void Move ( int x = 0, int y = 0 );
    void SpaceX ( int xmin = 0, int xmax = 639 );
    void SpaceY ( int ymin = 0, int ymax = 379);
    void Break( int lengt = 200 );
    void Wait( void );
    void PosGraph ( int ux, int uy, int fcol = BLACK, int bcol = WHITE );

    // emulate methods
    void EUpdate( void );

    void SetCursor( byte ID );
  };

#endif