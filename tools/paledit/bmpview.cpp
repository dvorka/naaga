//--------------------------------------------------------------------------
//                           Palette editor
//                               Dvorka
//                              1997,1999
//--------------------------------------------------------------------------

/*
 H ... ve stupnich 0-360
 S a V v promile takze to bude dost zkreslovat
 R,G,B v promile
*/

#include "dmouse.h"
#include "palgui.h"
#include "dbmp.h"
#include "dgraphic.h"
#include "rgbhsv.h"



unsigned char ActuallColor=15;
Mouse 	      mouse;

//--------------------------------------------------------------------------

 void ColorsIntegerToFloat( int ir,int ig,int ib,int is,int iv,
			    float &r,float &g,float &b,
			    float &s,float &v )
 {
  r=(float)ir/1000.0;
  g=(float)ig/1000.0;
  b=(float)ib/1000.0;

  // h same
  s=(float)is/1000.0;
  v=(float)iv/1000.0;
 }

//--------------------------------------------------------------------------

 void ColorsFloatToInteger( float r,float g,float b, float s,float v,
			    int &ir,int &ig,int &ib,int &is,int &iv )
 {
  ir=r*1000.0;
  ig=g*1000.0;
  ib=b*1000.0;
  // h same
  is=s*1000.0;
  iv=v*1000.0;
 }

 void DrawActuall( int x, int y, int width, int color )
 {
   setcolor(15);
   rectangle(x-1,y-1,x+width+1, y+1+width);

   setfillstyle(SOLID_FILL,color);
   bar( x,y,x+width, y+width);

   char string[5]; string[0]=0;
   itoa(ActuallColor, string, 10);
   setcolor(15); outtextxy(x+2,y+2,string);
 }

//--------------------------------------------------------------------------

 void DrawPalette( int x, int y, int width )
 {
  setcolor(15);
  rectangle(x-1,y-1,x+width+1, y+1+256);

  int i;
  for( i=0; i<=255; i++ )
  {
   setcolor(i);
   line( x, y+i, x+width, y+i );
  }
 }

//--------------------------------------------------------------------------

int main( int argc, char *argv[] )
{
 float r,g,b,h,s,v;
 int ir,ig,ib,ih=0,is=0,iv=0,
     RGBUpdated, HSVUpdated;
 union  REGS registr;
 unsigned char i, cr, cg, cb;

 DGraphics graph;
 Scroll ScrollR   (500-50,10,10,
                   DARKGRAY,WHITE,
                   YELLOW,LIGHTGRAY,
                   RED,
                   100,20,
		   0,
                   1000,0,
                   1,5 );
 Scroll ScrollG   (500-50,10+30,10,
                   DARKGRAY,WHITE,
                   YELLOW,LIGHTGRAY,
                   GREEN,
                   100,20,
                   0,
                   1000,0,
                   1,5 );
 Scroll ScrollB   (500-50,10+2*30,10,
                   DARKGRAY,WHITE,
                   YELLOW,LIGHTGRAY,
                   BLUE,
                   100,20,
                   0,
                   1000,0,
                   1,5 );

 Scroll ScrollH   (500-50,10+3*30,10,
                   DARKGRAY,WHITE,
                   YELLOW,LIGHTGRAY,
                   BROWN,
                   100,20,
                   0,
                   360,0,
                   1,5 );
 Scroll ScrollS   (500-50,10+4*30,10,
                   DARKGRAY,WHITE,
                   YELLOW,LIGHTGRAY,
                   BROWN,
                   100,20,
                   0,
                   1000,0,
                   1,5 );
 Scroll ScrollV   (500-50,10+5*30,10,
		   DARKGRAY,WHITE,
		   YELLOW,LIGHTGRAY,
		   BROWN,
		   100,20,
		   0,
		   1000,0,
		   1,5 );

 if( argc==1 )
 {
  cout << "\n\n BMP viewer by Dvorka";
  cout << "\n     - only 256-color up to 64kB size images supported...";
  cout << "\n Usage:";
  cout << "\n  bmpview -i filename        ...  to get info about the file";
  cout << "\n  bmpview filename           ...  to view file\n\n";
  return 0;
 }
 else
 {
  if( argc==3 )
  {
   DBMPPrintInfo( argv[2] );
  }
  else
  {
   ClearKeybBuf();

   mouse.Init(1,1);

   graph.Init(2);	// 3..1024x768

   setcolor(ActuallColor); outtextxy(500-50,2,"Red: (\%\%)");
   setcolor(ActuallColor); outtextxy(500-50,2+30,"Green: (\%\%)");
   setcolor(ActuallColor); outtextxy(500-50,2+30*2,"Blue: (\%\%)");
   setcolor(ActuallColor); outtextxy(500-50,2+30*3,"Hue:");
   setcolor(ActuallColor); outtextxy(500-50,2+30*4,"Saturation: (\%\%)");
   setcolor(ActuallColor); outtextxy(500-50,2+30*5,"Value: (\%\%)");

   DBMPLoad( argv[1] );

   mouse.SpaceX( 0, 1500 );
   mouse.SpaceY( 0, 1000 );

   DrawPalette( 600, 10, 15 );
   DrawActuall( 600, 10+280-15, 15+15, ActuallColor );

   ScrollR.Draw();
   ScrollG.Draw();
   ScrollB.Draw();
   ScrollH.Draw();
   ScrollS.Draw();
   ScrollV.Draw();

   while( !kbhit() )
   {
    mouse.EUpdate();

    if(mouse.LeftEar())
    {
     RGBUpdated=0;
     RGBUpdated|=ScrollR.Update(mouse.PosX(), mouse.PosY(),2);
     RGBUpdated|=ScrollG.Update(mouse.PosX(), mouse.PosY(),2);
     RGBUpdated|=ScrollB.Update(mouse.PosX(), mouse.PosY(),2);

     HSVUpdated=0;
     HSVUpdated|=ScrollH.Update(mouse.PosX(), mouse.PosY(),2);
     HSVUpdated|=ScrollS.Update(mouse.PosX(), mouse.PosY(),2);
     HSVUpdated|=ScrollV.Update(mouse.PosX(), mouse.PosY(),2);

     if( !(RGBUpdated || HSVUpdated) )
     {
      mouse.Hide();
       ActuallColor=getpixel(mouse.PosX(), mouse.PosY());
       DrawActuall( 600, 10+280-15, 15+15, ActuallColor );

       // get palette register

       asm	PUSH AX
       asm	PUSH BX
       asm	PUSH CX
       asm	PUSH DX
       asm	PUSH SI
       asm	PUSH DI
       asm	PUSH BP
       asm	PUSH SP

       registr.h.bh=0;
       registr.h.bl=ActuallColor;

       registr.h.ah=0x10;
       registr.h.al=0x15;

       int86(0x10,&registr,&registr);

       cr=registr.h.dh;
       cg=registr.h.ch;
       cb=registr.h.cl;

       asm	POP SP
       asm	POP BP
       asm	POP DI
       asm	POP SI
       asm	POP DX
       asm	POP CX
       asm	POP BX
       asm	POP AX

       r=(float)cr/63.0;
       g=(float)cg/63.0;
       b=(float)cb/63.0;

       RGBToHSV( r, g, b, h, s, v );
       ih=h;
       ColorsFloatToInteger( r,g,b,s,v,ir,ig,ib,is,iv );

       ScrollH.SetValue(ih); ScrollH.DrawInnards();
       ScrollS.SetValue(is); ScrollS.DrawInnards();
       ScrollV.SetValue(iv); ScrollV.DrawInnards();
       ScrollR.SetValue(ir); ScrollR.DrawInnards();
       ScrollG.SetValue(ig); ScrollG.DrawInnards();
       ScrollB.SetValue(ib); ScrollB.DrawInnards();

      mouse.Show();
      continue;
     }

     if( RGBUpdated )
     {
      h=(float)ScrollH.GetValue();
      ColorsIntegerToFloat( ScrollR.GetValue(),
                            ScrollG.GetValue(),
                            ScrollB.GetValue(),
                            ScrollS.GetValue(),
                            ScrollV.GetValue(),
                            r,g,b,s,v );
      RGBToHSV( r, g, b, h, s, v );
      ih=h;
      ColorsFloatToInteger( r,g,b,s,v,ir,ig,ib,is,iv );
      ScrollH.SetValue(ih); ScrollH.DrawInnards();
      ScrollS.SetValue(is); ScrollS.DrawInnards();
      ScrollV.SetValue(iv); ScrollV.DrawInnards();
     }

     if( HSVUpdated )
     {
      h=(float)ScrollH.GetValue();
      ColorsIntegerToFloat( ScrollR.GetValue(),
                            ScrollG.GetValue(),
                            ScrollB.GetValue(),
                            ScrollS.GetValue(),
                            ScrollV.GetValue(),
                            r,g,b,s,v );
      HSVToRGB( h, s, v, r, g, b );
      ih=h;
      ColorsFloatToInteger( r,g,b,s,v,ir,ig,ib,is,iv );
      ScrollR.SetValue(ir); ScrollR.DrawInnards();
      ScrollG.SetValue(ig); ScrollG.DrawInnards();
      ScrollB.SetValue(ib); ScrollB.DrawInnards();
     }

   // change palette segments
   cr=(r*63.0);
   cg=(g*63.0);
   cb=(b*63.0);

   registr.h.bh=0;
   registr.h.bl=ActuallColor;
   registr.h.dh=(unsigned char)cr;
   registr.h.ch=(unsigned char)cg;
   registr.h.cl=(unsigned char)cb;

   registr.h.ah=0x10;
   registr.h.al=0x10;

   int86(0x10,&registr,&registr);

    }
   }

//   KeybWait();
   graph.Close();
   ClearKeybBuf();
  }
 }
 return 0;
}

