//----------------------------------------------------------------------------------
//
//                               DGRAPHIC.CPP
//
//                                  Dvorka
//
//                                1996-1997
//
//----------------------------------------------------------------------------------


#include "dgraphic.h"

//#define VGA_SEG                 0xA000;
//#define VGA_OFF                 0x0000;


byte far *ScreenPtr=(char far*)MK_FP( 0xA000, 0x000 );
byte DGraphicColor    =WHITE;
byte DGraphicFillColor=BLACK;
byte DGraphicWriteMode=COPY_PUT;

#ifdef USE_DGRAPHIC

// PROGRAMMED USING BRUTAL FORCE

//- Dvorka graphics ---------------------------------------------------------------------------------


void InitChain4( void )
{
 asm {
	mov AX, 13h
	int 10h
 }


}


//- Dvorka graphics ---------------------------------------------------------------------------------

void setwritemode( int mode )
{
 DGraphicWriteMode=mode;
}

void setcolor( int color )
{
 DGraphicColor=color;
}

byte getcolor( void )
{
 return DGraphicColor;
}

void setfillcolor( int color )
{
 DGraphicFillColor=color;
}

byte getfillcolor( void )
{
 return DGraphicFillColor;
}

// pseudo functions
void setfillstyle(int pattern, int color)
{
 setfillcolor( color );
}

void settextjustify( int horiz, int vert )
{
 return;
}

//- Dvorka graphics ---------------------------------------------------------------------------------


void closegraph( void )
{
 asm {
	MOV AX, 0003h
	int 10h
     }
}


//- Dvorka graphics ---------------------------------------------------------------------------------

// macro would be better
void putpixel( int x, int y, byte color )
{
 ScreenPtr[y*320+x]=color;
}


byte getpixel( int x, int y )
{
 return ScreenPtr[y*320+x];
}

//- Dvorka graphics ---------------------------------------------------------------------------------

void line     ( int x1, int y1, int x2, int y2 )
// DDA algorithm - not too fast but I am lazy ... ( for all quadrants )
// chybi xor mode => xorovani barev
// getpixel, xor, putpixel
//
{
 int   deltax, deltay, counter, k;
 float xadd, yadd, x, y;

 deltax=x2-x1;
 deltay=y2-y1;

 if( abs(deltax)>abs(deltay) )
  counter=abs(deltax);
 else
  counter=abs(deltay);

 xadd=deltax/counter;
 yadd=deltay/counter;

 x=x1; y=y1;

 ScreenPtr[ceil(y)*320+ceil(x)]=DGraphicColor;

 for( k=1; k<=counter; k++ )
 {
  x+=xadd;
  y+=yadd;

  ScreenPtr[ceil(y)*320+ceil(x)]=DGraphicColor;
 }
}

void rectangle( int left, int top, int right, int bottom )
{
 if(left<0)  	left=0;
 if(right<0) 	right=0;
 if(top<0)  	top=0;
 if(bottom<0) 	bottom=0;
 left%=320;
 right%=320;
 top%=320;
 bottom%=320;

 line( left, top, right, top );
 line( left, bottom, right, bottom );
 line( left, top, left, bottom );
 line( right, top, right, bottom );
}


void bar( int left, int top, int right, int bottom)
{
 int i;

 for( i=top; i<=bottom, i<=319; i++ )
  line( left, i, right, i );
}

void getimage( int left, int top, int right, int bottom, void far *bitmap)
{
 int i, j, row=0, column=0, width=right-left+1;

 for( i=top; i<=bottom, i<=199; i++, row++ )
  for( j=left, column=0; j<=right, j<=319; j++, column++ )
   ((char*)bitmap)[row*width+column]=ScreenPtr[i*320+j];
}


void putimage( int left, int top, int right, int bottom, void far *bitmap, int operation )
{
 int i, j, row=0, column=0, width=right-left+1;

 for( i=top; i<=bottom, i<=199; i++, row++ )
  for( j=left, column=0; j<=right, j<=319; j++, column++ )
   ScreenPtr[i*320+j]=((char*)bitmap)[row*width+column];
}


void outtextxy(int x, int y, char far *textstring)
{
 return;
}

int  imagesize( int left, int top, int right, int bottom )
{
 return (right-left+1)*(bottom-top+1);
}

int  getmaxx( void )
{
 return 319;
}

int  getmaxy( void )
{
 return 199;
}

void cleardevice()
{
 bar( 0, 0, 319, 199 );
}




#else

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


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

 return TRUE;
}


//----------------------------------------------------------------------------------


void DGraphics::Close( void )
{
 asm    MOV        AX, 0x0003
 asm    int        0x10
}


//----------------------------------------------------------------------------------




void DGraphics::PutFrom0x0y( unsigned char far *VirtualScreen, unsigned int Size )
// puts block of bytes from position  0, 0 sequentialy
{
 movedata( FP_SEG(VirtualScreen), FP_OFF(VirtualScreen),
	   0xA000,                0x0000,               Size);
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

#endif