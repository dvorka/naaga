//----------------------------------------------------------------------------------
//			          DDEFS.H
//				   Dvorka
//				 1996-1998
//----------------------------------------------------------------------------------

/*
 Sizes:
  - screen 32*20 level array
  - 5x5 screens
  - in editor 18 rows
  - cell is 10*10 pixels
*/


#ifndef __DDEFS_H
 #define __DDEFS_H

 #define JAILROWS   18

 #define SCREENCOLS 32
 #define SCREENROWS 20

 // level field defines
 #define LEVELSCREENS     5
 #define LEVELHGT         20 * LEVELSCREENS
 #define LEVELWDT         32 * LEVELSCREENS

 #define LEVELSIZEBYTE     64000u //2 * 2 * LEVELHGT * LEVELWDT

 #define TRUE           1
 #define FALSE 		!TRUE

 typedef unsigned char bool;
 typedef unsigned char byte;
 typedef unsigned int  word;
 typedef unsigned long dword;


#endif

 // #define UP    1
 // #define DOWN  2
 // #define LEFT  3
 // #define RIGHT 4
