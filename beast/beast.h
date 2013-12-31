//----------------------------------------------------------------------------------
//
//			          BEAST.H
//
//				   Dvorka
//				    1999
//
//----------------------------------------------------------------------------------

#ifndef __BEAST_H
 #define __BEAST_H

 #include <alloc.h>
 #include <fcntl.h>
 #include <dos.h>
 #include <stdlib.h>
 #include <stdio.h>

 #include "defs.h"
 #include "dbmp.h"
 #include "console.h"

 // types of beasts
 #define AVATAR		0x00000000
 #define AVA_SHOT	0x00000001
 #define SOFT_SHOT	0x00000002
 #define HARD_SHOT	0x00000003

 // MOVES
 #define bUP	0x0001
 #define bDOWN	0x0002
 #define bLEFT	0x0004
 #define bRIGHT	0x0010
 #define bJUMP	0x0020
 #define bFIRE	0x0040

 // size of image vector in Beast, this vector is filled in time
 #define BEAST_IMAGES	30

//----------------------------------------------------------------------------------

struct far BeastImg		// always constr. and destr. manually: farfree
{
  byte far *image;           	// pointer to image
  word w,
       h;			// dimensions of this image
  word far *checkPoints;	// check points of this image
				// checkpoint is offset in image
				// so position can be count this way:
				// posx = point % x
				// posy = point / y
  word checkLng;		// nuber of checkpoints (tuple of words)

 // pseudo constructors
 void load( int handle);	// loads one BeastImg record from file
 void save( int handle);	// save self to BeastImg file
 void square(int w, int h, byte color);

 // pseudo destructor
 void destruct();
};

//----------------------------------------------------------------------------------

class far Beast
{
 public:
  Beast();
  Beast( char *fileName );
  Beast( word sizeOfImages, int type );
  ~Beast();
   void move();                 // recounts internal x and y
   void unmove();               // return back last move
   bool check(byte far* screen);// argument is collision screen
   void show(byte far* screen);	// draw cur. img on cur. coords to screen

   bool collision(byte far* screen);	// this method is called when object
					// is in collision, TRUE want delete
   void save(char *fileName);   // flush beast to disk

   void addImage(char *BMPFile);// adds item to imagesVector, image is BMP
				// file -> w and h are get from BMP header
				// then specify checkpoints
   void specifyChecks(word ID); // specification of beast checkpoints
				// (in graphics using mouse)


 public:
  word 	   sizeOfImages,	// lengt of allocated vector
	   nrOfImages;		// number of images - points to empty vecItem
  BeastImg far *images;       	// array of images
				// new BeastImg[] calls constructors!

  word     far *moveDescr;      // string describing move

  dword energy;
  int   type;       		// type of beast:
				// avatar/soft shot/hard shot/background

  word x, y;			// position
  word ID;			// current image used, ID==off in array

  bool about2Dye;		// beast is dying
  word ox, oy;			// old position - used in unmove

};

//----------------------------------------------------------------------------------

enum AvaConst
{
 GoL0=0,
 GoL1,
 GoL2,
 GoL3,

 GoR0,
 GoR1,
 GoR2,
 GoR3,

 FallL,
 FallR,

 UpL,
 UpR,

 StopL,
 StopR,

 MoveLBeg=0, MoveLng=4,
 MoveRBeg=4
};

class far Avatar : public Beast
{
 public:
  Avatar(int x, int y );
  ~Avatar();

 private:
};

//----------------------------------------------------------------------------------

class far MrZombie : public Beast
{
 public:
  MrZombie(int x, int y, byte color);
  ~MrZombie();

 private:
};

#endif

//- EOF ----------------------------------------------------------------------------
