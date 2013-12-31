#include "dbmp.h"

#define PRINT_LOG
#define SIZE_OF_PALETTE 256*4 // 4 because:  BLUE GREEN RED UNUSED


void ClearKeybBuf( void )
{
 if ( kbhit() ) { do getch(); while( kbhit() ); }
}


void KeybWait( void )
{
 if ( kbhit() ) { do getch(); while( kbhit() ); }
 getch();
}


int DBMPPrintInfo( char *FileName )
{
 int Handle;
 unsigned int RealWidth, Read, DataSize;
 tBMPHeader BMPHeader;
 tBITMAPINFOHEADER BITMAPINFOHEADER;

 if( !FileName ) return 0;

 if (_dos_open(FileName, O_RDWR | O_BINARY, &Handle) != 0) return 1;
 _dos_read( Handle, &BMPHeader, sizeof(BMPHeader), &Read );
 _dos_read( Handle, &BITMAPINFOHEADER, sizeof(BITMAPINFOHEADER), &Read );
 _dos_close(Handle);

 RealWidth = (BMPHeader.bfSize-BMPHeader.bfOffBits)/BITMAPINFOHEADER.biHeight-1;
 DataSize  = BMPHeader.bfSize-sizeof(BITMAPINFOHEADER)-sizeof(BMPHeader)-SIZE_OF_PALETTE;

 if( BMPHeader.bfType!=19778 )   // "BM"=19778
  {
   cout << "\n\n " << FileName << " - this is not BMP file...\n";
   return 1;
  }

  cout << "\n\n " << FileName <<  " info...";
  cout << "\n Size of file: " << BMPHeader.bfSize;
  cout << "\n Data offset: " << BMPHeader.bfOffBits;

  cout << "\n Bitmap info header size: " << BITMAPINFOHEADER.biSize;
  cout << "\n Width " << BITMAPINFOHEADER.biWidth;
  cout << "\n Height: " << BITMAPINFOHEADER.biHeight;
  cout << "\n Number of planes (1): " << BITMAPINFOHEADER.biPlanes;
  cout << "\n Bits per pixel: " << BITMAPINFOHEADER.biBitCount;
  cout << "\n Type of compression (0): " << BITMAPINFOHEADER.biCompression;
  cout << "\n Size of image in pixels: " << BITMAPINFOHEADER.biSizeImage;
  cout << "\n Horizontal resolution pix/m: " << BITMAPINFOHEADER.biXPelsPerMeter;
  cout << "\n Vertical resolution pix/m: " << BITMAPINFOHEADER.biYPelsPerMeter;
  cout << "\n Used colors: " << BITMAPINFOHEADER.biClrUsed;
  cout << "\n Important colors: " << BITMAPINFOHEADER.biClrImportant;

  cout << "\n\n FileLenght-BMPHeaderHead-BMPHead-pallette(256*3) (READ): " << DataSize;
  cout << "\n RealWidth is " << RealWidth <<"+1 ";
 return 0;
}


int DBMPLoad( char *FileName )
{
 int Handle;
 unsigned char far *Palette=(unsigned char far *)farmalloc(SIZE_OF_PALETTE); // 4: R+G+B+UNUSED
 unsigned int  DataSize;
 unsigned int i, x=0, y=0, RealWidth, Read;
 tBMPHeader BMPHeader;
 tBITMAPINFOHEADER BITMAPINFOHEADER;
 struct palettetype PaletteType;

 if( !FileName ) return 0;
 if( !Palette ) return 0;

 // read headers
 if (_dos_open(FileName, O_RDWR | O_BINARY, &Handle) != 0) return 1;
 _dos_read( Handle, &BMPHeader, sizeof(BMPHeader), &Read );

 _dos_read( Handle, &BITMAPINFOHEADER, sizeof(BITMAPINFOHEADER), &Read );

 _dos_read( Handle, Palette, SIZE_OF_PALETTE, &Read );

 DataSize=BMPHeader.bfSize-sizeof(BITMAPINFOHEADER)-sizeof(BMPHeader)-SIZE_OF_PALETTE;
 unsigned char far *DataBuffer=(unsigned char far *)farmalloc(DataSize);
 if( DataBuffer==NULL ) return 0;
 _dos_read( Handle, DataBuffer, DataSize, &Read );

 _dos_close(Handle);

 RealWidth=(BMPHeader.bfSize-BMPHeader.bfOffBits)/BITMAPINFOHEADER.biHeight-1;

 if( BMPHeader.bfType!=19778 )   // "BM"=19778
  return 1;

 getpalette(&PaletteType);       // get system pallette
 //   Setting pallette:
 //   INT 10H,  AH=10H, AL=10H, BX = number of register to set
 //			      DH = red value
 //			      CH = green value
 //			      CL = blue value
 union  REGS registr;

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

 // drawing picture
 for(i=DataSize; i>0; i--)
 {
  if( x>RealWidth) { y++; x=0; }
  putpixel(RealWidth-x, y,(char)DataBuffer[i-1]);
  x++;
 }

 farfree( DataBuffer );
 farfree( Palette );
 return 0;
}
