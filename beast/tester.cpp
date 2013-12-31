//----------------------------------------------------------------------------------
//
//			 	Beast tester
//
//				   Dvorka
//				    1999
//
//----------------------------------------------------------------------------------

#include "tester.h"

#define WITH_FADEOUTS

extern Beast *shellBeast;

// globals
byte far  *ComponentArray;
word      ComponentIndex;       // index of the first free cell

word far  *LevelArray;
word      GlobalXLevel,
	  GlobalYLevel;

byte far  *Background;

byte far  *GameMenu;

byte far  *Original;
word	  OriginalFP_OFF,
	  OriginalFP_SEG;

byte far  *BeastPool;		// virtual screen for drawing beasts

word	  ListIndex;

char   	  lvlFile[50];		// name of the level file *.lvl

extern Mouse mouse;
extern byte Palette[256][3];

//----------------------------------------------------------------------------------

int tester( void )
{
 DGraphics graph;

 // global initializations
 directvideo = 1; 		// video directly from HW, DOES NOT USE BIOS
 GlobalXLevel=0;
 GlobalYLevel=0;
 ComponentIndex=0;

 int  Action, handle;
 int  x;
// int  y;
 int  ExitApplication=FALSE;
 byte ActuallColor, v,
      MouseDiference=2;
 int  Videomode=0;
 word sbytes, lenght;
 word XORcolor;


 // loading cfg
 _dos_open( "beasted.cfg", O_RDONLY, &handle);
  _dos_read(handle, (void far*)&MouseDiference, sizeof(MouseDiference), &sbytes);
  _dos_read(handle, lvlFile, 50, &sbytes);
  _dos_read(handle, &XORcolor, 2, &sbytes);
 _dos_close(handle);

 printf("\n\n                            NAAGA editor	"
	"\n                               Dvorka	"
	"\n                              1997-1998	"
	"\n"
	"\n Configuration file:				"
	"\n    mouse diference is %d			"
	"\n    XOR color set to %d			"
	"\n"
	, MouseDiference, XORcolor
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

 // ToDo: normalize these poiters!
 OriginalFP_OFF=FP_OFF(Original);
 OriginalFP_SEG=FP_SEG(Original);

 printf("\n Allocating BeastPool...");
 BeastPool=(byte far *)farmalloc(64000ul);
 printf(" %lu bytes after...", farcoreleft());
 if( BeastPool==NULL )
 {
  farfree(Original);
  farfree(LevelArray);
  farfree(ComponentArray);
  farfree(Background);
  farfree(GameMenu);
  printf("\n\n Unable to allocate it...");
  return 0;
 }

 // load level last editted -----------------------------------------------
 if( lvlFile[0] )
 {
  if(_dos_open( lvlFile, O_RDWR, &handle))
   { printf("\n Err: open %s", lvlFile ); KeybWait(); lvlFile[0]=0;}
  else
  {
   printf( "\n\n Active file: %s", lvlFile );
   if(_dos_read(handle, &ComponentIndex, sizeof(word), &sbytes))
    { printf("\n Err: WR %s", lvlFile ); KeybWait(); }
   else printf("\n Component index - number of used cells: %d ", ComponentIndex );
   if(_dos_read(handle, &ListIndex, sizeof(word), &sbytes))
    { printf("\n Err: WR %s", lvlFile ); KeybWait(); }
   else { ListIndex%=640; printf("\n List index in component array: %d ", ListIndex ); }
   if( _dos_read(handle, LevelArray, LEVELSIZEBYTE, &sbytes))
    { printf("\n Err: WR %s", lvlFile ); KeybWait(); }
   else printf("\n Level array ptr: %p ", LevelArray );
   if( _dos_read(handle, ComponentArray, 64000u, &sbytes))
    { printf("\n Err: WR %s", lvlFile ); KeybWait(); }
   else printf("\n Component array ptr: %p ", ComponentArray );
   if( _dos_read(handle, Background, 64000u, &sbytes))
    { printf("\n Err: WR %s", lvlFile ); KeybWait(); }
   else printf("\n Background ptr: %p ", Background );
   if( _dos_read(handle, Palette, 256*3, &sbytes))
    { printf("\n Err: WR %s", lvlFile ); KeybWait(); }
   else printf("\n Palette ptr: %p ", Palette );
   if( _dos_read(handle, GameMenu, 20*320, &sbytes))
    { printf("\n Err: WR %s", lvlFile ); KeybWait(); }
   else printf("\n Game menu ptr: %p \n", GameMenu );
   _dos_close(handle);
  }
 }
 else
 {
  puts("\n ... no file previously editted...\n");
  lvlFile[0]=0;
 }

 ActuallColor=FALSE;

 // stop on printed informatios
 if( ActuallColor )
 {
  while( kbhit() ) getch; getch();
 }

 // - end of brutal allocations ---------------------------------------------

 #ifdef WITH_FADEOUTS
  GFXFadeOut();
 #endif

 // install naaga keyboard handler
 ConsoleConstructor();

 #ifdef USE_DGRAPHIC
  InitChain4();
 #else
  graph.Init( Videomode );
 #endif

 mouse.Init( MouseDiference );
 mouse.Move( 320/2, 200/2 );
 mouse.SetCursor(mouse.CROSS);

 // show cells content
 if( lvlFile[0] )
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

 mouse.Show();

 KeybClear();

 #ifdef WITH_FADEOUTS
  GFXFadeIn();
 #endif

 #ifdef ROTATE
  InstallPaletteRotator();
 #endif

 shellBeast->y=shellBeast->x=0;

 do
 {

  shellBeast->move();

  // draw screen
  ShowLevelCells();

  if( kbhit() )
  {
   switch( getch() )
   {
    case 'x':
	    ExitApplication=TRUE;
	    break;

    // slow scrolling
    case '2': // down arrow
		      MoveWindowInLevelArray( DOWN );
		     break;
    case '3': // right arrow
		      MoveWindowInLevelArray( RIGHT );
		     break;
    case '1': // left arrow
		      MoveWindowInLevelArray( LEFT );
		     break;
    case '5': // up arrow
		      MoveWindowInLevelArray( UP );
		     break;

    case 0:
	   switch( getch() )
	   {
	    case 68: // F10
	    case 45: // Alt-x
		      ExitApplication=TRUE;
		      break;
	    /*
	    // slow scrolling
	    case 80: // down arrow
		      MoveWindowInLevelArray( DOWN );
		     break;
	    case 77: // right arrow
		      MoveWindowInLevelArray( RIGHT );
		     break;
	    case 75: // left arrow
		      MoveWindowInLevelArray( LEFT );
		     break;
	    case 72: // up arrow
		      MoveWindowInLevelArray( UP );
		     break;
	    */
	    // fast scrolling
	    case 160: // down arrow
		      MoveWindowInLevelArray( DOWN, TRUE, FALSE );
		     break;
	    case 157: // right arrow
		      MoveWindowInLevelArray( RIGHT, TRUE, FALSE );
		     break;
	    case 155: // left arrow
		      MoveWindowInLevelArray( LEFT, TRUE, FALSE );
		     break;
	    case 152: // up arrow
		      MoveWindowInLevelArray( UP, TRUE, FALSE );
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

 // return old keyboard handler
 ConsoleDestructor();

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

  printf("\n Before: far heap: %lu bytes", (unsigned long) farcoreleft());
 farfree( BeastPool );
  printf("\n BeastPool: far heap: %lu bytes", (unsigned long) farcoreleft());
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
 _dos_creat( "beasted.cfg", _A_NORMAL, &handle);
  _dos_write(handle, (void far*)&MouseDiference, sizeof(MouseDiference), &sbytes);
  _dos_write(handle, lvlFile, 50, &sbytes);
  _dos_write(handle, &XORcolor, 2, &sbytes);
 _dos_close(handle);

 return 0;
}

//- EOF ----------------------------------------------------------------------------
