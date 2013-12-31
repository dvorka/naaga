//---------------------------------------------------------------------------
//
//			    Main NAAGA arcade editor
//                                Dvorka
//				 1997-1998
//
//---------------------------------------------------------------------------

#define WITH_FADEOUTS
//#define ROTATE
//#define SMOOTH

#include "console.h"
#include "defs.h"
#include "dgraphic.h"
#include "dmouse.h"
#include "edittool.h"
#include "gfx.h"
#include "gui.h"
#include "image.h"
#include "paleta.h"


#ifndef USE_DGRAPHIC
 DGraphics graph;
#endif

Mouse     mouse;
Jail 	  jail;
KrsMenu	  *PDrawMenu;

byte far  *ComponentArray;
word      ComponentIndex;       // index of the first free cell

word far  *LevelArray;
word      GlobalXLevel,
	  GlobalYLevel;

byte far  *Background;

byte far  *GameMenu;

byte MenuHidden;

#ifdef SMOOTH
 byte far  *foreg;
#endif

byte far* Original;
word	  OriginalFP_OFF,
	  OriginalFP_SEG;


extern char file[50];
extern byte Palette[256][3];
extern word ListIndex;

int main( int argc, char *argv[] )
{
 // global initializations
 directvideo = 1; 		// video directly from HW, DOES NOT USE BIOS
 MenuHidden=FALSE;
 GlobalXLevel=0;
 GlobalYLevel=0;
 ComponentIndex=0;

 int  Action, handle;
 int  x, y;
 int  ExitApplication=FALSE;
 byte ActuallColor, v,
      MenuHidden=FALSE,
      MouseDiference=2;
 int  Videomode=0;
 word sbytes, lenght;


 // loading cfg
 _dos_open( "editor.cfg", O_RDONLY, &handle);
  _dos_read(handle, (void far*)&MouseDiference, sizeof(MouseDiference), &sbytes);
  _dos_read(handle, file, 50, &sbytes);
  _dos_read(handle, &(jail.XORcolor), 2, &sbytes);
 _dos_close(handle);

 //- command line begin ------------------------------------------------------------

 ActuallColor=FALSE; // will be used for stopping on printed statistic
 v=1;
 v=argv[1][1];

 if( argc>1 )
 {
  switch( v )
  {
   case 'h':
   case 'H':
   case '?':
	    printf("\n\n                            NAAGA editor "
		   "\n                               Dvorka    "
		   "\n                              1997-1998     "
		   "\n\n Command line help:        "
		   "\n Usage: editor <-switches> "
		   "\n   -h"
		   "\n   -H"
		   "\n   -? ... for help"
		   "\n   -v ... v=0,1,3 or 5 select videomode "
		   "\n   -m ... if mouse coordinates are doubled"
		   "\n   -n ... start editor without configuration file"
		   "\n   -p ... stop on printed statistic"
		   "\n"
		   "\n"
		   "\n"
		  );
	    return 0;

   case '0': // videomodes
   case '1':
   case '3':
   case '5':
	     Videomode=v-'0';
	     break;

   case 'm': // single mouse diference ( implicitly doubled )
	     MouseDiference=1;
	     break;
   case 'n': // no configuration file
	     file[0]=0;
	     break;
   case 'p': // stop on printed statistic
	     ActuallColor=TRUE;
	     break;
  }
 }

 //- command line end ------------------------------------------------------------

 printf("\n\n                            NAAGA editor	"
	"\n                               Dvorka	"
	"\n                              1997-1998	"
	"\n"
	"\n Configuration file:				"
	"\n    mouse diference is %d			"
	"\n    XOR color set to %d			"
	"\n"
	, MouseDiference, jail.XORcolor
       );
 printf("\n Memory statistic: far heap: %lu bytes", (unsigned long) farcoreleft());

 // save palette
 GFXInit();			// put palette to array
 GFXConstructor();		// save array

 // level array contains indexes of components
 // word LevelArray[32*5][JAILROWS*5] - [x][y]
 printf("\n\n Allocating Level Array...");
 LevelArray=(word far *)farmalloc(LEVELSIZEBYTE);
 printf(" %lu bytes after...", farcoreleft());
 if( LevelArray==NULL )
 {
  printf("\n\n Unable to allocate LevelArray...");
  return 0;
 }

 // array of components
 // byte ComponentArray[640][100] - [index][data]
 printf("\n Allocating Component Array...");
 ComponentArray=(byte far *)farmalloc(64000u);
 printf(" %lu bytes after...", farcoreleft());

 if( ComponentArray==NULL )
 {
  farfree(LevelArray);
  printf("\n\n Unable to allocate ComponentArray...");
  return 0;
 }

 // background bitmap 320x200
 printf("\n Allocating background bitmap buffer...");
 Background=(byte far *)farmalloc(64000u);
 printf(" %lu bytes after...", farcoreleft());

 if( Background==NULL )
 {
  farfree(LevelArray);
  farfree(ComponentArray);
  printf("\n\n Unable to allocate background bitmap buffer...");
  return 0;
 }

 // game menu bitmap 320x180
 printf("\n Allocating game menu buffer...");
 GameMenu=(byte far *)farmalloc(320*20);
 printf(" %lu bytes after...", farcoreleft());

 if( GameMenu==NULL )
 {
  farfree(LevelArray);
  farfree(ComponentArray);
  farfree(Background);
  printf("\n\n Unable to allocate game menu buffer...");
  return 0;
 }

 printf("\n Allocating original ( virtual screen )...");
 Original=(byte far *)farmalloc(64000ul);
 printf(" %lu bytes after...", farcoreleft());
 if( Original==NULL )
 {
  farfree(LevelArray);
  farfree(ComponentArray);
  farfree(Background);
  farfree(GameMenu);
  printf("\n\n Unable to allocate it...");
  return 0;
 }
 OriginalFP_OFF=FP_OFF(Original);
 OriginalFP_SEG=FP_SEG(Original);

 #ifdef SMOOTH
  foreg=(byte far *)farmalloc(64000ul);
 #endif

 // load level last editted
 if( file[0] )
 {
  if(_dos_open( file, O_RDWR, &handle))
   { printf("\n Err: open %s", file ); KeybWait(); file[0]=0;}
  else
  {
   printf( "\n\n Active file: %s", file );
   if(_dos_read(handle, &ComponentIndex, sizeof(word), &sbytes))
    { printf("\n Err: WR %s", file ); KeybWait(); }
   else printf("\n Component index - number of used cells: %d ", ComponentIndex );
   if(_dos_read(handle, &ListIndex, sizeof(word), &sbytes))
    { printf("\n Err: WR %s", file ); KeybWait(); }
   else { ListIndex%=640; printf("\n List index in component array: %d ", ListIndex ); }
   if( _dos_read(handle, LevelArray, LEVELSIZEBYTE, &sbytes))
    { printf("\n Err: WR %s", file ); KeybWait(); }
   else printf("\n Level array ptr: %p ", LevelArray );
   if( _dos_read(handle, ComponentArray, 64000u, &sbytes))
    { printf("\n Err: WR %s", file ); KeybWait(); }
   else printf("\n Component array ptr: %p ", ComponentArray );
   if( _dos_read(handle, Background, 64000u, &sbytes))
    { printf("\n Err: WR %s", file ); KeybWait(); }
   else printf("\n Background ptr: %p ", Background );
   if( _dos_read(handle, Palette, 256*3, &sbytes))
    { printf("\n Err: WR %s", file ); KeybWait(); }
   else printf("\n Palette ptr: %p ", Palette );
   if( _dos_read(handle, GameMenu, 20*320, &sbytes))
    { printf("\n Err: WR %s", file ); KeybWait(); }
   else printf("\n Game menu ptr: %p \n", GameMenu );
   _dos_close(handle);
  }
 }
 else
 {
  puts("\n ... no file previously editted...\n");
  file[0]=0;
 }

 // stop on printed informatios
 if( ActuallColor )
 {
  while( kbhit() ) getch; getch();
 }

 // - end of brutal allocations ---------------------------------------------

 #ifdef WITH_FADEOUTS
  GFXFadeOut();
 #endif

 KeybSetFastest();

 #ifdef USE_DGRAPHIC
  InitChain4();
 #else
  graph.Init( Videomode );
 #endif

 mouse.Init( MouseDiference );
 mouse.Move( 320/2, 200/2 );
 mouse.SetCursor(mouse.CROSS);

 byte  IsArrow=FALSE;

 // show cells content
 if( file[0] )
 {
  GFXSetPal();
  GFXInit();
  #ifdef WITH_FADEOUTS
   GFXPalBlack();
  #endif
  ShowLevelCells();
  // draw game menu
  movedata( FP_SEG(GameMenu), FP_OFF(GameMenu), 0xA000, 320*JAILROWS*10,320*20);
 }
 else
 {
  GFXInit();
  //GFXConstructor();
  InitializeLevelArray();
  #ifdef WITH_FADEOUTS
   GFXPalBlack();
  #endif
  ShowLevelCells();
 }


 KrsMenu   *DrawMenu = new KrsMenu( &mouse, 0, 0 );
 PDrawMenu=DrawMenu;

 // jail.Draw( LIGHTGRAY ); draw grid ( mriz )

 mouse.Show();

 KeybClear();

 DrawMenu->Show(BLACK);
 DrawMenu->UpdateMousePos();

 #ifdef WITH_FADEOUTS
  GFXFadeIn();
 #endif

 #ifdef ROTATE
  InstallPaletteRotator();
 #endif


 do
 {
  DrawMenu->UpdateMousePos();


  // cursor changes
  y=mouse.PosY();
  if( y>=DrawMenu->Y && y<=(PDrawMenu->Y+PDrawMenu->Height) ) // over menu
  {
   if( !MenuHidden && !IsArrow)
   {
    mouse.SetCursor(mouse.ARROW);
    IsArrow=TRUE;
   }
  }
  else
  {
   if( IsArrow )
   {
    mouse.SetCursor(mouse.CROSS);
    IsArrow=FALSE;
   }
  }


  //-------------------------------------------------------------------------------

  if( mouse.RightEar() && mouse.LeftEar() )
  {
   if( !MenuHidden )
    DrawMenu->Hide();
   else
    DrawMenu->Show(0);

   MenuHidden=!MenuHidden;

   mouse.Break();
  }

  //-------------------------------------------------------------------------------

  if( mouse.LeftEar() )
  {
    x=mouse.PosX();
    y=mouse.PosY();
    Action=DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0 );

    switch( Action )
    {
     case FALSE: // not in menu or menu invisible
		if( mouse.PosY()<(JAILROWS*10) )
		 jail.Update( mouse.PosX(), mouse.PosY(), 0 );
		break;
     case MAXINT:
		 ExitApplication=TRUE;
		 break;
    }
  }

  //-------------------------------------------------------------------------------

  if( mouse.RightEar() )
  {
   x=mouse.PosX(); y=mouse.PosY();
   Action=DrawMenu->MouseRightEar( x, y, ActuallColor ); // menu help or getting menu pallete color
   switch( Action )
   {
    case FALSE: ActuallColor=getpixel( x, y );
		break;
    default:
	    break;
   }
  } // right ear pushed

  //-------------------------------------------------------------------------------


  if( kbhit() )
  {
   switch( getch() )
   {
    case '+': // menu editation
	      // menu must be at position 0, 0

	     KeybClear();

	     mouse.SetCursor(mouse.PEN);
	     IsArrow=FALSE;

	     do
	     {
	      DrawMenu->UpdateMousePos();

	      if( mouse.LeftEar() )
	      {
	       mouse.Hide();
		putpixel( mouse.PosX(), mouse.PosY(), ActuallColor );
	       mouse.Show();
	      }

	      if( mouse.RightEar() )
	      {
	       mouse.Hide();
		ActuallColor=getpixel( mouse.PosX(), mouse.PosY());
	       mouse.Show();
	       DrawMenu->UpdateActuallColor(ActuallColor);
	      }

	     }
	     while( !kbhit() );

	     KeybClear();

	     mouse.Hide();
	      getimage( 0, 0, 319, 59, Original );
	      lenght=imagesize( 0, 0, 319, 59 );

	      _dos_creat("menu.krs", _A_NORMAL, &handle);
	       _dos_write(handle, Original, lenght, (unsigned int *)&Videomode );
	      _dos_close(handle);

	     mouse.Show();
	     break;

    case 'x':
	    ExitApplication=TRUE;
	    break;

    case 'l': // load
	     if( MenuHidden ) DrawMenu->Show(0); else mouse.Hide();
	      DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0x8000|0 );
	     if( MenuHidden ) DrawMenu->Hide(); else mouse.Show();
	    break;
    case 's': // save

	     /*----------------------------------------------------------------------
	      Last flag in call MouseLeftEar:
	       - its word
	       - upper byte is index to upper menu
	       - lower byte is index to lower menu
	       - if the highest bit in byte is set i want something
	     */ //----------------------------------------------------------------------

	     if( MenuHidden ) DrawMenu->Show(0); else mouse.Hide();
	      DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0x8000|0x100 );
	     if( MenuHidden ) DrawMenu->Hide(); else mouse.Show();
	    break;

    case 27: // esc
    case 'h': // toggle menu visibility
	     if( !MenuHidden ) DrawMenu->Hide(); else DrawMenu->Show(0);

	     MenuHidden=!MenuHidden;
	    break;
    case 'd': // draw to cell
	    mouse.Hide();
	      DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0x80|3 );
	    mouse.Show();
	    break;
    case 'i': // list through cells
	    mouse.Hide();
	      DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0x80|4 );
	    mouse.Show();
	    break;
    case 'c': // copy
	    mouse.Hide();
	      DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0x8000|0x400 );
	    mouse.Show();
	    break;
    case 'v': // paste
    case 'p': // paste
	    mouse.Hide();
	      DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0x8000|0x500 );
	    mouse.Show();
	    break;
    case 0:
	   switch( getch() )
	   {
	    case 68: // F10
	    case 45: // Alt-x
		      ExitApplication=TRUE;
		      break;
	    // moving with whole level


	    case 59: // F1 help
	      DrawMenu->Message("Right mouse ear gives help");
	      break;

	    case 61: // F3 load
	     if( MenuHidden ) DrawMenu->Show(0); else mouse.Hide();
	      DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0x8000 );
	     if( MenuHidden ) DrawMenu->Hide(); else mouse.Show();
	    break;
	    case 63: // F5 save
	     if( MenuHidden ) DrawMenu->Show(0); else mouse.Hide();
	      DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0x8000|0x100 );
	     if( MenuHidden ) DrawMenu->Hide(); else mouse.Show();
	    break;
	    case 60: // F2 quicksave
	      DrawMenu->MouseLeftEar( x, y, ExitApplication, ActuallColor, 0x8000|0x200 );
	     if( MenuHidden ) DrawMenu->Hide(); else mouse.Show();
	    break;


	    // slow scrolling
	    case 80: // down arrow
		      DrawMenu->Hide();
		      MoveWindowInLevelArray( DOWN );
		      if( !MenuHidden ) DrawMenu->Show(BLACK);

		     break;
	    case 77: // right arrow
		      DrawMenu->Hide();
		      MoveWindowInLevelArray( RIGHT );
		      if( !MenuHidden ) DrawMenu->Show(BLACK);

		     break;
	    case 75: // left arrow
		      DrawMenu->Hide();
		      MoveWindowInLevelArray( LEFT );
		      if( !MenuHidden ) DrawMenu->Show(BLACK);

		     break;
	    case 72: // up arrow
		      DrawMenu->Hide();
		      MoveWindowInLevelArray( UP );
		      if( !MenuHidden ) DrawMenu->Show(BLACK);

		     break;

	    // fast scrolling
	    case 160: // down arrow
		      DrawMenu->Hide();
		      MoveWindowInLevelArray( DOWN, TRUE, FALSE );
		      if( !MenuHidden ) DrawMenu->Show(BLACK);

		     break;
	    case 157: // right arrow
		      DrawMenu->Hide();
		      MoveWindowInLevelArray( RIGHT, TRUE, FALSE );
		      if( !MenuHidden ) DrawMenu->Show(BLACK);

		     break;
	    case 155: // left arrow
		      DrawMenu->Hide();
		      MoveWindowInLevelArray( LEFT, TRUE, FALSE );
		      if( !MenuHidden ) DrawMenu->Show(BLACK);

		     break;
	    case 152: // up arrow
		      DrawMenu->Hide();
		      MoveWindowInLevelArray( UP, TRUE, FALSE );
		      if( !MenuHidden ) DrawMenu->Show(BLACK);

		     break;
	    default:
		     KeybClear();
	   }
    default:
	    KeybClear();
   }
  }


  //-------------------------------------------------------------------------------

  if( ExitApplication ) break;
 }
 while( TRUE );

 #ifdef ROTATE
  DeinstallPaletteRotator();
 #endif

 #ifdef WITH_FADEOUTS
  GFXFadeOut();
 #endif


 #ifdef USE_DGRAPHIC
  closegraph();
 #else
  graph.Close();
 #endif

 #ifdef WITH_FADEOUTS
  GFXPalBlack();
 #endif

 delete DrawMenu;

 #ifdef SMOOTH
  farfree(foreg);
 #endif

  printf("\n Before: far heap: %lu bytes", (unsigned long) farcoreleft());
 farfree( Original );
  printf("\n Original: far heap: %lu bytes", (unsigned long) farcoreleft());
 farfree(GameMenu);
  printf("\n GameMenu: far heap: %lu bytes", (unsigned long) farcoreleft());
 farfree(Background);
  printf("\n PBackground: far heap: %lu bytes", (unsigned long) farcoreleft());
 farfree(ComponentArray);
  printf("\n ComponentArray: far heap: %lu bytes", (unsigned long) farcoreleft());
 farfree(LevelArray);
  printf("\n LevelArray: far heap: %lu bytes", (unsigned long) farcoreleft());

 printf("\n\n Editor compiled at %s, %s\n", __DATE__, __TIME__
       );
 printf("\n Memory statistic: far heap: %lu bytes", (unsigned long) farcoreleft());
 printf("\n Bye! \n\n");


 #ifdef WITH_FADEOUTS
  GFXFadeIn(15);
  GFXDestructor();
 #endif

 // save cfg
 _dos_creat( "editor.cfg", _A_NORMAL, &handle);
  _dos_write(handle, (void far*)&MouseDiference, sizeof(MouseDiference), &sbytes);
  _dos_write(handle, file, 50, &sbytes);
  _dos_write(handle, &(jail.XORcolor), 2, &sbytes);
 _dos_close(handle);

 return 0;
}
