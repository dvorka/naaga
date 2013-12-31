#include "gui.h"


#define VYSKA_HLAVICKY 16
// #define DEBUG


extern Mouse mouse;

char ErrorLevel;


//- Cuflik methods ------------------------------------------------------------


Cuflik::Cuflik( int iX, int iY, int iSirka, int iVyska,
		int iTxtBarva, int iBgBarva, int iFrameBarva,
		char *iPopis )
{
 X=iX;
 Y=iY;
 Sirka=iSirka;
 Vyska=iVyska;
 TxtBarva=iTxtBarva;
 BgBarva=iBgBarva;
 FrameBarva=iFrameBarva;
 Popis=iPopis;
 PushFlag=FALSE;
}


void Cuflik::Draw( void )
{
 mouse.Hide();
  setfillstyle( SOLID_FILL, BgBarva );
  bar(X+1,Y+1,X+Sirka-1,Y+Vyska-1);
  setcolor( FrameBarva );
  rectangle(X,Y,X+Sirka,Y+Vyska);
  setcolor( TxtBarva );
  settextjustify( CENTER_TEXT, TOP_TEXT );
  outtextxy(X+Sirka/2,Y+3+1,Popis);
  settextjustify( LEFT_TEXT, TOP_TEXT ); // default
 mouse.Show();
}


int Cuflik::Update( int x, int y )
{
 if( x>=X && x<=(X+Sirka) && y>=Y && y<=(Y+Vyska) )
 {
  PushFlag=TRUE;
  return TRUE;
 }
 return FALSE;
}


//- Wokno methods ------------------------------------------------------------


Wokno::Wokno( int iX, int iY, int iSirka, int iVyska,
	  int iHeadTxtBarva, int iHeadBgBarva,
	  int iTxtBarva, int iBgBarva, int iFrameBarva,
	  char *iHlavicka )
{
 X=iX;
 Y=iY;
 Sirka=iSirka;
 Vyska=iVyska;
 HeadTxtBarva=iHeadTxtBarva;
 HeadBgBarva=iHeadBgBarva;
 TxtBarva=iTxtBarva;
 BgBarva=iBgBarva;
 FrameBarva=iFrameBarva;
 Hlavicka=iHlavicka;

 int velikost=imagesize( X, Y, X+Sirka, Y+Vyska );

 #ifndef DEBUG
 if ( (Underground=farmalloc(velikost))==NULL )
 {
  closegraph();
  printf("\n Free memory %lu B, required %lu B" , farcoreleft(), velikost);
  printf("\n Error - not enough heap space to save Win's underground ...");
  exit(1);
 }

 mouse.Hide();
  getimage( X, Y, X+Sirka, Y+Vyska, Underground);
 mouse.Show();
 #endif
}


Wokno::~Wokno()
{
 #ifndef DEBUG
 farfree( Underground );
 #endif
}


void Wokno::Draw( void )
{
 mouse.Hide();
  // whole
  setcolor( FrameBarva );
  rectangle(X,Y,X+Sirka,Y+Vyska);
  setfillstyle( SOLID_FILL, BgBarva );
  bar(X+1,Y+1,X+Sirka-1,Y+Vyska-1);

  // hlavicka
  setfillstyle( SOLID_FILL, HeadBgBarva );
  bar(X+1,Y+1,X+Sirka-1,Y+VYSKA_HLAVICKY-1);
  setcolor( HeadTxtBarva );
  settextjustify( CENTER_TEXT, TOP_TEXT );
  outtextxy(X+Sirka/2,Y+3+1,Hlavicka);
  settextjustify( LEFT_TEXT, TOP_TEXT ); // default
 mouse.Show();
}


void Wokno::Skovej( void )
{
 #ifndef DEBUG
 mouse.Hide();
  putimage( X, Y, Underground, COPY_PUT );
 mouse.Show();
 #endif
}


//- ThreeButtonWnd methods ------------------------------------------------------------


ThreeButtonWnd::ThreeButtonWnd( int iX, int iY,
				char *h, char *b1, char *b2, char *b3 )
{
 X=iX;
 Y=iY;

 #define BTN_WIDTH    70
 #define BTN_HEIGHT   14
 #define WIN_WIDTH    BTN_WIDTH*3+(2+2)*10
 #define WIN_HEIGHT   70-10-10

 MasterCuflik=new Cuflik(X+10, Y+30-5, BTN_WIDTH, BTN_HEIGHT,
		WHITE, 127, BLACK,
		b1 );
 SlaveCuflik=new Cuflik(X+10*2+BTN_WIDTH, Y+30-5, BTN_WIDTH, BTN_HEIGHT,
		WHITE, 127, BLACK,
		b2 );
 CancelCuflik=new Cuflik(X+10*3+BTN_WIDTH*2, Y+30-5, BTN_WIDTH, BTN_HEIGHT,
		WHITE, 127, BLACK,
		b3 );

 Okno=new Wokno( X, Y, WIN_WIDTH, WIN_HEIGHT,
	  WHITE, 127,
	  LIGHTRED, LIGHTGRAY, 25,
	  h );

 #undef BTN_WIDTH
 #undef BTN_HEIGHT
 #undef WIN_WIDTH
 #undef WIN_HEIGHT
}


ThreeButtonWnd::~ThreeButtonWnd()
{
 Okno->Skovej();

 delete MasterCuflik;
 delete SlaveCuflik;
 delete CancelCuflik;
 delete Okno;
}


void ThreeButtonWnd::Draw( void )
{
 Okno->Draw();
 MasterCuflik->Draw();
 SlaveCuflik->Draw();
 CancelCuflik->Draw();
}


int ThreeButtonWnd::Choosen( void )
{
 int x=0, y=0, retval=INTERNIC;


 while( !retval )
 {
  // mouse.EUpdate();

  if( mouse.LeftEar() )
  {
   mouse.Pos( x, y );
    retval=REGISTER*MasterCuflik->Update(x,y);
    retval|=REPLACE*SlaveCuflik->Update(x,y);
    retval|=DISCARD*CancelCuflik->Update(x,y);
   mouse.Break();
  }

  if( kbhit() ) // escape
  {
   if( getch()==27 )
   {
    retval=FALSE;
    break;
   }
  }
 }

 return retval;
}


//- OneCancelWnd methods ------------------------------------------------------------


OneButtonWnd::OneButtonWnd( int iX, int iY,
			    char *iHead, char *iBtnMsg, char *iMasage )
{
 X=iX;
 Y=iY;
 Masage=iMasage;

 #define BTN_WIDTH    70
 #define BTN_HEIGHT   14
 #define WIN_WIDTH    BTN_WIDTH*3+(2+2)*10
 #define WIN_HEIGHT   70-10

 CancelCuflik=new Cuflik(X+(WIN_WIDTH)/2-(BTN_WIDTH)/2, Y+36,
			 BTN_WIDTH, BTN_HEIGHT,
		WHITE, BLUE, BLACK,
		iBtnMsg );

 Okno=new Wokno( X, Y, WIN_WIDTH, WIN_HEIGHT,
	  WHITE, BLUE,
	  BLACK, LIGHTGRAY, BLACK,
	  iHead );

 #undef BTN_WIDTH
 #undef BTN_HEIGHT
 #undef WIN_WIDTH
 #undef WIN_HEIGHT
}


OneButtonWnd::~OneButtonWnd()
{
 Okno->Skovej();

 delete CancelCuflik;
 delete Okno;
}


void OneButtonWnd::Draw( void )
{
 Okno->Draw();
 CancelCuflik->Draw();

 mouse.Hide();
  setcolor( BLACK );
  outtextxy( X+8, Y+21, Masage );
 mouse.Show();
}


int OneButtonWnd::Update( void )
{
 int x=0, y=0, retval=FALSE;

 if( mouse.LeftEar() )
 {
  mouse.Pos( x, y );
  retval=TRUE*CancelCuflik->Update(x,y);
  mouse.Break();
 }

 return retval;
}


//- GuiTxtLn methods ------------------------------------------------------------


GuiTxtLn::GuiTxtLn( int iX, int iY, int iSirka,
	     int iBgBarva, int iFgBarva, int iFrameBarva )
{
 X=iX;
 Y=iY;
 Sirka=iSirka;
 Vyska=8+2*(2+2);
 BgBarva=iBgBarva;
 FgBarva=iFgBarva;
 FrameBarva=iFrameBarva;
}


void GuiTxtLn::Draw( void )
{
 mouse.Hide();
  setcolor( FrameBarva );
  rectangle( X, Y, X+Sirka, Y+Vyska );
  setfillstyle( SOLID_FILL, BgBarva );
  bar( X+1, Y+1, X+Sirka-1, Y+Vyska-1 );
 mouse.Show();
}


void GuiTxtLn::Clear( void )
{
 mouse.Hide();
  setfillstyle( SOLID_FILL, BgBarva );
  bar( X+1, Y+1, X+Sirka-1, Y+Vyska-1 );
 mouse.Show();
}


void GuiTxtLn::Put( char *Masage )
{
 mouse.Hide();
  setfillstyle( SOLID_FILL, BgBarva );
  bar( X+1, Y+1, X+Sirka-1, Y+Vyska-1 );
  setcolor( FgBarva );
  outtextxy( X+3, Y+3+1, Masage );
 mouse.Show();
}

