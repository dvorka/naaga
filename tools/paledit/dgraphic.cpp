//----------------------------------------------------------------------------------
//			         DGRAPHIC.CPP
//				   Dvorka
//				  1996-1997
//----------------------------------------------------------------------------------

#include <math.h>

#include "dgraphic.h"

#define VGA_SEG  		0xA000;
#define VGA_OFF  		0x0000;

int DGraphics::Init( int gmode )
{
 int gdriver = VGA,
     errorcode;

 gdriver=installuserdriver("SVGA256",NULL);
 initgraph(&gdriver, &gmode, "");

 if ( (errorcode = graphresult()) != grOk )
  {
    cout << "Error: Graphics - %s\n" << grapherrormsg(errorcode);
    return FALSE;
  }

 ActiveMode=gmode;

 return TRUE;
}


void DGraphics::PutFrom0x0y( unsigned char far *VirtualScreen, unsigned int Size )
{
 movedata( FP_SEG(VirtualScreen), FP_OFF(VirtualScreen),
	   0xA000,  		  0x0000, 		Size);
}


void DGraphics::ANDPutImage( unsigned char far *VirtualScreen,
			     int x, int y, int width, int height )
{
 char far* Screen=(char far*)MK_FP( 0xA000, 0x000 );
 int SrcOff=0,
     DstOff=y*320+x, // x is counted from 0
     k,
     j;

 for(j=0;j<height;j++)
 {
  for( k=0; k<width; k++ ) Screen[DstOff+k]=VirtualScreen[SrcOff+k];
  //memmove(Screen+DstOff,VirtualScreen+SrcOff,width);
  DstOff+=320; SrcOff+=width;
 }
}


void DGraphics::ORPutImage( unsigned char far *VirtualScreen, int IgnoredColor,
			     int x, int y, int width, int height )
{
 char far* Screen=(char far*)MK_FP( 0xA000, 0x000 );
 int SrcOff=0,
     DstOff=y*320+x, // x is counted from 0
     j,k;

 for(j=0;j<height;j++)
 {
   for( k=0; k<width; k++ )
    {
     if( VirtualScreen[SrcOff+k]!=IgnoredColor )
      Screen[DstOff+k]=VirtualScreen[SrcOff+k];
    }
   DstOff+=320; SrcOff+=width;
 }
}


void DGraphics::Close( void )
{
 closegraph();
}


