//----------------------------------------------------------------------------------
//
//                             Queue of beasts
//
//				   Dvorka
//				    1999
//
//----------------------------------------------------------------------------------

#ifndef __BEASTQUE_H
 #define __BEASTQUE_H

 #include <alloc.h>
 #include <process.h>
 #include <stdlib.h>

 #include "beast.h"
 #include "defs.h"
 #include "list.h"

 //----------------------------------------------------------------------------------

 class far BeastQueueItem : public ListItem
 {
  public:
   Beast far *beast;

   BeastQueueItem(Beast far *beast);
   ~BeastQueueItem(); // !!! destroys beast !!!

 };

 //----------------------------------------------------------------------------------

 class far BeastQueue
 {
  public:
   BeastQueue();
    void show(byte far *screen); // checks for collisions, treats them
			    // draws whole beast scene
    void insert(Beast far *beast, word type );
   ~BeastQueue();

  private:
   Beast far *avatar;
   List  far *AvaShot,
	 far *SoftShot,
	 far *HardShot;
 };

#endif

//- EOF ----------------------------------------------------------------------------
