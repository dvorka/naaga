//------------------------------------------------------------------------------
//
//		          	Beast SHELL
//
//                                 Dvorka
//                                  1999
//
//------------------------------------------------------------------------------//------------------------------------------------------------------------------//------------------------------------------------------------------------------//------------------------------------------------------------------------------

#include "bshell.h"

char  beastFile[20];   	// active beast file
char  palFile[20];   	// name of palette to use
char  noName[50];   	// buffer for miscellaneous use
char  noName1[50];   	// buffer for miscellaneous use
Beast far *shellBeast=NULL; 	// actuall beast

extern char   	  lvlFile[50];		// name of the level file *.lvl

//----------------------------------------------------------------------------------

void shellBeastInfo()
{
 if(shellBeast)
 {
  printf("\n Beast name     : %s"
	 "\n Type <0,3>     : %u"
	 "\n Size of images : %u"
	 "\n Nr. of images  : %u"
	 "\n about2Dye : %u"
	 "\n imageID   : %u"
	 "\n Energy    : %lu"
	 "\n X	: %u"
	 "\n Y	: %u"
	 "\n OX	: %u"
	 "\n OY	: %u",
	 beastFile,
	 shellBeast->type,
	 shellBeast->sizeOfImages,
	 shellBeast->nrOfImages,
	 shellBeast->about2Dye,
	 shellBeast->ID,
	 shellBeast->energy,
	 shellBeast->x,
	 shellBeast->y,
	 shellBeast->ox,
	 shellBeast->oy
	);
 }
 else
  printf("\n No beast");

}

//----------------------------------------------------------------------------------

void shellNewFile()
{
 int  type;
 word nrOfImages;

 beastFile[0]=0;

 printf("\n Beast name    : "); scanf("%s",beastFile);
 printf(" Type <0,3>    : "); scanf("%d",&type);
 printf(" Size of images: "); scanf("%u",&nrOfImages);

 // create instance of beast
 shellBeast= new Beast(nrOfImages,type);
}

//----------------------------------------------------------------------------------

void shellLoad()
{
 if( shellBeast )
 {
  printf("\n Beast lives, first kill it...");
  return;
 }

 beastFile[0]=0;
 printf("\n Enter filename: "); scanf("%s",beastFile);

 // load file - create new instance and fill it
 shellBeast = new Beast(beastFile);

}

//----------------------------------------------------------------------------------

void shellSave()
{
 word i, sbytes;
 int handle;

 beastFile[0]=0;

 printf("\n Enter filename: "); scanf("%s",beastFile);

 if( shellBeast )
 {
  shellBeast->save(beastFile);
  printf("\n %s saved",beastFile);
 }
 else
  printf(" No beast");

}

//----------------------------------------------------------------------------------

void shellQuickSave()
{
 int handle;

 if(beastFile[0]==0)
 {
  printf("\n Error: no filename...");
  return;
 }

 if( shellBeast )
  shellBeast->save(beastFile);
 else
  printf(" No beast");

 printf("\n %s saved",beastFile);
}

//----------------------------------------------------------------------------------

void shellAddImage()
{
 printf("\n Enter filename of *.BMP: "); scanf("%s",noName);
 shellBeast->addImage(noName);
}

//----------------------------------------------------------------------------------

void shellShowImage()
{
 DGraphics graph;
 word i, j;

 // save palette
 GFXInit();			// put palette to array
 GFXConstructor();		// save array -> editor.pal

 GFXFadeOut();

 // initialize graphics
 graph.Init();

 // if wanted load palette to array
 if( palFile )
  GFXLoadPal(palFile);
 else
 {
  printf("\n No palette file specified");
  sleep(1);

  GFXInit();     		// save palette to array
 }

  GFXPalBlack();		// make everything black



 // draw screen
 for(i=0; i<shellBeast->images[shellBeast->ID].w; i++)
  for(j=0; j<shellBeast->images[shellBeast->ID].h; j++)
   putpixel(i,j,
	    shellBeast->images[shellBeast->ID].image
	     [
	      j*(shellBeast->images[shellBeast->ID].w) + i
	     ]
	   );



 GFXFadeIn();			// show it



 // actions
 KeybClear(); getch();



 GFXFadeOut();			// out

 graph.Close();

 GFXPalBlack();                 // make everything black

 GFXFadeIn(15);			// show

 GFXDestructor();               // load palette from file
}

//----------------------------------------------------------------------------------

void shellNextImage()
{
 if( shellBeast->ID < (shellBeast->nrOfImages-1) )
  shellBeast->ID++;

 printf("\n ID is: %d/%d",shellBeast->ID,(shellBeast->nrOfImages-1));
}

//----------------------------------------------------------------------------------

void shellLastImage()
{
 if( shellBeast->ID > 0 )
  shellBeast->ID--;

 printf("\n ID is: %d/%d",shellBeast->ID,(shellBeast->nrOfImages-1));
}

//----------------------------------------------------------------------------------

void shellGetPalFromBMP()
{
 byte far *d;

 printf("\n Enter *.BMP with palette: "); scanf("%s",noName);
 printf(" Enter *.PAL to create   : "); scanf("%s",noName1);

	DGraphics graph;

	// save palette
	GFXInit();			// put palette to array
	GFXConstructor();		// save array

	GFXFadeOut();

	// initialize graphics
	graph.Init();

 // draw screen
 DBMPLoad( noName, d, SHOWBMP ); // d is not used

	GFXInit();     			// save palette to array
	GFXSavePal(noName1);		// save palette to file



	// actions
	KeybClear(); getch();



	GFXFadeOut();			// out

	graph.Close();

	GFXPalBlack();                 // make everything black

	GFXFadeIn(15);			// show

	GFXDestructor();               // load palette from file
}

//----------------------------------------------------------------------------------

void shellSetPalName()
{
 printf("\n Enter *.PAL to use: "); scanf("%s",palFile);
}

//----------------------------------------------------------------------------------

void shellBeastTester()
{
 if(!shellBeast)
 {
  printf("\n Please load some beast...");
  return;
 }

 tester();
}

//----------------------------------------------------------------------------------

void shellSetLVL()
{
 int  handle;
 word sbytes;
 byte MouseDiference;
 word XORcolor;

 printf("\n Enter *.LVL to use: "); scanf("%s",lvlFile);

 // loading cfg
 _dos_open( "beasted.cfg", O_RDONLY, &handle);
  _dos_read(handle, (void far*)&MouseDiference, sizeof(MouseDiference), &sbytes);
  _dos_read(handle, noName, 50, &sbytes);
  _dos_read(handle, &XORcolor, 2, &sbytes);
 _dos_close(handle);

 // saving it with new name
 _dos_creat( "beasted.cfg", _A_NORMAL, &handle);
  _dos_write(handle, (void far*)&MouseDiference, sizeof(MouseDiference), &sbytes);
  _dos_write(handle, lvlFile, 50, &sbytes);
  _dos_write(handle, &XORcolor, 2, &sbytes);
 _dos_close(handle);
}

//- EOF ----------------------------------------------------------------------------
