//----------------------------------------------------------------------------------
//
//                                  Beasts
//
//                                  Dvorka
//                                   1998
//
//----------------------------------------------------------------------------------

#include "beast.h"

#define BEASTDEBUG

extern byte keyBoard[]; // array of keys. If byte is 1, key is down

extern word DBMPw;
extern word DBMPh;

extern byte far  *Original;

extern word      GlobalXLevel,	// upper left corner position in cells
		 GlobalYLevel;  // 1 cell = 10 pixels

//----------------------------------------------------------------------------------

void BeastImg::square(int w, int h, byte color)
{
 #define CHECKINIT(A,B) \
  checkPoints[i++]=A; checkPoints[i++]=B;

 int i=0;

 image=(byte far *)farmalloc(w*h);
 this->w=w;
 this->h=h;

 // checkpoints
 checkLng=4;
 checkPoints=(word far*)farmalloc(checkLng*sizeof(word)*2);
 CHECKINIT(0,0)
 CHECKINIT(w-1,0)
 CHECKINIT(w-1,h-1)
 CHECKINIT(0,h-1)

 // create image of rectangle
 for(i=0; i<w*h; i++) image[i]=color;
 image[0]=image[w*h-1]=image[w-1]=image[w*h-w]= 15;
}

//----------------------------------------------------------------------------------

void BeastImg::save( int handle )
/*
 - one imgRecord:
	BeastImg "image"
	Image   		array of nrOfImageBytes bytes
	Checks  		nrOfCheckpointTuples*2 bytes
*/
{
 word sbytes;

 if (_dos_write(handle, this, sizeof(BeastImg), &sbytes)) { printf("\n Err: unable to write file..."); return; }
 if (_dos_write(handle, image, w*h, &sbytes)) { printf("\n Err: unable to write file..."); return; }
 if(checkLng)
  if (_dos_write(handle, checkPoints, sizeof(word)*checkLng*2, &sbytes)) { printf("\n Err: unable to write file..."); return; }
 printf("\n ImgDescr: size %d w %d h %d checkLng %d", w*h, w, h, checkLng );
}

//----------------------------------------------------------------------------------

void BeastImg::load( int handle )
/*
 - one imgRecord:
	BeastImg "image"
	Image   		array of nrOfImageBytes bytes
	Checks  		nrOfCheckpointTuples*2 bytes
*/
{
 word sbytes;

 if (_dos_read(handle, this, sizeof(BeastImg), &sbytes)) { printf("\n Err: unable to read file..."); return; }

 // alloc space for image
 image=(byte far*)farmalloc(w*h);
 if (_dos_read(handle, image, w*h, &sbytes)) { printf("\n Err: unable to read file..."); return; }

 // checkpoints
 if(checkLng)
 {
  checkPoints=(word far*)farmalloc(checkLng*sizeof(word)*2);
  if (_dos_read(handle, checkPoints, sizeof(word)*checkLng*2, &sbytes)) { printf("\n Err: unable to read file..."); return; }
 }

 printf("\n ImgDescr: size %u w %u h %u checkLng %u", w*h, w, h, checkLng );
}

//----------------------------------------------------------------------------------

void BeastImg::destruct()
{
 if(image)
  farfree(image);

 if(checkPoints)
  farfree(checkPoints);
}

//----------------------------------------------------------------------------------

Beast::Beast()
{
 //printf(" -> ::Beast() constructor");

 this->sizeOfImages=BEAST_IMAGES;

 // allocate images
 images = (BeastImg far*)farmalloc(sizeof(BeastImg)*sizeOfImages);
		#ifdef BEASTDEBUG
		printf(" -> ::Beast, free: %lu",farcoreleft());
		#endif

 this->type=SOFT_SHOT;
 nrOfImages=0;
}

//----------------------------------------------------------------------------------

Beast::Beast( char *fileName )
{
 word sbytes;
 int  handle,
      i;

 if (_dos_open( fileName, O_RDONLY, &handle) != 0) { printf("Err: unable to open file..."); return; }
  if (_dos_read(handle, this, sizeof(Beast), &sbytes) != 0) { printf("Err: unable to read file..."); return ; }

 // now alloc and load images
 images = (BeastImg far*)farmalloc(sizeof(BeastImg)*sizeOfImages);

 printf(" Images:");
 for(i=0; i<nrOfImages; i++)
  images[i].load(handle);

 _dos_close(handle);

}

//----------------------------------------------------------------------------------

Beast::Beast( word sizeOfImages, int type )
{
 //printf(" -> ::Beast constructor");

 this->sizeOfImages=sizeOfImages;

 // allocate images
 images = (BeastImg far*)farmalloc(sizeof(BeastImg)*sizeOfImages);
		#ifdef BEASTDEBUG
		printf(" -> ::Beast, free: %lu",farcoreleft());
		#endif

 this->type=type;
 nrOfImages=0;
}

//----------------------------------------------------------------------------------

Beast::~Beast()
{
 // printf("\n -> ::Beast destructor");

 // destroy active images
 word i;
 for( i=0; i<nrOfImages; i++ )
  images[i].destruct();

 // delete images vector
 farfree(images);
}

//----------------------------------------------------------------------------------

bool Beast::check(byte far *screen)
// - returns TRUE if collision occured else returns FALSE
{
 word j;

 for( j=0; j<(images->checkLng)*2; j+=2 )
 {
   if( screen[x+images->checkPoints[j]+(y+images->checkPoints[j+1])*320] ) // pixel full -> collision
   {
    screen[0]=images->image[2];
    return TRUE;
   }
 }

 return FALSE;
}

//----------------------------------------------------------------------------------
/*
void Beast::move()
{
 // save old position for unmove

 int i=rand();
 if( i%2 ) x++; else x--;

 i=rand();
 if( i%2 ) y++; else y--;

 if(x<20) x=20; if(y<20) y=20;
 if(x>300-images->w) x=300-images->w;
 if(y>180-images->h) y=180-images->h;
}
*/
//----------------------------------------------------------------------------------

void Beast::show( byte far* screen )
{
 word ww,hh;
 word xx=x-GlobalXLevel*10,   // normalize it to screen
      yy=y-GlobalYLevel*10;

 for( ww=0; ww<images[ID].w; ww++ )
  for( hh=0; hh<images[ID].h; hh++ )
   if( images[ID].image[ww+hh*images[ID].w] )
    screen[xx+ww+(yy+hh)*320] = images[ID].image[ww+hh*images[ID].w];
}

//----------------------------------------------------------------------------------

#define JUMP_HEIGHT     10
#define MOVE_STEP	2
#define MAX_FALL_STEP   9

void Beast::move()
// - prohazovani stop obrazku pri padu je obracene
{
 // jump properties
 static bool jumping=FALSE,
	     falling=FALSE;

 static byte jumpStep,		// how fast move when jumping
	     fallStep,          // how fast move when falling
	     moveStep=MOVE_STEP,// how fast move when walking
	     step=0;            // each move increases by one
				// -> e.g. used for image switching
 static bool left;	// last time moved left else moved right



 // save old position
 ox=x; oy=y;


 if( keyBoard[KEYB_DOWN] )
 {
  y+=moveStep;
 }

 if( keyBoard[KEYB_RIGHT] )
 {
  left=FALSE;
  x+=moveStep;
  if( !(step++%7) ) ID=MoveLBeg + step%MoveLng;
 }

 if( keyBoard[KEYB_LEFT] )
 {
  left=TRUE;
  x-=moveStep;
  if( !(step++%7) ) ID=MoveRBeg + step%MoveLng;
 }

 // slow down move when falling or jumping
 if( falling && (step++%3) ) return;
 if( jumping && (step++%3) ) return;

 if( keyBoard[KEYB_UP] )
 {
  word xx=x-GlobalXLevel*10,   // normalize it to screen
       yy=y-GlobalYLevel*10;

  // jump is possible only if staying on something
  if( Original[(yy+images[ID].h)*320+(xx+images[ID].w/2)] )
  {
   jumping=TRUE;
   // change image ID by direction
   if(left) ID=UpR; else ID=UpL;
  }
 }



 // --- NO CEIL - FALL ---
 if( !jumping )
 {
  word xx=x-GlobalXLevel*10,   // normalize it to screen
       yy=y-GlobalYLevel*10;

  // check if avatar is staying on ground, if not - fall
  // taking bottom-middle pixel
  if( Original[(yy+images[ID].h)*320+(xx+images[ID].w/2)]==0 )
  {
   if( !falling )
   {
    falling=TRUE;
    fallStep=1;
   }
   else
    {
     if( fallStep<MAX_FALL_STEP )fallStep++;
    }

    y+=fallStep;

   // check position I jumped in
   // if I am in some floor -> i must be *on* object, never inside ->
   // move up and search for the first pixel transparent - it is ==0
   if( Original[(yy+images[ID].h)*320+(xx+images[ID].w/2)]==0 )
   {
    do
    {
     y--;
     yy=y-GlobalYLevel*10;
    }
    while( Original[(yy+images[ID].h)*320+(xx+images[ID].w/2)] );
    y++; // move to floor (I was one pixel over)
   }
   // it's probably end of fall -> change image ID
   if(left) ID=StopR; else ID=StopL;

  }
  else // end of fall
  {
   // it's end of fall
   falling=FALSE;
  }

 }
 else // I am in jump
  falling=FALSE;



 // JUMP
 // jump bude pohyb pouze nahoru, jakmile dosahnu vrsku -> konec skoku
 // a avatar bude uz sam padat
 // Bude tady flag skakani.
 if( jumping )
 {
  // it's new jump
  if(!jumpStep)
   jumpStep=JUMP_HEIGHT;

  y-=jumpStep--;	// decrease jump

  if(!jumpStep)
  {
   jumping=FALSE;
   ID = FallR;
  }
 }


 // MOVE SCREEN
 // left
 if( x-GlobalXLevel*10 < 50 ) // i am too near border
 {
  if(GlobalXLevel) GlobalXLevel--;
 }
 // right
 if( ((GlobalXLevel+SCREENCOLS)*10)-x < 50 )
 {
  if( GlobalXLevel<(LEVELWDT-SCREENCOLS) ) GlobalXLevel++;
 }
 // up
 if( y-GlobalYLevel*10 < 80 ) // i am too near border
 {
  if(GlobalYLevel) GlobalYLevel--;
 }
 // down
 if( ((GlobalYLevel+SCREENROWS)*10)-y < 80 )
 {
  if( GlobalYLevel<=(LEVELHGT-SCREENROWS+1) ) GlobalYLevel++;
 }

}

//----------------------------------------------------------------------------------

void Beast::unmove()
// is used to return back last move
{
 x=ox; y=oy;
}

//----------------------------------------------------------------------------------

bool Beast::collision( byte far* screen )
{
 if( screen )
  return TRUE;
 else
  return TRUE;
}

//----------------------------------------------------------------------------------

void Beast::addImage( char *BMPFile )
{
 if( nrOfImages>=sizeOfImages )
  printf("\n images vector is full: %u/%u",nrOfImages,sizeOfImages);
 else
 {
  byte far *p=images[nrOfImages].image;
   DBMPLoad( BMPFile, p, DBMPGETDATA );
  images[nrOfImages].image=p;
  // inside DBMPLoad are set globals DBMPw and DBMPh
  images[nrOfImages].w=DBMPw;
  images[nrOfImages].h=DBMPh;
  images[nrOfImages].checkLng=0;
  images[nrOfImages].checkPoints=NULL;
  printf(" %dx%d image, ID %u/%u added...",DBMPw,DBMPh,nrOfImages,sizeOfImages);
  nrOfImages++;
 }
}

//----------------------------------------------------------------------------------

void Beast::save( char *fileName )
/*
 - BeastFile format:
    - class Beast image
    - image records -> number is get from image
*/

{
 word sbytes,
      i;
 int handle;

 if (_dos_creat( fileName, _A_NORMAL, &handle)!=0) { printf(" Error: open file %s...", fileName); return; }
  if (_dos_write(handle, this, sizeof(Beast), &sbytes) != 0) { printf("Err: unable to write beast image..."); return; }

  printf("\n Beast core");

  // save nrOfImages images
  for( i=0; i<nrOfImages; i++ )
   images[i].save(handle);

 _dos_close(handle);

}

//----------------------------------------------------------------------------------

Avatar::Avatar(int x, int y )
{
 this->x=this->ox=x;
 this->y=this->oy=y;

 images[0].square(10,10,WHITE);
 nrOfImages=1;
}

//----------------------------------------------------------------------------------

MrZombie::MrZombie(int x, int y, byte color)
{
 this->x=this->ox=x;
 this->y=this->oy=y;

 images[0].square(10,10,color);
 nrOfImages=1;
}

//- EOF ----------------------------------------------------------------------------

