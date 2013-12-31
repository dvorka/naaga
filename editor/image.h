#ifndef __IMAGE_H
 #define __IMAGE_H

 #include <alloc.h>
 #include <dos.h>
 #include <fcntl.h>
 #include <io.h>
 #include <values.h>
 #include <stdio.h>
 #include <string.h>

 #include "console.h"
 #include "defs.h"
 #include "dbmp.h"
 #include "dgraphic.h"
 #include "dmouse.h"
 #include "edittool.h"
 #include "gui.h"
 #include "gfx.h"


 #define CONST_BASE       5
 #define LINE             CONST_BASE+1
 #define CIRCLE           CONST_BASE+2
 #define RECTANGLE        CONST_BASE+3
 #define FILL             CONST_BASE+4
 #define CHANGE_WIDTH     CONST_BASE+5
 #define CLEAR_PAPER      CONST_BASE+6
 #define MOVE_MENU        CONST_BASE+7
 #define LOAD             CONST_BASE+8
 #define SAVE             CONST_BASE+9
 #define QSAVE            CONST_BASE+10
 #define CHANGE_COLOR     CONST_BASE+11
 #define ACTION_DONE      CONST_BASE+12


 void ShakerSort( word far *buffer, word size );

 class KrsImage
 {
  public:
   void far* MemAddr;
   char *FileName;
   bool InMem;      // means memory alocated
   word X, Y, Width, Height,
	ByteSize;
   byte WriteMode;

   KrsImage( char *iFileName, word iX, word iY, word iWidth, word iHeight );
   KrsImage( char *iFileName, word iX, word iY, word iByteSize );
    virtual void Load( void );
    virtual void Save( void );
    virtual void Get( void );  // gets specified rectangle from screen to memory
    virtual void Show( void );
    virtual void Show( word x, word y);
    virtual void Free( void ); // frees image from memory
   virtual ~KrsImage();
 };


 class KrsMenu
 {
  // default krs menu file is menu.krs
  public:
   char     *FaceFileName,
	    *UnderFileName,
	    Visible;
   word     MenuByteSize;
   KrsImage *Face,
	    *Under;
   Mouse    *mouse;
   word     X, Y, Width, Height;
   word     CellClipBoard;
   byte     TextCol;

   KrsMenu( Mouse *imouse, word iX, word iY );
   virtual void Move( int ActuallColor ); // is called when movemenu button pushed
   virtual void Hide( void );
   virtual void Show( int ActuallColor );
   int  MouseLeftEar( int x, int y, int &ExitApplication, byte ActuallColor,
		      word direct );
   int  MouseRightEar( int x, int y, byte &ActuallColor );
    // returns FALSE if not else CONSTANT which identifies action
    // which should be done
   void UpdateMousePos( void );
   void UpdateActuallColor( byte color );
   void Message( char *mesg, byte hide=TRUE );
   int ReadString( char *Info, char *String ); // give him string already alocated!
   // returns TRUE if string was entered
   virtual ~KrsMenu();
 };


#endif


