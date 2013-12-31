//----------------------------------------------------------------------------------
//
//			 Beasts collisions shell main file
//
//				   Dvorka
//				    1999
//
//----------------------------------------------------------------------------------

#include <alloc.h>
#include <conio.h>
#include <stdlib.h>

#include "beast.h"
#include "beastque.h"
#include "console.h"
#include "defs.h"
#include "dmouse.h"
#include "bshell.h"

#define DEBUG

// globals
byte       far *beastScreen;
Mouse      mouse;

extern     char 	*palFile;
extern     char 	*beastFile;
extern     Beast far	*shellBeast;	// atributes of active beast

//----------------------------------------------------------------------------------

int main( void )
{
 bool exitShell=FALSE;
 char cmd;
 Beast far *b;



 palFile[0]=0;
 beastFile[0]=0;
 shellBeast=NULL;

 clrscr();
 printf("\n\n         Beast editor shell for NAAGA, ver. %s, %s\n",__DATE__, __TIME__ );
 printf("\n Memory statistic: far heap: %lu bytes", (unsigned long) farcoreleft());

 printf("\n\n Auto loading cosmo:");
  shellBeast = new Beast("cosmo.bst");

 do
 {
  // scan command
  KeybClear();
  printf("\nbeasted% ");
  cmd=getche();

  switch(cmd)
  {
   case 'q':
   case 'x':
   case 'X':
	exitShell=TRUE;
	break;

   case 'h':
   case 'H':
   case '?':
	printf("\n Help:"
	       "\n  i ... info about actuall beast"
	       "\n  k ... kill active beast"
	       "\n  N ... new beast file"
	       "\n  l ... load beast file"
	       "\n  s ... save beast file"
	       "\n  Q ... quick save - save current beast"
	       "\n  a ... add image to beast images"
	       "\n  n ... move to next image"
	       "\n  p ... move to previous image"
	       "\n  d ... show image"
	       "\n  c ... edit image checkpoints"
	       "\n  g ... get palette from BMP and save it to file in my format"
	       "\n  S ... set palette to load when switching to graphics"
	       "\n  T ... set *.lvl for testing"
	       "\n  t ... run test"
	       "\n  h ... this help"
	       "\n  x ... exit"
	      );
	break;

   case 'i':
	 shellBeastInfo();
	break;
   case 'k':
	if(shellBeast)
	{
	 delete shellBeast;
	 shellBeast=NULL;
	 printf("\n Beast killed...");
	}
	else
	 printf("\n No beast...");
	break;
   case 's':
	 shellSave();
	break;
   case 'l':
	 shellLoad();
	break;
   case 'Q':
	shellQuickSave();
	break;
   case 'a':
	 shellAddImage();
	break;
   case 't':
	 shellBeastTester();
	break;
   case 'T':
	 shellSetLVL();
	break;
   case 'd':
	 shellShowImage();
	break;
   case 'n':
	shellNextImage();
	break;
   case 'p':
	shellLastImage();
	break;
   case 'N':
	shellNewFile();
	break;
   case 'g':
	shellGetPalFromBMP();
	break;
   case 'S':
	shellSetPalName();
	break;

   default:
	printf("\n Unknown command...");
	break;
  }

 }
 while( !exitShell );

 if(shellBeast)
  delete shellBeast;

 printf("\n Memory statistic: far heap: %lu bytes", (unsigned long) farcoreleft());
 printf("\n Bye!");

 return 0;
}

//----------------------------------------------------------------------------------
/*

 word i;

 beastScreen=(byte far*)farmalloc(64000u);
 for(i=0;i<64000u;i++) beastScreen[i]=0;

 BeastQueue beasts;
 for(i=0; i<30; i++ )
  beasts.insert(new MrZombie(20+rand()%280,20+rand()%160,rand()%15), AVA_SHOT);

 DGraphics graph;

 graph.Init();
 // ---

 KeybClear();

 do
 {
  for(i=0;i<64000u;i++) beastScreen[i]=0;

  beasts.show(beastScreen);

  graph.PutFrom0x0y( beastScreen );

 }
 while(!kbhit());

 // KeybWait();

 // ---

 graph.Close();

 farfree(beastScreen);


*/
