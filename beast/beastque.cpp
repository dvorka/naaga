//----------------------------------------------------------------------------------
//
//                             Queue of beasts
//
//				   Dvorka
//				    1999
//
//----------------------------------------------------------------------------------

#include "beastque.h"

#include <graphics.h>

//----------------------------------------------------------------------------------

BeastQueueItem::BeastQueueItem(Beast far *beast)
{
 this->beast=beast;
}
//----------------------------------------------------------------------------------

BeastQueueItem::~BeastQueueItem()
// !!! destroys beast !!!
{
 delete beast;
}

//----------------------------------------------------------------------------------

BeastQueue::BeastQueue()
{
 avatar  = new Avatar(200,30);
 AvaShot = new List;
 SoftShot= new List;
 HardShot= new List;
}

//----------------------------------------------------------------------------------

void BeastQueue::insert(Beast far *beast, word type )
{
 BeastQueueItem far *b = new BeastQueueItem(beast);

 switch(type)
 {
  case AVA_SHOT:
       AvaShot->Insert(b);
       break;
  case SOFT_SHOT:
       SoftShot->Insert(b);
       break;
  case HARD_SHOT:
       HardShot->Insert(b);
       break;
  default:
	exit(0);

 }
}

//----------------------------------------------------------------------------------

void BeastQueue::show(byte far *screen)
// - makes move & checks collisions & draws beast scene
// order of checking:
//  - draw avatar - check tiles for collision
//  - ava shots - only draw
//  - soft shots ... kills avatar and its shots, soft shot does nothing to
//		     another soft shot
//    - check everything
//    - draw everything
//  - hard shots ... kills everythig (brothers included)
//    - loop(check one, draw one)

{
 ListItem far *i,
	  far *ii;



 // fake wall - put it away!
 int f;
 for(f=0; f<300; f++ )
  screen[320*50+f]=RED;


 // --- avatar ---
 avatar->move();
 if( avatar->check(screen) )
 {
  avatar->unmove();
 }

 avatar->show(screen);



 // --- ava shots ---
 i=AvaShot->GetHead();

 while(i)
 {
  // save next, i can be deleted
  ii=AvaShot->Next(i);

  ((BeastQueueItem far*)i)->beast->move();

  if( ((BeastQueueItem far*)i)->beast->check(screen) )
  {
   // returns true if object wants to be deleted
   if( ((BeastQueueItem far*)i)->beast->collision(screen) )
   {
    ((BeastQueueItem far*)i)->beast->show(screen);
    AvaShot->Delete(i);
   }
  }
  else
   ((BeastQueueItem far*)i)->beast->show(screen);

  i=ii;
 }



 // --- soft shots ---
 i=SoftShot->GetHead();

 while(i)
 {
  // save next, i can be deleted
  ii=SoftShot->Next(i);

  ((BeastQueueItem far*)i)->beast->move();

  if( ((BeastQueueItem far*)i)->beast->check(screen) )
  {
   // returns true if object wants to be deleted
   if( ((BeastQueueItem far*)i)->beast->collision(screen) )
    AvaShot->Delete(i);
  }

  i=ii;
 }
 // draw everything
 i=SoftShot->GetHead();

 while(i)
 {
  ((BeastQueueItem far*)i)->beast->show(screen);
  i=SoftShot->Next(i);
 }

 // --- hard shots ---
 i=HardShot->GetHead();

 while(i)
 {
  // save next, i can be deleted
  ii=HardShot->Next(i);

  ((BeastQueueItem far*)i)->beast->move();

  if( ((BeastQueueItem far*)i)->beast->check(screen) )
  {
   // returns true if object wants to be deleted
   if( ((BeastQueueItem far*)i)->beast->collision(screen) )
   {
    ((BeastQueueItem far*)i)->beast->show(screen);
    AvaShot->Delete(i);
   }
  }
  else
   ((BeastQueueItem far*)i)->beast->show(screen);

  i=ii;
 }


}

//----------------------------------------------------------------------------------

BeastQueue::~BeastQueue()
{
 delete avatar;
 delete AvaShot;
 delete SoftShot;
 delete HardShot;
}

//- EOF ----------------------------------------------------------------------------
