//---------------------------------------------------------------------------
//
//			   Menu operations in NAAGA editor
//
//                             Dvorka 1997-1998
//
//---------------------------------------------------------------------------


/*
 - REGISTER/REPLACE/... je stejne => do jedne funkce
 - vybirani obdelniku u importu, block operaci jde dat do funkce
*/


#include "image.h"

#define DRAW_MENU movedata( FP_SEG(GameMenu), FP_OFF(GameMenu),	\
			    0xA000, 320*JAILROWS*10, 320*20);


extern Jail      jail;
extern Mouse     mouse;
extern KrsMenu	 *PDrawMenu;
extern word far  *LevelArray;

extern byte far  *ComponentArray;
extern word       ComponentIndex;
extern byte far  *Background;
extern byte far  *GameMenu;

extern word      GlobalXLevel,
		 GlobalYLevel,

		 ListIndex=0;   // last index in component array

extern byte Palette[256][3];
extern byte MenuHidden;
extern byte far  *Original;

extern byte BgLayer; 		// true if background layer visible
extern byte FgLayer;            // true if foreground layer visible
extern word Layer;              // 0 if bg layer active, 16000 if fg layer active
				// it is off of layer in level array
extern byte far *ScreenPtr;


char file[50];		        // filename for I/O operations
byte clipboard[646]; 		// clipboard for block paste



//---------------------------------------------------------------------------


KrsImage::KrsImage( char *iFileName, word iX, word iY,
		    word iWidth, word iHeight )
{
 FileName=iFileName;
 X=iX; Y=iY; Width=iWidth; Height=iHeight;
 ByteSize=imagesize( X, Y, X+Width, Y+Height );
 WriteMode=COPY_PUT;

 if( (MemAddr=(void far*)farmalloc(ByteSize))==NULL) { beep(); setcolor(WHITE); outtextxy(50,50,"Err: image - not enougth memory... ");}
 if(MemAddr!=NULL) InMem=TRUE;
}


KrsImage::KrsImage( char *iFileName, word iX, word iY, word iByteSize )
{
 FileName=iFileName;
 X=iX; Y=iY; Width=0; Height=0; // !!! chtelo by to !!!
 ByteSize=iByteSize;
 WriteMode=COPY_PUT;

 if( (MemAddr=(void far*)farmalloc(ByteSize))==NULL) { beep(); setcolor(WHITE); outtextxy(50,50,"Err: image - not enougth memory... ");}
 if(MemAddr!=NULL) InMem=TRUE;
}


KrsImage::~KrsImage()
{
 if( InMem ) farfree(MemAddr);
}


void KrsImage::Load( void )
{
 if( !InMem )
 {
  if( (MemAddr=(void far*)farmalloc(ByteSize))==NULL)
  {
   setcolor(WHITE);
   outtextxy(50,50,"Err: image - not enougth memory... ");
   return;
  }
  else
   InMem=TRUE;
 }

 int handle;
 word bytes;

 if (_dos_open(FileName, O_RDONLY, &handle) != 0)
 { perror("Err: image - unable to open infile..."); }

 if (_dos_read(handle, MemAddr, ByteSize, &bytes) != 0)
 { perror("Err: image - unable to read file..."); }

 _dos_close(handle);
}


void KrsImage::Save( void )
{
 if( !InMem )
 {
  if( (MemAddr=(void far*)farmalloc(ByteSize))==NULL)
  {
   setcolor(WHITE);
   outtextxy(50,50,"Err: image - not enougth memory... ");
   return;
  }
  else
   InMem=TRUE;
 }

 int handle;
 word bytes;

 if (_dos_open(FileName, O_RDONLY, &handle) != 0)
 { perror("Err: image - unable to open outfile..."); }

 if (_dos_write(handle, MemAddr, ByteSize, &bytes) != 0)
 { perror("Err: image - unable to save file..."); }

 _dos_close(handle);
}


void KrsImage::Get( void )
{
 if( InMem ) getimage(X, Y, X+Width-1, Y+Height-1, MemAddr );
 else { setcolor(WHITE); outtextxy(50,50,"Err: image - no get memory... ");}
}


void KrsImage::Show( void )
{
 if( InMem )  putimage(X, Y, MemAddr, WriteMode);
 else { setcolor(WHITE); outtextxy(50,50,"Err: image - nothing to put... ");}
}


void KrsImage::Show( word x, word y)
{
 if( InMem ) putimage(x, y, MemAddr, WriteMode);
 else { setcolor(WHITE); outtextxy(50,50,"Err: image - nothing to put... ");}
}


void KrsImage::Free( void )
{
 if( InMem ) farfree(MemAddr);
 InMem=FALSE;
}


//-- KrsMenu ------------------------------------------------------------------


#define CMD_RECT_WIDTH 10
#define CMD_LEFT       30

KrsMenu::KrsMenu( Mouse *imouse, word iX, word iY )
{
 X=iX; Y=iY; Width=320; Height=60;
 FaceFileName=strdup("menu.krs");
 UnderFileName=strdup("under.krs");
 MenuByteSize=19206;
 Under = new KrsImage( UnderFileName, X, Y, Width, Height );
 Under->Get();   // don't save only get what's under
 Face = new KrsImage( FaceFileName, X, Y, MenuByteSize);
 Face->Load();
 mouse=imouse;
 Visible=FALSE;
 CellClipBoard=0;
 TextCol=99;
}


KrsMenu::~KrsMenu()
{
 free(FaceFileName);
 free(UnderFileName);
 delete Face;
 delete Under;
}


void KrsMenu::Show( int ActuallColor )
{
 if( !Visible )
 {
  mouse->Hide();
   Under->Get();
   Face->Show();
   Visible=TRUE;

   UpdateMousePos();
   UpdateActuallColor( ActuallColor );
   Message("");
  mouse->Show();
 }
}

void KrsMenu::Hide( void )
{
 if( Visible )
 {
  mouse->Hide();
   Under->Show();
  mouse->Show();
 }
 Visible=FALSE;
}


void KrsMenu::Move( int ActuallColor )
{
 // rectangle which size is the same as menu size is XOR moving,
 // move only up & down
 word oldy=Y;
 word shift=X-mouse->PosX(), oldx=X;

 shift=0; oldx=0;

 Hide();
 mouse->Hide();

 setcolor( jail.XORcolor );
 setwritemode(XOR_PUT);
 rectangle(X+shift,Y,X+Width-1+shift,Y+Height-1);  // show moving rectangle

 do
 {
  oldy=mouse->PosY();
  oldx=0; // mouse->PosX();
  if( oldx != X || oldy != Y )
  {
   rectangle(X+shift,Y,X+Width-1+shift,Y+Height-1);
   X=oldx; Y=oldy;
   rectangle(X+shift,Y,X+Width-1+shift,Y+Height-1);
  }
 }
 while( !(mouse->LeftEar() || mouse->RightEar()) );
 rectangle(X+shift,Y,X+Width-1+shift,Y+Height-1); // hide moving rectangle
 setwritemode(COPY_PUT);

 //Under->X = X; Face->X = X;
 Under->Y = Y; Face->Y = Y;

 Show( ActuallColor );
 mouse->Show();
}


void KrsMenu::UpdateMousePos( void )
{
 if( Visible )
 {
  static OldX, OldY;
  word ux=X+202, uy=Y+11, bcol=BLACK;
  int  x, y;
  char   StringX[20],
         StringY[20];

  mouse->Pos( x, y );
  if ( OldX != x )
   {
    itoa( x,StringX,10 );
    mouse->Hide();
     setfillstyle( SOLID_FILL,bcol );
     bar( ux,uy,ux+25,uy+9 );
     setcolor( TextCol ); outtextxy( ux,uy+2,StringX ); OldX = x;
    mouse->Show();
   }
   if ( OldY != y )
    {
     itoa( y,StringY,10 );
     mouse->Hide();
      setfillstyle( SOLID_FILL,bcol );
      bar( ux+45,uy,ux+100-28,uy+9 );
      setcolor( TextCol ); outtextxy( ux+45,uy+2,StringY ); OldY = y;
     mouse->Show();
    }
 }
}


//-----------------------------------------------------------------------------


void CellToScreen( word oldx, word oldy, word index )
{
 word i;

 for(i=0; i<100; i++ )
 {
  ScreenPtr[oldy*320+oldx+((i/10)*320+i%10)]
  =
  ComponentArray[index*100+i];
 }
}


void ScreenToCell( word oldx, word oldy, word index )
{
 word i;

 for(i=0; i<100; i++ )
 {
  ComponentArray[index*100+i]
  =
  ScreenPtr[oldy*320+oldx+((i/10)*320+i%10)];
 }
}


//-----------------------------------------------------------------------------


int KrsMenu::MouseLeftEar( int x, int y, int &ExitApplication,
			   byte ActuallColor, word direct )
// direct means that switch should be used,

{
 int  handle, end=FALSE;
 word sbytes;
 char info[]={"File: "}, String[35]; String[0]=0;
 char importfile[50];
 word i, xx, yy, index, k, ix, iy;
 word left, right, top, bottom;
 word oldindex;
 word far *Array;

 ThreeButtonWnd *wokno, *wwokno;


 if( Visible || (direct&0x8080))  // some drawing/system action
 {
  word action=0;
  if( (x>X+CMD_LEFT && x<X+191)||(direct&0x8080)) // in command menu rectangle
  {
  if( (y>Y+4 && y<Y+14)||(direct&0x8000))
  {
   x-=(X+CMD_LEFT);
   action=x/CMD_RECT_WIDTH;

   if( (direct&0x8000) ) action=(direct>>8)&0x7F;// if highest bit is set
						 // some action in upper menu

   switch( action )
   {
    case 0: // load

	    file[0]=0;
	    if( !ReadString(info,file) ) { Message("Canceled..."); break; }
	    Hide(); mouse->Hide();

	    if (_dos_open( file, O_RDONLY, &handle) != 0) { Show( ActuallColor ); Message("Err: unable to open file..."); return ACTION_DONE; }
	     if (_dos_read(handle, &ComponentIndex, sizeof(word), &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; } // last index in component
	     if (_dos_read(handle, &ListIndex, sizeof(word), &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to write file..."); return ACTION_DONE; }
				   ListIndex%=640;
	     if (_dos_read(handle, LevelArray, LEVELSIZEBYTE, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_read(handle, ComponentArray, 64000u, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_read(handle, Background, 64000u, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_read(handle, Palette, 256*3, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_read(handle, GameMenu, 20*320, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; }
	    _dos_close(handle);

	    Show( ActuallColor ); Message("Loaded...");

	    Hide();
	     GFXSetPal();
	     ShowLevelCells();
	     DRAW_MENU
	    Show(ActuallColor);

	    return ACTION_DONE;

    case 1: // save

	    GFXGetPal();

	    if( !ReadString(info,file) ) { Message("Canceled..."); break; }

	    if (_dos_creat(file, _A_NORMAL, &handle) != 0) { Show( ActuallColor ); Message("Err: unable to open file..."); return ACTION_DONE; }
	     if (_dos_write(handle, &ComponentIndex, sizeof(word), &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to write file..."); return ACTION_DONE; }
				    ListIndex%=640;
	     if (_dos_write(handle, &ListIndex, sizeof(word), &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to write file..."); return ACTION_DONE; }
	     if (_dos_write(handle, LevelArray, LEVELSIZEBYTE, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to write file..."); return ACTION_DONE; }
	     if (_dos_write(handle, ComponentArray, 64000u, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_write(handle, Background, 64000u, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_write(handle, Palette, 256*3, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_write(handle, GameMenu, 20*320, &sbytes) != 0) { Show( ActuallColor ); Message("Err: unable to read file..."); return ACTION_DONE; }
	    _dos_close(handle);

	    Message("Saved...");
	    return ACTION_DONE;

    case 2: // quick save

	    if( file[0]!=0 )
	    {
	     if (_dos_open( file, O_RDWR, &handle) != 0) { Show( ActuallColor ); Message("Err: open file..."); return ACTION_DONE; }

	     if (_dos_write(handle, &ComponentIndex, sizeof(word), &sbytes)) { Show( ActuallColor ); Message("1Err: unable to write file..."); return ACTION_DONE; }
				    ListIndex%=640;
	     if (_dos_write(handle, &ListIndex, sizeof(word), &sbytes)) { Show( ActuallColor ); Message("2Err: unable to write file..."); return ACTION_DONE; }
	     if (_dos_write(handle, LevelArray, LEVELSIZEBYTE, &sbytes)) { Show( ActuallColor ); Message("3Err: unable to write file..."); return ACTION_DONE; }
	     if (_dos_write(handle, ComponentArray, 64000u, &sbytes)) { Show( ActuallColor ); Message("4Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_write(handle, Background, 64000u, &sbytes)) { Show( ActuallColor ); Message("5Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_write(handle, Palette, 256*3, &sbytes)) { Show( ActuallColor ); Message("6Err: unable to read file..."); return ACTION_DONE; }
	     if (_dos_write(handle, GameMenu, 20*320, &sbytes)) { Show( ActuallColor ); Message("7Err: unable to read file..."); return ACTION_DONE; }

	     _dos_close(handle);

	     if( !MenuHidden )
	     {
	      Show( ActuallColor );
	      xx=strlen(file); strcat(file," quicksaved...");
	      Message(file);
	      file[xx]=0;
	     }
	    }
	    else
	     Message("No filename...");

	    return ACTION_DONE;

    case 3: // cell import

	    importfile[0]=0;
	    if( !ReadString(info,importfile) ) break;

	    Hide();
	    cleardevice();

	    mouse->Hide();
	     DBMPLoad(importfile, CELLIMPORT);
	    mouse->Show();

	    // select rectangle to copy
	    setwritemode( XOR_PUT );
	    setcolor( jail.XORcolor );

	       left=right=mouse->PosX();
	       top=bottom=mouse->PosY();

	       do
	       {
		if( !(((x=mouse->PosX())==right)&&((y=mouse->PosY())==bottom)) )
		{
		 mouse->Hide();
		  rectangle((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10);
		   right=x; bottom=y;
		  rectangle((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10);
		 mouse->Show();
		}

		if( mouse->LeftEar() )
		{
		  do
		  {
		   if( !(((x=mouse->PosX())==right)&&((y=mouse->PosY())==bottom)) )
		   {
		    mouse->Hide();
		     rectangle((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10);
		     x=mouse->PosX(); y=mouse->PosY();
		     left+=(x-right); right=x;
		     top+=(y-bottom); bottom=y;
		     rectangle((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10);
		    mouse->Show();
		   }
		  }
		  while( mouse->LeftEar() );
		}
	       }
	       while( !mouse->RightEar() );
	       mouse->Break();

	    setwritemode( COPY_PUT );

	    mouse->Hide();
	     getimage((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10, Original );
	    mouse->Show();

	    // ix is upper left, iy is upper left
	    // xx is bottom right, yy is bottom right
	    ix=left;
	    iy=top;
	    xx=right;
	    yy=bottom;

	    // save rectangle
	    jail.HideActive();
	    ShowLevelCells();
	    mouse->Hide();
	     DRAW_MENU
	    mouse->Show();

	    setwritemode( XOR_PUT );

	    // xx will be width
	    if( xx>ix ) xx=(xx/10)*10-(ix/10)*10; else xx=(ix/10)*10-(xx/10)*10;
	    if( yy>iy ) yy=(yy/10)*10-(iy/10)*10; else yy=(iy/10)*10-(yy/10)*10;

	    ix=mouse->PosX(); iy=mouse->PosY();
	    rectangle((ix/10)*10, (iy/10)*10, ((ix+xx)/10)*10, ((iy+yy)/10)*10 );

	    end=FALSE;
	    mouse->Break();

	    while( !end )
	    {
	     if( mouse->PosX()!=ix || mouse->PosY()!=iy )
	     {
	      mouse->Hide();
	       rectangle((ix/10)*10, (iy/10)*10, ((ix+xx)/10)*10, ((iy+yy)/10)*10 );
	       ix=mouse->PosX(); iy=mouse->PosY();
	       rectangle((ix/10)*10, (iy/10)*10, ((ix+xx)/10)*10, ((iy+yy)/10)*10 );
	      mouse->Show();
	     }

	     if( mouse->LeftEar() )
	     {
	      mouse->Hide();
	       rectangle((ix/10)*10, (iy/10)*10, ((ix+xx)/10)*10, ((iy+yy)/10)*10 );
	       setwritemode( COPY_PUT );
	       putimage((ix/10)*10, (iy/10)*10, Original, COPY_PUT );
	      mouse->Show();
	      end=TRUE;
	     }
	    }

	    // ix is in pixels upper left, iy yes
	    // xx, yy width height in pixels
	    ix/=10; x=ix;
	    iy/=10; y=iy;
	    xx=ix+xx/10;
	    yy=iy+yy/10;

	    mouse->Hide();

	    for( ix=x; ix<xx; ix++ )
	     for( iy=y; iy<yy; iy++ )
	     {
	      LevelArray[(GlobalYLevel+iy)*LEVELWDT+GlobalXLevel+ix+Layer]
	      =
	      ComponentIndex;

	      ScreenToCell( ix*10, iy*10, ComponentIndex );

	      ComponentIndex++;
	     }


	    jail.HideActive();
	    ShowLevelCells();
	    mouse->Hide();
	     DRAW_MENU;
	    mouse->Show();

	    return ACTION_DONE;

    case 4: // copy

	    CellClipBoard
	    =
	    LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];
	    Message("Cell copied to clip...");
	    return ACTION_DONE;

    case 5:// paste

	    // fill rectangle with current cell
	    CellToScreen( jail.oldx, jail.oldy, CellClipBoard );

	    // set new index in level array
	    LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer]
	    =
	    CellClipBoard;

	    Message("Cell pasted from clip...");
	    return ACTION_DONE;

    case 6: // block copy
	    Hide();

	    // select rectangle to copy
	    setwritemode( XOR_PUT );
	    setcolor( jail.XORcolor );

	       left=right=mouse->PosX();
	       top=bottom=mouse->PosY();

	       do
	       {
		if( !(((x=mouse->PosX())==right)&&((y=mouse->PosY())==bottom)) )
		{
		 mouse->Hide();
		  rectangle((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10);
		   right=x; bottom=y;
		  rectangle((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10);
		 mouse->Show();
		}

		if( mouse->LeftEar() )
		{
		  do
		  {
		   if( !(((x=mouse->PosX())==right)&&((y=mouse->PosY())==bottom)) )
		   {
		    mouse->Hide();
		     rectangle((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10);
		     x=mouse->PosX(); y=mouse->PosY();
		     left+=(x-right); right=x;
		     top+=(y-bottom); bottom=y;
		     rectangle((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10);
		    mouse->Show();
		   }
		  }
		  while( mouse->LeftEar() );
		}
	       }
	       while( !mouse->RightEar() );
	       mouse->Break();

	    // clean...
	    rectangle((left/10)*10,(top/10)*10,(right/10)*10,(bottom/10)*10);
	    setwritemode( COPY_PUT );

	    // rectangle selected, now copy numbers and size to buffer
	    // 32*20 words => 640 bytes + 2+2 bytes for size => 644 bytes
	    // => static array, first two bytes width, height rest data

	    if( right>left )
	    {
	     x=left/10;
	     right/=10;
	    }
	    else
	    {
	     x=right/10;
	     right=left/10;
	    }
	    clipboard[0]=right-x; 	// save width to right

	    if( top>bottom )
	    {
	     y=bottom/10;
	     top/=10;
	    }
	    else
	    {
	     y=top/10;
	     top=bottom/10;
	    }
	    clipboard[1]=top-y;		// save height to top

					// fill clipboard with indexes
	    xx=x;
	    yy=y;

	    i=2; 			// points to free in clipboard

	    for( x=xx; x<right; x++ )
	     for( y=yy; y<top; y++ )
	     {
	      clipboard[i++]
	      =
	      LevelArray[(GlobalYLevel+y)*LEVELWDT+GlobalXLevel+x+Layer];
	     }

	    Show( ActuallColor );

	    return ACTION_DONE;

    case 7: // block paste

	    Hide();
	    jail.HideActive();

	    setwritemode( XOR_PUT );

	    ix=mouse->PosX(); iy=mouse->PosY();
	    xx=clipboard[0]*10;
	    yy=clipboard[1]*10;

	    rectangle((ix/10)*10, (iy/10)*10, ((ix+xx)/10)*10, ((iy+yy)/10)*10 );

	    end=FALSE;
	    mouse->Break();

	    while( !end )
	    {
	     if( mouse->PosX()!=ix || mouse->PosY()!=iy )
	     {
	      mouse->Hide();
	       rectangle((ix/10)*10, (iy/10)*10, ((ix+xx)/10)*10, ((iy+yy)/10)*10 );
	       ix=mouse->PosX(); iy=mouse->PosY();
	       rectangle((ix/10)*10, (iy/10)*10, ((ix+xx)/10)*10, ((iy+yy)/10)*10 );
	      mouse->Show();
	     }

	     if( mouse->LeftEar() )
	     {
	      mouse->Hide();
	       rectangle((ix/10)*10, (iy/10)*10, ((ix+xx)/10)*10, ((iy+yy)/10)*10 );
	       setwritemode( COPY_PUT );
	      mouse->Show();
	      end=TRUE;
	     }
	    }

	    right=((ix+xx)/10);
	    top=((iy+yy)/10);
	    xx=x=ix/10;
	    yy=y=iy/10;

	    i=2;

	    for( x=xx; x<right; x++ )
	    {
	     for( y=yy; y<top; y++ )
	     {
	      LevelArray[(GlobalYLevel+y)*LEVELWDT+GlobalXLevel+x+Layer]
	      =
	      clipboard[i++];
	     }
	    }

	    ShowLevelCells();
	    DRAW_MENU;
	    Show( ActuallColor );

	    return ACTION_DONE;

    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14: // optimize current level file
	     /*
	      - frees cells from component array which are not used
		in level array
	      - algorithm
		- goes through level array and counts cells used
		  in level. Making twins

		   array[k]=k         // when cell nr. k is in level array
				      // k is index from component array
		   array[k]=FFFF      // initialization in searching array
				      // means array nr. k is not here
		- sort array
		   0,1,3,5,...27,FFFF,FFFF,FFFF,...
		  and k is on new indexes

		- make level array and component array transformation
		  go through component array and for each index do

		  i=array[counter] // i old index, counter new index

		  level cycle c
		  {
		   if( level[c]==i ) level[c]=counter;
		  }

		  // move in component array
		  cycle from 0 to 99 j
		  component[counter*100+j]=component[i*100+j]

		  // we move compononent cells content left => OK
	     */

	    // cast
	    Array=(word far *)Original;

	    // initialize
	    for( i=0; i<32000; i++ ) Array[i]=0xFFFF;

	    // count in back level array
	    for( i=0; i<32000; i++ ) // level cells; ++ in words not bytes
	     Array[LevelArray[i]]=LevelArray[i];

	    // sort original
	    ShakerSort( Array, 32000 );

	    // make transformation
	    for( i=0; i<640; i++ )      // through component
	    {
	     oldindex=Array[i];    	// i is new index

	     if( oldindex!=0xFFFF ) 	// if is not FFFF
	     {
	      for( k=0; k<32000; k++ )  // reindexing level array => through
	       if( LevelArray[k]==oldindex )
		LevelArray[k]=i;

	      for( k=0; k<100; k++ )    // move cell in component
	       ComponentArray[i*100+k]=ComponentArray[oldindex*100+k];

	      ComponentIndex=i+1;
	     }
	     else
	     {
	      for( k=0; k<100; k++ )    // fill with some color
	       ComponentArray[i*100+k]=116;
	     }

	     switch( i%4 )
	     {
	      case 0:
			PDrawMenu->Message("-", FALSE );
			break;
	      case 1:
			PDrawMenu->Message("\\", FALSE );
			break;
	      case 2:
			PDrawMenu->Message("|", FALSE );
			break;
	      case 3:
			PDrawMenu->Message("/", FALSE );
			break;
	     }

	    } 	// for

	    Hide();
	    ShowLevelCells();
	    Show(ActuallColor);

	    itoa( ComponentIndex, String, 10 );
	    strcat(String, " cells in level now...");
	    Message(String);

	    return ACTION_DONE;

    case 15: // exit editor

	    ExitApplication=TRUE;
	    Message("Exiting program ...");
	    return MAXINT;

    default: break;
   } // switch
  } // if ... upper y position
  else // another y
  {
  if( (y>=Y+14 && y<Y+24)||(direct&0x80))
  {
   x-=(X+CMD_LEFT);
   action=x/CMD_RECT_WIDTH;

   if( (direct&0x80) )     // if highest bit in lower byte
   {                         // some action in lower menu
    action=direct&0x007F;
   }

   switch( action )
   {
    case 0: // background load

	    importfile[0]=0;
	    if( !ReadString(info,importfile) ) break;

	    Hide();mouse->Hide();
	     DBMPLoad( importfile, BACKGROUND );
	     ShowLevelCells();
	    Show( ActuallColor );

	    Message("BMP file loaded");
	    return ACTION_DONE;

    case 1: // game menu load

	    importfile[0]=0;
	    if( !ReadString(info,importfile) ) break;

	    Hide();mouse->Hide();
	     DBMPLoad( importfile, GAMEMENU );
	     DRAW_MENU 		// show it
	    Show( ActuallColor );

	    Message("BMP file loaded");
	    return ACTION_DONE;

    case 2: // XOR color change

	    Message("Exit with ESC");
	    KeybClear();
	    mouse->Break();

	    while( !end )
	    {
	     UpdateMousePos();

	     if( mouse->RightEar() ) // choosing color
	     {
	      int xx=mouse->PosX(), yy=mouse->PosY();
	      mouse->Hide();
	       ActuallColor=getpixel( xx, yy );
	       UpdateActuallColor( ActuallColor );
	       jail.HideActive();
	       jail.XORcolor=ActuallColor;
	       jail.ShowActive();
	      mouse->Show();
	     }

	     if( kbhit() )
	     {
	      if( getch()==27 ) end=TRUE;
	     }
	    }

	    Message("OK");

	    return ACTION_DONE;

    case 3: // draw to cell

	    // show old without background
	    Hide();

	    mouse->SetCursor(mouse->PEN);

	    // show old component on place of actuall cell
	    index=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];

	    for(i=0; i<100; i++ )
	    {
	     ScreenPtr[jail.oldy*320+jail.oldx+((i/10)*320+i%10)]
	     =
	     ComponentArray[index*100+i];
	    }

	    Show( ActuallColor );
	    Message("Exit ESC, arrows moves...");
	    KeybClear();
	    mouse->Break();

	    while( !end )
	    {
	     UpdateMousePos();

	     if( mouse->LeftEar() ) // draw only to active cell
	     {
	      int xx=mouse->PosX(), yy=mouse->PosY();

	      if( xx>=jail.oldx && xx<=(jail.oldx+9) &&
		  yy>=jail.oldy && yy<=(jail.oldy+9) )
	      {
	       mouse->Hide();
		putpixel( xx, yy, ActuallColor );
	       mouse->Show();
	      }
	     }

	     if( mouse->RightEar() ) // choosing color
	     {
	      int xx=mouse->PosX(), yy=mouse->PosY();
	      mouse->Hide();
	       ActuallColor=getpixel( xx, yy );
	       UpdateActuallColor( ActuallColor );
	      mouse->Show();
	     }

	     if( kbhit() )
	      if( getch()==27 ) end=TRUE;

	    }

	    KeybClear();
	    mouse->SetCursor(mouse->ARROW);

	    wokno=new ThreeButtonWnd( 30, 60, "Cell changed so...", "Register", "Replace", "Discard" );

	    wokno->Draw();
	     switch( wokno->Choosen() )
	     {
	      case DISCARD:
			   Hide(); delete wokno;
			   // show old component on place of actuall cell
			   index=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];

			   CellToScreen( jail.oldx, jail.oldy, index );

			   break;

	      case REPLACE:
			   // replace pixels act in component with actual cell
			   delete wokno; Hide();

			   mouse->Hide();

			   // get index of actuall
			   index=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];

			   ScreenToCell( jail.oldx, jail.oldy, index );

			   break;

	      case REGISTER:
			   delete wokno; Hide();
			   // copy actuall from screen to end of component

			   // get index of actuall

			   ScreenToCell( jail.oldx, jail.oldy, ComponentIndex );

			   // set new index
			   LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer]
			   =
			   ComponentIndex;

			   ComponentIndex++;

			   break;
	     }

	    jail.HideActive();
	    Hide();
	     ShowLevelCells();
	    Show(ActuallColor);

	    return ACTION_DONE;

    case 4: // list through component array

	    Hide();

	    // index in level array => index of cell on screen
	    oldindex=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];
	    // index in component array
	    index=ListIndex;

	    if( jail.oldy> getmaxy()/2 )
	     wokno=new ThreeButtonWnd( 30, 10, "List through cells...", "Last", "Next", "OK" );
	    else
	     wokno=new ThreeButtonWnd( 30, 120, "List through cells...", "Last", "Next", "OK" );
	    wokno->Draw();

	    while( !end )
	    {

	     switch( wokno->Choosen() )
	     {
	      case NEXT: // take next and draw to actuall cell
			index++; index%=640;// rotate round and round in array

			CellToScreen( jail.oldx, jail.oldy, index );

			// set new index in level array
			LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer]
			=
			index;

			break;
	      case LAST: // take last and draw to actuall cell
			if( index ) index--;

			CellToScreen( jail.oldx, jail.oldy, index );

			// set new index in level array
			LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer]
			=
			index;

			break;

	      case OK  :
			ListIndex=index;

			end=TRUE;
			break;

	      case FALSE:
			LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer]
			=
			oldindex;

			CellToScreen( jail.oldx, jail.oldy, oldindex );

			end=TRUE;
			break;

	     }
	    }

	    delete wokno;
	    Show(ActuallColor);
	    break;

    case 5: // fill current cell with current color

	    Hide();

	    for(i=0; i<100; i++ ) // fill rectangle with current cell
	    {
	     ScreenPtr[jail.oldy*320+jail.oldx+((i/10)*320+i%10)]
	     =
	     ActuallColor;
	    }
	    Show(ActuallColor);


	    KeybClear();

	    wokno=new ThreeButtonWnd( 30, 60, "Cell changed so...", "Register", "Replace", "Discard" );
	    wokno->Draw();
	     switch( wokno->Choosen() )
	     {
	      case DISCARD:
			   Hide(); delete wokno;
			   // show old component on place of actuall cell
			   index=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];

			   CellToScreen( jail.oldx, jail.oldy, index );

			   break;

	      case REPLACE:

			   delete wokno; Hide();
			   // replace pixels act in component with actual cell

			   // get index of actuall
			   index=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];

			   ScreenToCell( jail.oldx, jail.oldy, index );

			   break;

	      case REGISTER:
			   delete wokno; Hide();
			   // copy actuall from screen to end of component

			   // get index of actuall
			   ScreenToCell( jail.oldx, jail.oldy, ComponentIndex );

			   // set new index
			   LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer]
			   =
			   ComponentIndex;

			   ComponentIndex++;

			   break;
	     }

	    jail.HideActive();
	    ShowLevelCells();
	    Show(ActuallColor);

	    return ACTION_DONE;

    case 6: // mirror cell

	    Hide();

	    wwokno=new ThreeButtonWnd( 30, 60, "Mirror type",
				       "Horiz.", "Vertic.", "CANCEL" );
				   //  "Register", "Replace", "Discard" );
	    wwokno->Draw();
	     switch( wwokno->Choosen() )
	     {
	      case VERTICALY:

			   delete wwokno;

			   index=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];

			   for(i=0; i<100; i++ )
			   {
			    xx=9-i%10;
			    yy=i/10;

			    ScreenPtr[jail.oldy*320+jail.oldx+(yy*320+xx)]
			    =
			    ComponentArray[index*100+(99-i)];
			   }

			   break;


	      case HORIZONTALY:

			   delete wwokno;

			   index=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];

			   for(i=0; i<100; i++ )
			   {
			    xx=9-i%10;		// mirror horizontaly
			    yy=i/10;

			    ScreenPtr[jail.oldy*320+jail.oldx+(yy*320+xx)]
			    =
			    ComponentArray[index*100+i];
			   }

			   break;

	      case DISCARD:
			   delete wwokno;
			   Show(ActuallColor);

			   return ACTION_DONE;
	     }


	    // register / replace / discard
	    wokno=new ThreeButtonWnd( 30, 60, "Cell changed so...", "Register", "Replace", "Discard" );

	    wokno->Draw();
	     switch( wokno->Choosen() )
	     {
	      case DISCARD:
			   Hide(); delete wokno;
			   // show old component on place of actuall cell
			   index=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];

			   CellToScreen( jail.oldx, jail.oldy, index );

			   return ACTION_DONE;

	      case REPLACE:
			   // replace pixels act in component with actual cell
			   delete wokno;
			   Hide();
			   mouse->Hide();

			   // get index of actuall
			   index=LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer];

			   ScreenToCell( jail.oldx, jail.oldy, index );

			   jail.HideActive();
			   ShowLevelCells();
			   mouse->Show();

			   Show(ActuallColor);
			   return ACTION_DONE;

	      case REGISTER:
			   delete wokno; Hide();
			   // copy actuall from screen to end of component

			   // get index of actuall
			   ScreenToCell( jail.oldx, jail.oldy, ComponentIndex );

			   // set new index
			   LevelArray[(GlobalYLevel+jail.oldy/10)*LEVELWDT+GlobalXLevel+jail.oldx/10+Layer]
			   =
			   ComponentIndex;

			   ComponentIndex++;

			   jail.HideActive();
			   ShowLevelCells();

			   Show(ActuallColor);

			   return ACTION_DONE;
	     }

	    Show(ActuallColor);

	    return ACTION_DONE;


    case 7: // toggle FgLayer
	   Hide();

	   FgLayer= !FgLayer;

	   if( FgLayer )
	    Message("Fg ON");
	   else
	    Message("Fg OFF");

	   mouse->Break();
	   jail.HideActive();
	   ShowLevelCells();

	   return ACTION_DONE;
    case 8: // toggle BgLayer
	   Hide();

	   BgLayer= !BgLayer;

	   if( BgLayer )
	    Message("Bg ON");
	   else
	    Message("Bg OFF");

	   mouse->Break();
	   jail.HideActive();
	   ShowLevelCells();

	   return ACTION_DONE;
    case 9:
	   if( !Layer )
	   {
	    Layer=16000;
	    Message("FG layer active...");
	   }
	   else
	   {
	    Layer=0;
	    Message("BG layer active...");
	   }

	   mouse->Break();

	   return ACTION_DONE;

    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
	    Message("Feature not implemented");
	    return ACTION_DONE;
    case 15:
	    Message("Click area with right button");
	    return ACTION_DONE;

    default: break;
   } // switch
   } // if lower y position
   else
   {
    if( x>=X && y>=Y && x<=(X+Width) && y<=(Y+Height) )
    {
     mouse->Break();
     Move( ActuallColor );
     mouse->Break();
     return MOVE_MENU;
    }
    else
     return FALSE;
   }
   } // else another y
  } // if x position
  else
  {
   if( x>=X && y>=Y && x<=(X+Width) && y<=(Y+Height) )
   {
    mouse->Break();
    Move( ActuallColor );
    mouse->Break();
    return MOVE_MENU;
   }
   else
    return FALSE;
  }
 }
 return FALSE;
}


int KrsMenu::MouseRightEar( int x, int y, byte &ActuallColor )
{
 if( Visible ) // getting color from menu if visible
 {
  word action=0;
  if( x>X+CMD_LEFT && x<X+191 )
  {
  if( y>Y+4 && y<Y+14 ) 		// upper menu rectangle
  {
   x-=(X+CMD_LEFT);                 	// y-=(Y+27);
   action=x/CMD_RECT_WIDTH;
   switch( action )
   {
    case 0: Message("Load level to *.lvl");
	    break;
    case 1: Message("Save level to *.lvl");
	    break;
    case 2: Message("QuickSave current level");
	    break;
    case 3: Message("Import cell");
	    break;
    case 4: Message("Copy from current cell");
	    break;
    case 5: Message("Paste to current cell");
	    break;
    case 6: Message("Block copy");
	    break;
    case 7: Message("Block paste");
	    break;
    case 8: Message("Block move");
	    break;
    case 9:
    case 10:
    case 11:
    case 12: Message("");
	     break;
    case 13: Message("Back Normal/Symetric/Big");
	     break;
    case 14: Message("Optimize current level");
	     break;
    case 15: Message("Quit editor");
	     break;
    default: break;
   }
  }
  else
  {
   if( y>=Y+14 && y<Y+24 ) 		// lower menu rectangle
   {
    x-=(X+CMD_LEFT);                 	// y-=(Y+27);
    action=x/CMD_RECT_WIDTH;
    switch( action )
    {
    case 0: Message("Load background: *.bmp");
	    break;
    case 1: Message("Load game menu: *.bmp");
	    break;
    case 2: Message("Choose XOR color");
	    break;
    case 3: Message("Put pixels to active cell");
	    break;
    case 4: Message("List through cells");
	    break;
    case 5: Message("Fill cell with current col.");
	    break;
    case 6: Message("Mirror h/v current cell");
	    break;
    case 7: Message("Toggle FG layer visibility");
	    break;
    case 8: Message("Toggle BG layer visibility");
	    break;
    case 9: Message("Toggle active layer FG/BG");
	    break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
	     Message("");
	     break;

    case 15: Message("Help");
	     break;
    default: break;
    }
   }// if lower y
   else // x
   {
    mouse->Hide();
     ActuallColor=getpixel( x, y );
     UpdateActuallColor( ActuallColor );
     Message("Color choosen.");
    mouse->Show();
    return CHANGE_COLOR;
   }
  } // else another y
  } // if x
  else // x
  {
   mouse->Hide();
    ActuallColor=getpixel( x, y );
    UpdateActuallColor( ActuallColor );
    Message("Color choosen.");
   mouse->Show();
   return CHANGE_COLOR;
  }
 }

 return FALSE;
}


void KrsMenu::UpdateActuallColor( byte ActuallColor )
{
 char   String[7];

 setfillstyle( SOLID_FILL, ActuallColor );
 bar(X+12,Y+12,X+17,Y+48);   // left
 bar(X+302,Y+12,X+307,Y+48); // right
 // update color number
 setfillstyle( SOLID_FILL, BLACK );
 bar(X+262,Y+27,X+288,Y+35);   // left
 itoa( ActuallColor, String,10 );
 setcolor( TextCol ); outtextxy( X+263,Y+28,String );
}


void KrsMenu::Message( char *mesg, byte hide )
{
 if( Visible )
 {
  if( hide ) mouse->Hide();
   setcolor(TextCol);
   setfillstyle( SOLID_FILL, BLACK );
   bar(X+31,Y+27,X+257,Y+35);
   outtextxy(X+33,Y+28,mesg);
  if( hide ) mouse->Show();
 }
}


int KrsMenu::ReadString( char *Info, char *String )
{
 char masage[100]; masage[0]='\0';
 char buffer[50]; buffer[0]='\0';
 unsigned char  c, BufIndex=0;  // points to empty

 Message(Info);
 // clear keyboard buffer
 if ( kbhit() ) { do getch(); while( kbhit() ); }

 do
 {
  c=getch();

  switch( c )
  {
   case 8:  // backspace
            if( BufIndex>0 ) buffer[--BufIndex]='\0';
            else { sound(100); delay(200); nosound(); };
            break;
   case 13: // enter
            strcpy( String, buffer);
            return TRUE;
   case 27: // escape
             return FALSE;      // writing canceled
   default:
    buffer[BufIndex++]=(char)c;
    buffer[BufIndex]='\0';
    break;
  } // switch

  masage[0]='\0';
  strcpy(masage,Info);
  strcat(masage,buffer);
  Message(masage);
 }
 while( TRUE );
}

//-----------------------------------------------------------------------------


void ShakerSort( word far *buffer, word size )
// from smallest to highest, sort by words
{
 word changes,
      i, h, process=0;

 do
 {
  changes=0;

  // left to right
  for( i=0; i<(size-1); i++ )
  {
   if( buffer[i]>buffer[i+1] )
   {
    h=buffer[i+1];
    buffer[i+1]=buffer[i];
    buffer[i]=h;
    changes++;
   }
  }

  // right to left
  for( i=(size-1); i>0; i-- ) // 0 is not done here
  {
   if( buffer[i]>buffer[i+1] )
   {
    h=buffer[i+1];
    buffer[i+1]=buffer[i];
    buffer[i]=h;
    changes++;
   }
  }


  switch( process%4 )
  {
   case 0:
    PDrawMenu->Message("-", FALSE);
    break;
   case 1:
    PDrawMenu->Message("\\", FALSE);
    break;
   case 2:
    PDrawMenu->Message("|", FALSE);
    break;
   case 3:
    PDrawMenu->Message("/", FALSE);
    break;
  }

  process++;
 }
 while( changes );

}


//-----------------------------------------------------------------------------
