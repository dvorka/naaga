//----------------------------------------------------------------------------------
//                               EDITTOOL.H
//                                 Dvorka
//                                  1997
//----------------------------------------------------------------------------------

#ifndef __EDITTOOL_H
 #define __EDITTOOL_H


 #include "defs.h"
 #include "dgraphic.h"
 #include "dmouse.h"
 #include "defs.h"
 #include "gfx.h"
 #include "image.h"


 class Jail
 {
  public:
   int oldx,
       oldy,       // coordinates of active cell
       NoErase,
       XORcolor      // color for XOR
       ;

   Jail( void ) { NoErase=TRUE; XORcolor=255; };
   void Update( int x, int y, int hcolor );
   void Up( void );
   void Down( void );
   void Right( void );
   void Left( void );
   void MoveActiveCell( int direction );
   void ShowActive( void );
   void HideActive( void );

 };

 void InitializeLevelArray( void );
 void ShowLevelCells( void );
 void MoveWindowInLevelArray( int direction, int show=TRUE, int slow=TRUE );
 void SmoothShowLevelCells( int direction );

#endif
