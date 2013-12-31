#include "edittool.h"

// background scrolling posibilities
// #define SMOOTH
 #define NORMAL_BG
// #define SYMETRIC_BG
// #define ZOOM4_BG


extern Mouse 	 mouse;
extern Jail	 jail;
extern KrsMenu	 *PDrawMenu;
extern byte far  *ComponentArray;
extern byte far  *Background;
extern word far  *LevelArray;
extern word      GlobalXLevel,
		 GlobalYLevel;
extern byte      MenuHidden;
extern byte      FlagPalette[256];
extern byte far  *Original;
#ifdef SMOOTH
 extern byte far  *foreg;		// DOCASNE - SMOOTH
#endif
extern word 	 OriginalFP_OFF;
extern word 	 OriginalFP_SEG;

byte BgLayer=TRUE; 	 // true if background layer visible
byte FgLayer=TRUE;       // true if foreground layer visible
word Layer=0;            // 0 if bg layer active, 16000 if fg layer active
			 // it is off of layer in level array

word BackgroundXOff=0;
word HelpBackgroundXOff=0;

// predelavam na 5x5 obrazovek => rychlejsi scrooling pozadi

// shift bckg when :
//   320 background, 5*320 foreground => each 5.
//   but scrool is < 0, virtual width - screen width > => 4
word WhenX=4;

word BackgroundYOff=0;
word HelpBackgroundYOff=0;
// shift bckg when :
//   20 background, 5*180 forground => each 45.
//   but scrool is < 0, virtual height - screen height > => 4

#ifdef ZOOM4_BG
 word WhenY=4;
#else
 word WhenY=45;
#endif


extern "C" { void SlowShow320x200( word OriginalFP_SEG, word OriginalFP_OFF, word number ); };
extern "C" { void ORPut( word OriginalFP_SEG, word OriginalFP_OFF,
			 word foregFP_SEG,    word foregFP_OFF    ); }


//----------------------------------------------------------------------------


void Jail::HideActive( void )
{
  mouse.Hide();
   setcolor( XORcolor );
   setwritemode(XOR_PUT);

    if( !NoErase )
    {
     rectangle( oldx-1, oldy-1, oldx+9+1, oldy+9+1 );
     NoErase=TRUE;
    }
    else
     NoErase=FALSE;

   setwritemode(COPY_PUT);
  mouse.Show();
}


//----------------------------------------------------------------------------


void Jail::ShowActive( void )
{
  mouse.Hide();
   setcolor( XORcolor );
   setwritemode(XOR_PUT);

     rectangle( oldx-1, oldy-1, oldx+9+1, oldy+9+1 );
     NoErase=FALSE;

   setwritemode(COPY_PUT);
  mouse.Show();
}


//----------------------------------------------------------------------------


void Jail::Update( int x, int y, int hcolor )
// 0..9, 10..19
{
 byte hidden=FALSE;

 hcolor=XORcolor;

 if( y<JAILROWS*10 )
 {
  x=(x/10)*10;
  y=(y/10)*10;

  if( x!=oldx || y!=oldy )
  {

  // predelat podminky jsou zbytecne zbesile!!!

  if( ( PDrawMenu->Y>=(y-1) && PDrawMenu->Y<=(y+9+1) && y ) ||
      ( PDrawMenu->Y>=y && PDrawMenu->Y<=(y+9+1) && !y ) ||

      ( PDrawMenu->Y<=(y+9+1) && (PDrawMenu->Y+PDrawMenu->Height)>=(y-1) && y ) ||
      ( PDrawMenu->Y<=(y+9) && (PDrawMenu->Y+PDrawMenu->Height)>=(y) && !y ) ||

      ( PDrawMenu->Y<=(oldy+9+1) && (PDrawMenu->Y+PDrawMenu->Height)>=(oldy-1) && oldy ) ||
      ( PDrawMenu->Y<=(oldy+9) && (PDrawMenu->Y+PDrawMenu->Height)>=(oldy) && !oldy )
    )
  {
   PDrawMenu->Hide(); hidden=TRUE;
  }

  mouse.Hide();
   setcolor( hcolor );
   setwritemode(XOR_PUT);

    if( !NoErase )
     rectangle( oldx-1, oldy-1, oldx+9+1, oldy+9+1 );
    else
     NoErase=FALSE;

    rectangle( x-1, y-1, x+9+1, y+9+1 );
   setwritemode(COPY_PUT);
  mouse.Show();

  if( hidden ) PDrawMenu->Show(LIGHTGRAY);

  oldx=x; oldy=y;
 } //same
 else
 {
  if( NoErase )
  {
   // TADY JE PES!
     NoErase=FALSE;
  }
 }

 }
}


//----------------------------------------------------------------------------


void Jail::Up( void )
{
 if( oldy )
 {
//  Update( oldx, oldy-10, 255 );
  PDrawMenu->Message("Cell up...");
 }
}


//----------------------------------------------------------------------------


void Jail::Down( void )
{
 if( oldy<(JAILROWS*10) )
 {
//  Update( oldx, oldy+10, 255 );
  PDrawMenu->Message("Cell down...");
 }
}


//----------------------------------------------------------------------------


void Jail::Right( void )
{
 if( oldx<310 )
 {
//  Update( oldx+10, oldy, 255 );
  PDrawMenu->Message("Cell right...");
 }
}


//----------------------------------------------------------------------------


void Jail::Left( void )
{
 if( oldx )
 {
//  Update( oldx-10, oldy, 255 );
  PDrawMenu->Message("Cell left...");
 }
}


//----------------------------------------------------------------------------


void InitializeLevelArray( void )
{
 word i,j;

 for( i=0; i<(LEVELSIZEBYTE); i++ ) // was lev.../2
 {
//  LevelArray[i]=i%250; // generating nr of cell
  LevelArray[i]=0;
 }


 for( i=0; i<255; i++ )
 {
  for( j=0; j<100; j++ )
   ComponentArray[i*100+j]=0; //   ComponentArray[i*100+j]=i;

 }


 for( i=0; i<64000l; i++ )
  Background[i]=0;
}


//----------------------------------------------------------------------------


void ShowBackground( void )
{
 word j=BackgroundXOff, // how much shifted right
      radek, sirka;

  for(radek=0;radek<200; radek++)
  {
   // leva cast z kukadlo1
   for( sirka=0; sirka<(320-j); sirka++ )
    Original[radek*320+sirka] = Background[radek*320+j+sirka];

   // prava cast z kukadlo2
   for( sirka=(320-j); sirka<320; sirka++ )
    Original[radek*320+sirka] = Background[radek*320+(sirka-(320-j))];
  }


 // nezachovava nejake registry
 SlowShow320x200( OriginalFP_SEG, OriginalFP_OFF, 14400 ); // 18 rows, whole: 16000
}


//----------------------------------------------------------------------------


void ShowLevelCells( void )
{
 word i, yy, xx;
 word j,
      nonj=320-BackgroundXOff,
      radek, sirka,
      forsage;

 mouse.Hide();

 xx=BackgroundYOff*320;

  for( radek=0; radek<(18*10*320l); radek+=320 )
  {
   yy=radek+xx;

   radek+=nonj-1;

   for( sirka=0; sirka<nonj; sirka++ ) 				// left symetric
    Original[radek-sirka] = Background[yy+sirka];

   radek-=nonj-1;
   yy-=nonj;

   for( sirka=nonj; sirka<320; sirka++ )			// right Original and symetric
    Original[radek+sirka] = Background[yy+sirka];

  }


 // cells

 if( BgLayer )
 {
 yy=GlobalYLevel*32*5;

 for( sirka=0; sirka<32*10; sirka+=10 )
 {
  xx=GlobalXLevel+sirka/10;

  for( radek=0; radek<18*32*5; radek+=32*5 )
  {
   forsage=LevelArray[yy+radek+xx]*100;

   if( forsage ) // if not 0 => empty cell
   {
    j=radek*20+sirka;

    for(i=0; i<100; i++, forsage++ ) // fill rectangle with current cell
    {
     if( nonj=ComponentArray[forsage] ) // its color
      Original[j+i/10*320+i%10]=nonj;
    }
   }
  }
 }
 }

 if( FgLayer )
 {
 yy=GlobalYLevel*32*5+16000;

 for( sirka=0; sirka<32*10; sirka+=10 )
 {
  xx=GlobalXLevel+sirka/10;

  for( radek=0; radek<18*32*5; radek+=32*5 )
  {
   forsage=LevelArray[yy+radek+xx]*100;

   if( forsage ) // if not 0 => empty cell
   {
    j=radek*20+sirka;

    for(i=0; i<100; i++, forsage++ ) // fill rectangle with current cell
    {
     if( nonj=ComponentArray[forsage] ) // its color
      Original[j+i/10*320+i%10]=nonj;
    }
   }
  }
 }
 }


 SlowShow320x200( OriginalFP_SEG, OriginalFP_OFF, 14400 );

 mouse.Show();
}


//----------------------------------------------------------------------------


void MoveWindowInLevelArray( int direction, int show, int slow )
{
 word i, yy, xx, k;
 word j,
      nonj=320-BackgroundXOff,
      radek, sirka,
      forsage;

 KeybClear();
 mouse.Hide();

 do
 {

 switch( direction )
 {
  case UP:
       if( GlobalYLevel>0 ) GlobalYLevel--;
       else slow=TRUE;

       if( HelpBackgroundYOff>=10 ) HelpBackgroundYOff-=10;
       BackgroundYOff=HelpBackgroundYOff/WhenY;

       break;
  case DOWN:
       if( GlobalYLevel<=(LEVELHGT-SCREENROWS+1) ) GlobalYLevel++;
       else slow=TRUE;

       if( HelpBackgroundYOff<JAILROWS*WhenX*10 ) HelpBackgroundYOff+=10;
       BackgroundYOff=HelpBackgroundYOff/WhenY;

       break;
  case LEFT:
       if( GlobalXLevel>0 ) GlobalXLevel--;
       else slow=TRUE;

       if( HelpBackgroundXOff>=10 ) HelpBackgroundXOff-=10;
       BackgroundXOff=HelpBackgroundXOff/WhenX;

       break;
  case RIGHT:
       if( GlobalXLevel<(LEVELWDT-SCREENCOLS) ) GlobalXLevel++;
       else slow=TRUE;

       if( HelpBackgroundXOff<(WhenX*320-5) ) HelpBackgroundXOff+=10;
       BackgroundXOff=HelpBackgroundXOff/WhenX;

       break;
 }


 if( show )
 {
  #ifdef SMOOTH
   SmoothShowLevelCells( direction );
  #else

   // mouse operations slows down this code the most of all!
   // classic show level cells

   nonj=320-BackgroundXOff;
   xx=BackgroundYOff*320;

  for( radek=0; radek<(18*10*320l); radek+=320 )
  {
   yy=radek+xx;

   radek+=nonj-1;

   for( sirka=0; sirka<nonj; sirka++ ) 				// left symetric
    Original[radek-sirka] = Background[yy+sirka];

   radek-=nonj-1;
   yy-=nonj;

   for( sirka=nonj; sirka<320; sirka++ )			// right Original and symetric
    Original[radek+sirka] = Background[yy+sirka];

  }

 // cells


 // BACK OF cells ------------------------------------------------------------

 if( BgLayer )
 {
  yy=GlobalYLevel*32*5;

  for( sirka=0; sirka<32*10; sirka+=10 )
  {
   xx=GlobalXLevel+sirka/10;

   for( radek=0; radek<18*32*5; radek+=32*5 )
   {
    forsage=LevelArray[yy+radek+xx]*100;

    if( forsage ) // if not 0 => empty cell
    {
     j=radek*20+sirka;

     // last edited
     for(i=0; i<10; i++, j+=320 )
      for(k=0; k<10; k++, forsage++ )
      {
       if( ComponentArray[forsage] ) // its color
	Original[j+k]=ComponentArray[forsage];
      }
    }
   }
  }
 }

 // Foreg OF cells ------------------------------------------------------------

 if( FgLayer )
 {

  yy=GlobalYLevel*32*5+16000;

  for( sirka=0; sirka<32*10; sirka+=10 )
  {
   xx=GlobalXLevel+sirka/10;

   for( radek=0; radek<18*32*5; radek+=32*5 )
   {
    forsage=LevelArray[yy+radek+xx]*100;

    if( forsage ) // if not 0 => empty cell
    {
     j=radek*20+sirka;

     // last edited
     for(i=0; i<10; i++, j+=320 )
      for(k=0; k<10; k++, forsage++ )
      {
       if( ComponentArray[forsage] ) // its color
	Original[j+k]=ComponentArray[forsage];
      }
    }
   }
  }
 }

 SlowShow320x200( OriginalFP_SEG, OriginalFP_OFF, 14400 );

 #endif
 }

 jail.NoErase=TRUE;;

 }
 while( !kbhit()&&!slow );

 mouse.Show();

}


//----------------------------------------------------------------------------

#ifdef SMOOTH

void SmoothShowLevelCells( int direction )
{
 word x, y, i, yy, xx;
 word j, nonj,
      radek, sirka,
      forsage;
 word move; // XLevel and later move

 // misto kresleni celeho vnitrek posunout a nakreslit jen okraje

 switch( direction )
 {
  case RIGHT:
  move=GlobalXLevel-1;

 for( x=0; x<32; x++ )
 {
  j=x*10;

  for( y=0; y<18; y++ )
  {
    forsage=(LevelArray[(GlobalYLevel+y)*LEVELWDT+move+x])*100;

    for(i=0; i<100; i++ ) // fill rectangle with current cell
    {
     foreg[j+(i/10*320+i%10)]=ComponentArray[forsage+i];
    }

    j+=3200;
  }
 }


 // 0...move dodelat
 // to co je na obrazovce posunout vlevo

 j=BackgroundXOff;
 nonj=320-j;
 forsage=BackgroundYOff*320;

 for( move=10; move>=1; move-- )
 {
  for( radek=0; radek<57600; radek+=320 )
  {
      yy=radek+forsage;

      for( sirka=0; sirka<move; sirka ++ )
       Original[radek+sirka]=Original[radek+sirka+1];

      for( sirka=move; sirka<nonj; sirka++ )
      {
       if( i=foreg[radek+sirka-move] )
	Original[radek+sirka]=i;
       else
	Original[radek+sirka]=Background[yy+j+sirka];
      }

      for( sirka=nonj; sirka<320; sirka++ ) // prava cast z kukadlo2
      {
       if( i=foreg[radek+sirka-move] )
	Original[radek+sirka]=i;
       else
	Original[radek+sirka]=Background[yy+(sirka-nonj)];
      }
  }

  SlowShow320x200( OriginalFP_SEG, OriginalFP_OFF, 57600u/4u );
 }
 break;



 case DOWN:
 move=GlobalYLevel-1;

 for( x=0; x<32; x++ )
 {
  j=x*10;

  for( y=0; y<18; y++ )
  {
    forsage=(LevelArray[(move+y)*LEVELWDT+GlobalXLevel+x])*100;

    for(i=0; i<100; i++ ) // fill rectangle with current cell
    {
     foreg[j+(i/10*320+i%10)]=ComponentArray[forsage+i];
    }

    j+=3200;
  }
 }


 // 0...move dodelat
 // to co je na obrazovce posunout vlevo

 j=BackgroundXOff;
 nonj=320-j;
 forsage=BackgroundYOff*320;

 for( move=10; move>=1; move-- )
 {
  for( radek=move*320; radek<(57600); radek+=320 )
  {
      yy=radek+forsage;

      for( sirka=0; sirka<nonj; sirka++ )
      {
       if( i=foreg[radek+sirka-move*320] )
	Original[radek+sirka]=i;
       else
	Original[radek+sirka]=Background[yy+j+sirka];
      }

      for( sirka=nonj; sirka<320; sirka++ ) // prava cast z kukadlo2
      {
       if( i=foreg[radek+sirka-move*320] )
	Original[radek+sirka]=i;
       else
	Original[radek+sirka]=Background[yy+(sirka-nonj)];
      }
  }

  SlowShow320x200( OriginalFP_SEG, OriginalFP_OFF, 57600u/4u );
 }

 break;
 }

 mouse.Show();
}

#endif

//----------------------------------------------------------------------------

