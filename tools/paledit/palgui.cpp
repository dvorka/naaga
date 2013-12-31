#include <math.h>
#include "dmouse.h"

#include "palgui.h"

extern Mouse mouse;


Scroll::Scroll(
	  int iX,
	  int iY,
	  int iHeight,
	  int iBgColor,
	  int iFgColor,
	  int iTriFgColor,
	  int iTriBgColor,
	  int iValueColor,
	  int iBaseWidth,
	  int iTriWidth,
	  int iValue,
	  int iMaxValue,
	  int iMinValue,
	  int iStep,
	  int iBigStep
	  )
{
  X=iX;
  Y=iY;
  Height=iHeight;
  BgColor=iBgColor;
  FgColor=iFgColor;
  BaseWidth=iBaseWidth;
  TriWidth=iTriWidth;
  TriFgColor=iTriFgColor;
  TriBgColor=iTriBgColor;
  ValueColor=iValueColor;
  Value=iValue;
  OldValue=Value-1;       // far the first draw
  MaxValue=iMaxValue;
  MinValue=iMinValue;
  Step=iStep;
  BigStep=iBigStep;

  XEnd=X+BaseWidth+2*TriWidth;
  YEnd=Y+Height;
}


int Scroll::Update( int x, int y, int ear ) // left means 1, right ear means 2
{
 if( x>=X && x<=XEnd && y>=Y && y<=YEnd ) // I am over
 {
  mouse.Hide();
  if(x<=(X+TriWidth) && ear==1) // dec value by 1
  {
   Value-=Step; if( Value<MinValue ) Value=MinValue;
   DrawInnards();
   mouse.Show();
   return 1;
  }
  if(x<=(X+TriWidth) && ear==2) // dec value by 1
  {
   Value-=BigStep; if( Value<MinValue ) Value=MinValue;
   DrawInnards();
   mouse.Show();
   return 1;
  }
  if(x>=(X+TriWidth+BaseWidth) && ear==2) // inc by more
  {
   Value+=BigStep; if( Value>MaxValue ) Value=MaxValue;
   DrawInnards();
   mouse.Show();
   return 1;
  }
  else // inc by 1
  {
   Value+=Step; if( Value>MaxValue ) Value=MaxValue;
   DrawInnards();
   mouse.Show();
   return 1;
  }
 }
 return 0;
}


void Scroll::DrawInnards( void )
{
 if( OldValue!=Value )
 {
  OldValue=Value;

  // base bg
  setfillstyle( SOLID_FILL, BgColor );
  bar(X+TriWidth,Y+1,X+TriWidth+BaseWidth,YEnd-1);

  // variable value bar
  setfillstyle( SOLID_FILL, ValueColor );
  float di=(float)Value/(float)MaxValue;   // 1%
	 di*=BaseWidth;
	 di=floor(di);
  int i=(int)di;
  bar(X+TriWidth,Y+1,X+TriWidth+i,YEnd-1);

  char string[5]; string[0]=0;
  itoa(Value, string, 10);
  setcolor( FgColor );
  outtextxy(X+(TriWidth*2+BaseWidth)/2-5, Y+2, string);
 }
}

void Scroll::Draw( void )
{
 if( OldValue!=Value )
 {
  OldValue=Value;

 mouse.Hide();
  // base bg
  setfillstyle( SOLID_FILL, BgColor );
  bar(X+TriWidth,Y,X+TriWidth+BaseWidth,YEnd);

  // variable value bar
  setfillstyle( SOLID_FILL, ValueColor );
  float di=(float)Value/(float)MaxValue;   // 1%
	 di*=BaseWidth;
	 di=floor(di);
  int i=(int)di;
  bar(X+TriWidth,Y,X+TriWidth+i,YEnd);

  // print number
  char string[5]; string[0]=0;
  itoa(Value, string, 10);
  setcolor( FgColor );
  outtextxy(X+(TriWidth*2+BaseWidth)/2-5, Y+2, string);

  // sides bg
  setfillstyle( SOLID_FILL, TriBgColor );
  bar(X,Y,X+TriWidth-1,YEnd);
  bar(X+TriWidth+BaseWidth+1,Y,XEnd,YEnd);
  // sides fg
  setcolor( FgColor );
   // left
   rectangle(X,Y,X+TriWidth-1,YEnd);
    line(X+1,Y+Height/2,X+TriWidth-1-1,Y+1);      //  /
    line(X+1,Y+Height/2,X+TriWidth-1-1,YEnd-1);   //  \
   // right
   rectangle(X+TriWidth+BaseWidth+1,Y,XEnd,YEnd);
    line(X+TriWidth+BaseWidth+1+1,Y+1,XEnd-1,Y+Height/2);      //  \
    line(X+TriWidth+BaseWidth+1+1,YEnd-1,XEnd-1,Y+Height/2);      //  /
   // base sides
    line(X+TriWidth,Y,XEnd-TriWidth,Y);
    line(X+TriWidth,YEnd,XEnd-TriWidth,YEnd);

 mouse.Show();
 }
}


void Scroll::SetValue( int NewValue )
{
 Value=NewValue;
 if( Value>MaxValue ) Value=MaxValue;
 if( Value<MinValue ) Value=MinValue;
 DrawInnards();
}
