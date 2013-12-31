//----------------------------------------------------------------------------------
//
//			          BSHELL.H
//
//				   Dvorka
//				    1999
//
//----------------------------------------------------------------------------------

#ifndef __BSHELL_H
 #define __BSHELL_H


 #include <dos.h>
 #include <stdio.h>

 #include "beast.h"
 #include "gfx.h"
 #include "tester.h"

 void shellBeastInfo();
 void shellNewFile();
 void shellLoad();
 void shellSave();
 void shellQuickSave();
 void shellAddImage();
 void shellNextImage();
 void shellLastImage();
 void shellShowImage();
 void shellEditCheckpoints();
 void shellGetPalFromBMP();
 void shellSetPalName();
 void shellSetLVL();
 void shellBeastTester();

#endif

//- EOF ----------------------------------------------------------------------------
