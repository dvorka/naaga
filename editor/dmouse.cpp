//----------------------------------------------------------------------------------
//                           Methods of DMOUSE.H
//                                 Dvorka
//                                  1996
//----------------------------------------------------------------------------------


#include "dmouse.h"

#define DRAW_OLD_POINTER( EXIT_MODE )                           \
  color=getcolor();                                             \
  setcolor(LIGHTGREEN); setwritemode( XOR_PUT );                \
  line( eox, eoy, eox+3, eoy+3 );                               \
  line( eox, eoy+1, eox, eoy+2 );                               \
  line( eox+1, eoy, eox+2, eoy );                               \
  setwritemode( EXIT_MODE ); setcolor(color);

#define DRAW_NEW_POINTER( EXIT_MODE )                           \
  color=getcolor();                                             \
  setcolor(LIGHTGREEN); setwritemode( XOR_PUT );                \
  line( ex, ey, ex+3, ey+3 );                                   \
  line( ex, ey+1, ex, ey+2 );                                   \
  line( ex+1, ey, ex+2, ey );                                   \
  eox=ex; eoy=ey;                                               \
  setwritemode( EXIT_MODE ); setcolor( color );

  static eox, eoy;


  #define CROSS 0
  #define ARROW 1
  #define PEN   2

typedef
 struct
 {
  word mask[2][16]; // foreground+backrground: 8x8
  word x, y;        // active point
 }
MouseCursor;


MouseCursor MouseCursorTable[] // 16x16 pixels
={
  ~0x1000,~0x1000,~0x1000,~0xEE00,~0x1000,~0x1000,~0x1000,~0x0000, // fg
  ~0x0000,~0x0000,~0x0000,~0x0000,~0x0000,~0x0000,~0x0000,~0x0000,
  0x1000,0x1000,0x1000,0xEE00,0x1000,0x1000,0x1000,0x0000,         // bg
  0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
  4,3, // cross


  ~0x0000,~0xC000,~0xE000,~0xF000,~0xF800,~0xFC00,~0xFE00,~0xFF00,
  ~0xFF80,~0xFFC0,~0xFFE0,~0xFFE0,~0xFF00,~0xE780,~0xC3C0,~0x01C0,
  0x0000,0x0000,0x4000,0x6000,0x7000,0x7800,0x7C00,0x7E00,
  0x7F00,0x7F80,0x7FC0,0x7E00,0x6600,0x4300,0x0180,0x0000,
  0,0, // arrow

  ~0x0000,~0x6000,~0x7000,~0x3800,~0x1C00,~0x0E00,~0x0700,~0x03A0,
  ~0x01D0,~0x00E8,~0x0074,~0x003C,~0x001C,~0x000C,~0x0000,~0x0000,
  0x0000,0x6000,0x7000,0x3800,0x1C00,0x0E00,0x0700,0x03A0,
  0x01D0,0x00E8,0x0074,0x003C,0x001C,0x000C,0x0000,0x0000,
  0,0  // pen

 };


Mouse::Mouse( int iButtonNumb ) { ButtonNumb = iButtonNumb; };

char Mouse::Init( int idiference, int emulate  ) // returns diference
{
 OneButtonWnd *wokno;


 registr.x.ax = 0x00; int86 (0x33,&registr,&registr);

 if (registr.x.ax == 0x00)
  {
    while( kbhit() ) getch();
    wokno = new OneButtonWnd( 30, 30, "Error", "OK", "No mouse HW or SW installed!" );
    wokno->Draw();
    getch();
    delete wokno;
  }

 if ( registr.x.bx != ButtonNumb )
  {
   if( ButtonNumb == 2 )
   {
    while( kbhit() ) getch();
    wokno = new OneButtonWnd( 30, 30, "Error", "OK", "Please use 2-button mouse." );
    wokno->Draw();
    getch();
    delete wokno;
   }
  }

 // decide x = x or x = 2x, here initialized by user
 diference=idiference;

 // zjisteni  typu ovladace jestli x = x || x = 2x
 /*
 Show();
 Move( 10,0 );
 i=PosX();        // in case of compilers optimizations
 diference=i/10;
 Hide();
 */

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
 // x = registr.x.cx>>1;
    x = registr.x.cx/diference;

 y = registr.x.dx;
}


int Mouse::PosX( void )
{
 registr.x.ax = 0x03; int86 (0x33,&registr,&registr);
 // return registr.x.cx>>(diference-1);
 //  return registr.x.cx>>1;           // especially changed for drawup & Win95
  return registr.x.cx/diference;    // detect
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
 registr.x.cx = xmin*diference;
 registr.x.dx = xmax*diference;
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


void Mouse::SetCursor( byte ID )
{
  registr.x.bx=MouseCursorTable[ID].x;          // active point
  registr.x.cx=MouseCursorTable[ID].y;

  unsigned h=FP_SEG((void far *)(MouseCursorTable+ID));
  asm MOV ES, h;
  registr.x.dx=FP_OFF((void far *)(MouseCursorTable+ID));
  registr.x.ax=0x0009;

  int86( 0x33, &registr, &registr );
}

