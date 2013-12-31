//----------------------------------------------------------------------------------
//                                CELLS.H
//
//                                 Dvorka
//                               1997-1999
//
//----------------------------------------------------------------------------------

#ifndef __CELLS_H
 #define __CELLS_H


 #include "beast.h"
 #include "defs.h"
 #include "dgraphic.h"
 #include "dmouse.h"
 #include "defs.h"
 #include "gfx.h"

 void InitializeLevelArray( void );
 void ShowLevelCells( void );
 void MoveWindowInLevelArray( int direction, int show=TRUE, int slow=TRUE );

#endif
