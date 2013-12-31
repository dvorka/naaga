//----------------------------------------------------------------------------
//
//				Keyboard tester
//
//                                   Dvorka
//                                    1999
//
//----------------------------------------------------------------------------

#include <dos.h>
#include <stdio.h>
#include <conio.h>

//----------------------------------------------------------------------------

// variable arg lng
#ifdef __cplusplus
    #define __CPPARGS ...
#else
    #define __CPPARGS
#endif

//----------------------------------------------------------------------------

#define INTR_KEYB	0x09

//----------------------------------------------------------------------------

typedef unsigned char byte;

static byte keyBoard[128]; // array of keys. If byte is 1, key is down
			   // else, key is up

enum keyBoardKeys
{
 KEYB_UP        =72,
 KEYB_DOWN      =80,
 KEYB_LEFT      =75,
 KEYB_RIGHT     =77,

 KEYB_LSHIFT	=42,
 KEYB_RSHIFT    =54,
 KEYB_LALT      =56,
 KEYB_RALT      =56,
 KEYB_LCTRL     =29,
 KEYB_RCTRL     =29
};

//----------------------------------------------------------------------------

// declaring variable oldKeybHandler, will be used for storing old handler vec
void interrupt ( *oldKeybHandler)(__CPPARGS);

//----------------------------------------------------------------------------

 unsigned char key;

// my handler of keyboard
void interrupt naagaKeybHandler(__CPPARGS)
{
 // data must be declared outside

 // read key from port
 asm in  al,0x60
 asm mov key,al

 if( key<128 ) 		// key down
  keyBoard[key]=1;
 else                   // key up
  keyBoard[key-128]=0;

 // call the old routine
 oldKeybHandler();
}

//----------------------------------------------------------------------------

int main(void)
{
 int i;
 for(i=0;i<128;i++) keyBoard[i]=0;

 // save the old interrupt vector for int 0x09
 oldKeybHandler = getvect(INTR_KEYB);

 // install the new interrupt handler
 setvect(INTR_KEYB, naagaKeybHandler);



 // ... code ...
 do
 {
  printf("\n");

  if( keyBoard[KEYB_UP] ) 	printf(" up");
  if( keyBoard[KEYB_DOWN] ) 	printf(" down");
  if( keyBoard[KEYB_LEFT] ) 	printf(" left");
  if( keyBoard[KEYB_RIGHT] ) 	printf(" right");

  if( keyBoard[KEYB_LCTRL] ) 	printf(" LCtrl");

  if( kbhit() ) if( getch()=='q' ) break;
 }
 while( 1 );



 // reset the old interrupt handler
 setvect(INTR_KEYB, oldKeybHandler);

 return 0;
}

//- EOF ---------------------------------------------------------------------------






//     asm mov al,0xf4
//     asm out 0x60,al


/*
int main(void)
{
   unsigned char low;
   unsigned char high;
   unsigned char beg;
   unsigned char end;
   unsigned char b0;	// byte na beg+1
   unsigned char b1;    // byte na beg+2

   unsigned int  i;



   do
   {

//     printf("\n");

     asm push es
     asm xor ax,ax
     asm mov es,ax

     asm mov al,[es:0x41a]
     asm mov beg,al

     asm mov al,[es:0x41c]
     asm mov end,al
     asm pop es



     printf(" %d %d %d %d",beg,end,b0,b1);

     // clear buffer
//     while( kbhit() ) getch();

	if(kbhit())
	{
//	 break;
	 asm mov al,0xf4
	 asm out 0x60,al

	 sleep(1);
	}

   }
   while(1);
}

 porty
     asm mov al,0xed
     asm out 0x60,al
     asm mov al,0x0f
     asm out 0x60,al

     // read ack
     asm in  al,0x60
     asm mov b0,al

 klasika
     asm push es
     asm xor ax,ax
     asm mov es,ax

     asm mov al,[es:0x41a]
     asm mov beg,al

     asm mov al,[es:0x41c]
     asm mov end,al



     asm cmp beg ,al
     asm jz hotovka

     asm inc beg
     asm inc beg
     asm cmp beg, 62
     asm jnz hotovka

     asm mov beg, 30

     hotovka:

     asm mov al, beg
     asm mov [es:0x41a], al


     asm mov al,[es:0x41e]
     asm mov b0,al

     asm mov al,[es:0x41f]
     asm mov b1,al

     asm pop es


     if(low&0x01) printf(" R");
     if(low&0x02) printf(" L");
     if(low&0x04) printf(" CTRL");
     if(low&0x08) printf(" ALT");


     if(high&0x01) printf(" Ctrl");
     if(high&0x02) printf(" Alt");
     if(high&0x04) printf(" ?");
     if(high&0x08) printf(" hold");
     if(high&0x10) printf(" Scrollock");
     if(high&0x20) printf(" Numlock");
     if(high&0x40) printf(" Capslock");
     if(high&0x80) printf(" Insert");

     if(low&0x01) printf(" RShift");
     if(low&0x02) printf(" LShift");

     if(low&0x04 && high&0x01 )  printf(" LCTRL");
     if(low&0x04 && !(high&0x01) ) printf(" RCTRL");
     if(low&0x08 && high&0x02 )  printf(" LALT");
     if(low&0x08 && !(high&0x02) ) printf(" RALT");



     asm xor ax, ax
     asm mov ah, 0x01
     asm int 0x16
     asm jnz nic
     asm mov b0, 0
     nic:
     asm mov b0, 1
     asm mov beg, al
     asm mov end, ah

   asm xor ax, ax
   asm xor bx, bx
   asm mov ah, 0x06
   asm mov dl, 0xff
   asm int 0x21
   asm jnz nic:
    asm mov b0, 0
    asm jmp neco
   nic:
    asm mov b0, 1
    asm mov beg, al
    printf("\n %d %d %d",beg,end,b0);
   neco:
*/




