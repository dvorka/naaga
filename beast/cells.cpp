//----------------------------------------------------------------------------------
//
//			 	   Cells
//
//				   Dvorka
//				    1999
//
//----------------------------------------------------------------------------------

#include "cells.h"

#define FOREGOFF

// background scrolling posibilities
// #define SMOOTH
 #define NORMAL_BG
// #define SYMETRIC_BG
// #define ZOOM4_BG

extern Beast     *shellBeast;

extern Mouse 	 mouse;

extern byte far  *ComponentArray;
extern byte far  *Background;
extern word far  *LevelArray;
extern word      GlobalXLevel,
		 GlobalYLevel;
extern byte      FlagPalette[256];

extern byte far  *Original;		// virtual screen
extern word 	 OriginalFP_OFF;        // OFFset of virtual screen
extern word 	 OriginalFP_SEG;        // SEGment of virtual screen

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

void InitializeLevelArray( void )
{
 word i,j;

 for( i=0; i<(LEVELSIZEBYTE); i++ ) // was lev.../2
 {
  //LevelArray[i]=i%250; // generating nr of cell
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
// casem prepsat do ASM
{
 word i, yy, xx;
 word j,
      nonj=320-BackgroundXOff,
      radek, sirka,
      forsage;

 for( radek=0; radek<64000; radek++ )
  Original[radek]=0;
 /*
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
 */

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

 #ifndef FOREGOFF

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

 #endif

 // BeastScreen ------------------------------------------------------------

 shellBeast->show( Original );

 // Virtual -> Video ------------------------------------------------------------


 SlowShow320x200( OriginalFP_SEG, OriginalFP_OFF, 14400 );
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

 // BACKGround bitmap ------------------------------------------------------------

  for( radek=0; radek<64000; radek++ )
   Original[radek]=0;

/*
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
*/
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
      for(k=0; k<10; k++, forsage++ ) // k nejet od 0 ale od j az po j+10
      {
       if( ComponentArray[forsage] ) // its color
	Original[j+k]=ComponentArray[forsage];
      }
    }
   }
  }
 }

 // Foreg OF cells ------------------------------------------------------------

 #ifndef FOREGOFF

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

 #endif

 // BeastScreen ------------------------------------------------------------

 shellBeast->show( Original );

 // Virtual -> Video ------------------------------------------------------------

 SlowShow320x200( OriginalFP_SEG, OriginalFP_OFF, 14400 );

 #endif
 }

 }
 while( !kbhit()&&!slow );

}



//----------------------------------------------------------------------------

