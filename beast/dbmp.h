//----------------------------------------------------------------------------------
//
//			  DBMP.H - work with Windows BMP
//
//				  Dvorka
//				   1997
//
//----------------------------------------------------------------------------------

#ifndef __DBMP_H
 #define __DBMP_H

 #include <alloc.h>
 #include <conio.h>
 #include <fcntl.h>
 #include <fstream.h>
 #include <io.h>
 #include <dos.h>
 #include <string.h>
 #include <stdio.h>

 #include "console.h"
 #include "defs.h"
 #include "dgraphic.h"


 #define BACKGROUND   1
 #define GAMEMENU     2
 #define CELLIMPORT   3
 #define DBMPGETDATA  5
 #define SHOWBMP      6

 /*
  BMP structure:
   BITMAPFILEHEADER
   BITMAPINFOHEADER
   RGBQUAD[0] ... the first color of pallete ( rgb+unused )
   ...
   RGBQUAD[N] ... last color of pallete ( rgb+unused )
   BITS       ... picture data

   biClrUsed==0: N == 2^biBitCount - 1;
   biClrUsed!=0: N == 2^biClrUsed - 1;
 */

 typedef struct
 {
  int  bfType;       // BM identifies bmp file
  long bfSize;       // size
  int  bfReserved1;  // reserved set to 0
  int  bfReserved2;  // reserved set to 0
  long bfOffBits;    // offset of data from beginning BITMAPINFOHEADER
 } tBMPHeader;

 typedef struct
 {
  long biSize;       	// size of bitmapfileheader
  long biWidth;       	// width of picture
  long biHeight;       	// height of picture
  int  biPlanes;         // set to 1 ( number of planes )
  int  biBitCount;       // number of bits per pixel 1,4,8,24
  long biCompression;    // type of compression 1-BI_RGB,2-BI_RLE8,3-BI_RLE4
  long biSizeImage;      // size of image in bites if BI_RGB=> can be set to 0
  long biXPelsPerMeter;  // horizontal resolution in pixels per meter
  long biYPelsPerMeter;  // vertical resolution in pixels per meter
  long biClrUsed;        // number of colors used in bitmap, if==0 => #==2^BiCount colors
  long biClrImportant;   // number of important colors to draw image
			// if == 0 each color is important
 } tBITMAPINFOHEADER;

 // file must be <= 64k
 int DBMPLoad( char *FileName, byte far *&data, byte what=DBMPGETDATA );

#endif
