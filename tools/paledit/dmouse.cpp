//----------------------------------------------------------------------------------
//			     Methods of DMOUSE.H
//				   Dvorka
//				    1996
//----------------------------------------------------------------------------------
#include "dmouse.h"

#define DRAW_OLD_POINTER( EXIT_MODE )			\
  color=getcolor();					  \
  setcolor(LIGHTGREEN); setwritemode( XOR_PUT );        \
  line( eox, eoy, eox+3, eoy+3 );                           \
  line( eox, eoy+1, eox, eoy+2 );                           \
  line( eox+1, eoy, eox+2, eoy );                           \
  setwritemode( EXIT_MODE ); setcolor(color);

#define DRAW_NEW_POINTER( EXIT_MODE )			\
  color=getcolor();					  \
  setcolor(LIGHTGREEN); setwritemode( XOR_PUT );        \
  line( ex, ey, ex+3, ey+3 );                               \
  line( ex, ey+1, ex, ey+2 );                               \
  line( ex+1, ey, ex+2, ey );                               \
  eox=ex; eoy=ey;						\
  setwritemode( EXIT_MODE ); setcolor( color );

  static eox, eoy;

Mouse::Mouse( int iButtonNumb ) { ButtonNumb = iButtonNumb; };

char Mouse::Init( int idiference, int emulate  ) // returns diference
{
 registr.x.ax = 0x00; int86 (0x33,&registr,&registr);

 if (registr.x.ax == 0x00)
  {
    cout << "\n\r Hardware or Software for mouse is not installed.";
    return FALSE;
  }

 if ( registr.x.bx != ButtonNumb )
  {
   if( ButtonNumb == 2 )
    { cout << "\n\r Please use mouse with 3 buttons."; return FALSE; }
   else
    { cout << "\n\r Please use mouse with 2 buttons."; return FALSE; }
  }

 // decide x = x or x = 2x, here initialized by user
 diference=idiference;

 EWriteMode=COPY_PUT;
 Emulate=emulate;
 Visible=FALSE;

 return diference;
}


void Mouse::Show( void )
{
 if( !Visible )
 {
  if( Emulate )
  {
   DRAW_NEW_POINTER( EWriteMode )
  }
  else
   registr.x.ax = 0x01; int86(0x33,&registr,&registr);

 Visible=TRUE;
 }
}


void Mouse::Hide( void )
{
 if( Visible )
 {
  if( Emulate )
  {
   DRAW_OLD_POINTER( EWriteMode )
  }
  else
   registr.x.ax = 0x02; int86(0x33,&registr,&registr);
  Visible=FALSE;
 }
}


int Mouse::LeftEar( void )
{
 registr.x.ax = 0x03; int86(0x33,&registr,&registr);
 return registr.x.bx & 1;
}

int Mouse::Nose( void )
{
 return FALSE;
}

int Mouse::RightEar( void )
{
 registr.x.ax = 0x03; int86(0x33,&registr,&registr);
 return registr.x.bx & 2;
}


void Mouse::Pos( int &x,int &y )
{
 registr.x.ax = 0x03; int86 (0x33,&registr,&registr);
// x = registr.x.cx>>(diference-1);
 x = registr.x.cx>>1;
 y = registr.x.dx;
}


int Mouse::PosX( void )
{
 registr.x.ax = 0x03; int86 (0x33,&registr,&registr);
// return registr.x.cx>>(diference-1);
 return registr.x.cx>>1;  // especially changed for drawup & Win95
}


int Mouse::PosY( void )
{
 registr.x.ax = 0x03; int86 (0x33,&registr,&registr);
 return registr.x.dx;
}


void Mouse::Move( int x, int y )
{
 registr.x.ax = 0x04;
 registr.x.cx = x;
 registr.x.dx = y;
 int86(0x33,&registr,&registr);
}


void Mouse::SpaceX( int xmin, int xmax )
{
 registr.x.ax = 0x07;
 registr.x.cx = xmin;
 registr.x.dx = xmax;
 int86(0x33,&registr,&registr);
}


void Mouse::SpaceY( int ymin, int ymax )
{
 registr.x.ax = 0x08;
 registr.x.cx = ymin;
 registr.x.dx = ymax;
 int86(0x33,&registr,&registr);
}


void Mouse::Break( int length ) { delay( length ); }


void Mouse::Wait( void ) { do ; while( !RightEar() && !LeftEar() ); }


void Mouse::PosGraph ( int ux, int uy, int fcol, int bcol )
{
  int x,y;

  Pos( x,y );
  if ( OldX != x )
   {
    itoa( x,StringX,10 );
    Hide();
     setfillstyle( SOLID_FILL,bcol );
     bar( ux,uy,ux+48,uy+10 );
     setcolor( fcol ); outtextxy( ux,uy+2,StringX ); OldX = x;
    Show();
   }
   if ( OldY != y )
    {
     itoa( y,StringY,10 );
     Hide();
      setfillstyle( SOLID_FILL,bcol );
      bar( ux+50,uy,ux+100,uy+10 );
      setcolor( fcol ); outtextxy( ux+50,uy+2,StringY ); OldY = y;
     Show();
    }
}

// this method emulates mouse pointer in high resolutions
void Mouse::EUpdate( void )
{
 if( Visible && Emulate )
 {
  ex=PosX(), ey=PosY();

  if( ex!=eox || ey!=eoy )
  {
   setcolor(LIGHTGREEN); setwritemode( XOR_PUT ); color=getcolor();
   // delete old
   line( eox, eoy, eox+3, eoy+3 );
   line( eox, eoy+1, eox, eoy+2 );
   line( eox+1, eoy, eox+2, eoy );
   // draw new
   line( ex, ey, ex+3, ey+3 );
   line( ex, ey+1, ex, ey+2 );
   line( ex+1, ey, ex+2, ey );
   eox=ex; eoy=ey;
   setwritemode( EWriteMode ); setcolor( color );
  }
 }
}
