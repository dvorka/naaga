//----------------------------------------------------------------------------
//                        QNETGUI - item of project Dama
//                                 Dvorka
//				    1997
//----------------------------------------------------------------------------

/*

 Co udelat:

  - objekt okno jako v noteditu
  - objekt button do okna - bude se hodit
  - master/slave/cancel
  - vyzvan, cekam na odpoved - master
  - cekam na vyzvy - slave
  - send move/ end of game/ send masage
  - waiting for opponent move
  - error okno

  - listovaci okno na usery
    => pouze se bude vybirat, ja nebudu psat nic!
    => nebudu potrebovat editlinu

*/

#ifndef __QNETGUI_H
#define __QNETGUI_H


 #include <alloc.h>
 #include <conio.h>
 #include <dos.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>

 #include "defs.h"
 #include "dgraphic.h"
 #include "dmouse.h"


 #define INTERNIC 0

 #define DISCARD     1
 #define REGISTER    2
 #define REPLACE     3

 #define HORIZONTALY 2
 #define VERTICALY   3

 #define NEXT        3
 #define LAST        2
 #define OK          1


 class Cuflik
 {
  private:
   int X, Y,
       Sirka,
       Vyska,
       TxtBarva,
       BgBarva,
       FrameBarva,
       PushFlag;

   char *Popis;

  public:
   Cuflik( int iX, int iY, int iSirka, int iVyska,
	   int iTxtBarva, int iBgBarva, int iFrameBarva,
	   char *iPopis );
    void Draw( void );
    int  Update( int x, int y );
    int Pushed( void )
    {
     if( PushFlag ) { PushFlag=FALSE; return TRUE; } else return PushFlag;
    };

 };


 class Wokno
 {
  private:
   int X, Y,
       Sirka,
       Vyska,
       HeadTxtBarva,
       HeadBgBarva,
       TxtBarva,
       BgBarva,
       FrameBarva;
   char     *Hlavicka;
   void far *Underground;


  public:
   Wokno( int iX, int iY, int iSirka, int iVyska,
	  int iHeadTxtBarva, int iHeadBgBarva,
	  int iTxtBarva, int iBgBarva, int iFrameBarva,
	  char *iHlavicka );
    void Draw( void );
    void Skovej( void );
   ~Wokno();
 };



 class ThreeButtonWnd
 {
  private:
   int    X, Y;
   Cuflik *MasterCuflik,
	  *SlaveCuflik,
	  *CancelCuflik;
   Wokno  *Okno;


  public:
   ThreeButtonWnd( int iX, int iY, char *h, char *b1, char *b2, char *b3 );
    void Draw( void );
    int  Choosen( void );
   ~ThreeButtonWnd();
 };


 class OneButtonWnd
 {
  private:
   int    X, Y;
   Cuflik *CancelCuflik;
   Wokno  *Okno;
   char   *Masage;

  public:
   OneButtonWnd( int iX, int iY, char *iHead, char *iBtnMsg, char *iMasage );
    void Draw( void );
    int  Update( void );
   ~OneButtonWnd();
 };


 class GuiTxtLn
 {
  private:
   int X, Y, Sirka, Vyska, BgBarva, FgBarva, FrameBarva;

  public:
   GuiTxtLn( int iX, int iY, int iSirka,
	     int BgBarva, int FgBarva, int FrameBarva );
   void Draw( void );
   void Put( char *Masage );
   void Clear( void );
 };


#endif