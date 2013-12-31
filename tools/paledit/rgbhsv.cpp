#include "rgbhsv.h"

/*
 R ... <0,1>
 G ... <0,1>
 B ... <0,1>
 H ... <0,360>       zakladni spektralni barva
 S ... <0,1>         bila - spektralni
 V ... <0,1>         cerna - nejjasnejsi

 hue ... barevny odstin
	 H - zakladni spektralni barva - rozsah 0 -360 stupnu
 saturation ... sytost
         S - sytost, cistota - pomer ciste barvy a bile
         rozsah 0 ( bila ) az 1 ( spektralni )
 value ... jas
         V - jas, intenzita - rozsah 0 ( cerna ) az 1
*/



float minimum( float R, float G, float B )
{
 float min = R;
 if( G<min ) min=G;
 if( B<min ) return B; else return min;
}


float maximum( float R, float G, float B )
{
 float max = R;
 if( G>max ) max=G;
 if( B>max ) return B; else return max;
}


void RGBToHSV( float R, float G, float B, float &H, float &S, float &V )
// Hue, Saturation, Value ( jas )
{
 float max, min, delta;

 min = minimum( R, G, B );
 max = maximum( R, G, B );
 V = max;
 delta = max-min;
 if( max != 0.0 )
  S=delta/max;
 else
  S=0.0;

 if( delta != 0.0 )
 {
  if( R == max )
   H = ( G-B )/delta;
  else
   {
    if( G == max )
     H=2.0+(B-R)/delta;
    else
     H=4.0+(R-G)/delta;

    H*=60.0; // prevod na stupne
   }

  if( H<0.0 )
   H+=360.0;
 }
}



void HSVToRGB( float H, float S, float V, float &R, float &G, float &B )
{
 float i, f, p, q, t;

 if( S==0.0 )		// achromaticky pripad
 {
  R=V; G=V; B=V;
 }
 else			// chromaticky pripad
 {
  if( H==360.0 ) H=0.0;
  H/=60.0; 		// 0 <= H < 6
  i=(int)H;		// cislo vysece 0 <= i <= 5
  f=H-i;		// 0 <= f < 1

  p=V*(1.0-S);
  q=V*(1.0-S*f);
  t=V*(1.0-S*(1.0-f));

  switch( (int)i )  // sest vyseci
  {
   case 0:
	  R=V;
	  G=t;
	  B=p;
	  break;
   case 1:
	  R=q;
	  G=V;
	  B=p;
	  break;
   case 2:
	  R=p;
	  G=V;
	  B=t;
	  break;
   case 3:
	  R=p;
	  G=q;
	  B=V;
	  break;
   case 4:
	  R=t;
	  G=p;
	  B=V;
	  break;
   case 5:
	  R=V;
	  G=p;
	  B=q;
	  break;
  }
 } // chromaticky pripad
}



