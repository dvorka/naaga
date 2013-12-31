//----------------------------------------------------------------------------------
//
//			         BMP loader
//
//				   Dvorka
//				    1997
//
//----------------------------------------------------------------------------------

#include "dbmp.h"

#define PRINT_LOG
#define SIZE_OF_PALETTE 256*4 // 4 because:  BLUE GREEN RED UNUSED


extern byte far *Background;
extern byte far *GameMenu;
word   DBMPw,
       DBMPh;

int DBMPLoad( char *FileName, byte far *&data, byte what )
{
 byte help[320]; // upside down image
 byte far *Palette=(unsigned char far *)farmalloc(SIZE_OF_PALETTE); // 4: R+G+B+UNUSED
 int  Handle;
 word i, x=0, y=0, Read;
 long RealWidth, DataSize;

 tBMPHeader 		BMPHeader;
 tBITMAPINFOHEADER 	BITMAPINFOHEADER;

 if( !FileName ) return 0;
 if( !Palette ) return 0;

 // read headers
 if (_dos_open(FileName, O_RDWR | O_BINARY, &Handle) != 0) { printf("\n Error: Unable to open %1...",FileName); return 1; }
  _dos_read( Handle, &BMPHeader, sizeof(BMPHeader), &Read );
  _dos_read( Handle, &BITMAPINFOHEADER, sizeof(BITMAPINFOHEADER), &Read );
  _dos_read( Handle, Palette, SIZE_OF_PALETTE, &Read );

 DataSize=BMPHeader.bfSize-sizeof(BITMAPINFOHEADER)-sizeof(BMPHeader)-SIZE_OF_PALETTE;


 unsigned char far *import;

 switch( what )
 {
  case DBMPGETDATA:
		  import=(unsigned char far *)farmalloc(DataSize);
		  if( !import ) { printf("\n Error: Unable to alloc import buffer..."); return 1; }
		  _dos_read( Handle, import, (word)DataSize, &Read );
		  break;
  case SHOWBMP:
		  import=(unsigned char far *)farmalloc(DataSize);
		  _dos_read( Handle, import, (word)DataSize, &Read );
		  break;
  case CELLIMPORT:
		  import=(unsigned char far *)farmalloc(DataSize);
		  if( Background==NULL ) return 0;
		  _dos_read( Handle, import, (word)DataSize, &Read );
		  break;
  case BACKGROUND:
		  _dos_read( Handle, Background, (word)DataSize, &Read );
		  break;
  case GAMEMENU:
		  _dos_read( Handle, GameMenu, (word)DataSize, &Read );
		  break;
 }

 _dos_close(Handle);

 RealWidth=(BMPHeader.bfSize-BMPHeader.bfOffBits)/BITMAPINFOHEADER.biHeight-1u;

 if( BMPHeader.bfType!=19778 )   // "BM"=19778
  return 1;

 //   Setting pallette:
 //   INT 10H,  AH=10H, AL=10H, BX = number of register to set
 //			      DH = red value
 //			      CH = green value
 //			      CL = blue value
 union  REGS registr;

 if( what==BACKGROUND || what==SHOWBMP )
 {
  for (i=0; i<256; i++)
  {
   registr.h.bh=0;
   registr.h.bl=i;
   registr.h.dh=Palette[i*4+2]>>2;//    color must be shifted!!!
   registr.h.ch=Palette[i*4+1]>>2;
   registr.h.cl=Palette[i*4+0]>>2;

   registr.h.ah=0x10;
   registr.h.al=0x10;

   int86(0x10,&registr,&registr);
  }
 }

 switch( what )
 {
  case DBMPGETDATA:
		data=import;
		// reverse the content of data
		for(i=0; i<(DataSize/2); i++ )
		{
		 Read=data[i];
		 data[i]=data[DataSize-1-i];
		 data[DataSize-1-i]=Read;
		}

		DBMPw=RealWidth+1;
		DBMPh=DataSize/DBMPw;
		// printf("\n w: %d, h %d", DBMPw, DBMPh);
		break;
  case SHOWBMP:
  case CELLIMPORT:
		// drawing picture
		for(i=(word)DataSize; i>0; i--)
		{
		 if( x>RealWidth) { y++; x=0; }
		 putpixel((word)RealWidth-x, y,(char)import[i-1]);
		 x++;
		}
		farfree( import );
		break;
  case BACKGROUND:
		for( i=0; i<100; i++ )
		{
		 for( x=0; x<320; x++ )
		  help[x]=Background[i*320+x];

		 for( x=0; x<320; x++ )
		  Background[i*320+x]=Background[(200-1-i)*320+x];

		 for( x=0; x<320; x++ )
		  Background[(200-1-i)*320+x]=help[x];
		}
		break;
  case GAMEMENU:
		for( i=0; i<10; i++ )
		{
		 for( x=0; x<320; x++ )
		  help[x]=GameMenu[i*320+x];

		 for( x=0; x<320; x++ )
		  GameMenu[i*320+x]=GameMenu[(20-1-i)*320+x];

		 for( x=0; x<320; x++ )
		  GameMenu[(20-1-i)*320+x]=help[x];
		}
		break;
 }



 farfree( Palette );
 return 0;
}

