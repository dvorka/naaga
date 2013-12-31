#ifndef PALGUI_H
 #define PALGUI_H

class Scroll
{
 private:
 int
  X,
  Y,
  Height,
  BgColor,
  FgColor,
  ValueColor,
  BaseWidth,

  TriWidth,
  TriFgColor,
  TriBgColor,

  Value,
  OldValue,         // for redrawing
  MaxValue,
  MinValue,
  Step,
  BigStep,

  XEnd,
  YEnd
  ;

 public:
  Scroll(
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
	   );

   int  Update( int x, int y, int ear  ); // returns 1 if updated 0 if not
   void DrawInnards( void );
   void Draw( void );
   void SetValue( int NewValue );
   int  GetValue( void  ) { return Value; }
};

#endif