#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "prioQueue.h"

// Functions supported by Priority Queue
int 
InitPrioQueue (PrioQueue* queue, prioCompFunc cmpf, prioGetPos gpf, 
                   prioSetPos spf, prioPrintElem prf)
{
  queue->qSize = 0;
  queue->aSize = INIT_QUEUE_SIZE;
  queue->elems = (void**)malloc(queue->aSize * sizeof(void*));
  queue->compFunc = cmpf;
  queue->getPos = gpf;
  queue->setPos = spf;
  queue->printF = prf;
  return 0;  
}

// Returns the pointer to the head of the queue, do not remove it 
// from the queue
void*
prioQueueHead (PrioQueue* queue)
{
  if (queue->qSize > 0)
    return queue->elems[0];
  else {
    // printf ("No more data in priority queue\n");
    return NULL;
  }
}

// Removes the top of the queue, and reduce the queue size. Returns 1 
// for success. 
// Need to 
int  
prioQueuePop (PrioQueue* queue)
{
  if (queue->qSize == 0) {
    printf ("Priority queue underflow!!\n");
    return 0;
  }

  queue->elems[0] = queue->elems[--queue->qSize];
  // printf ("Priority queue size reduced to %d\n", queue->qSize);

  if (queue->qSize == 0) {
    return 1;
  }
  prioHeapify (queue, 0);

  return 1;
}

// Adding a new element. The aSize gets increased automatically.
// There should not be any pointers from outside to the top of the queue
int 
prioQueueAdd (PrioQueue* queue, void *elem)
{
  // update the elems array, if the quesize is equal to that.

  queue->setPos (elem, queue->qSize);
  queue->elems[queue->qSize++] = elem;
  // printf ("Priority queue size increased to %d\n", queue->qSize);
  return prioReduceCost (queue, elem);
}

// If an element cost is reduced, then this function can get the
// element pushed up through the tree
int 
prioReduceCost (PrioQueue* queue, void *elem)
{
  int index = queue->getPos (elem);
  void *key = queue->elems[index];

  while (index > 0) {
    // check if the root is smaller than its parent, if so, then swap 
    // parent and root, and set root to parent position. make sure to 
    // set the position in the elems
    if (queue->compFunc (key, queue->elems[PARENT(index)]) < 1 ) {
      queue->elems[index] = queue->elems[PARENT(index)];  //Push down parent element 
      queue->setPos (queue->elems[index], index);
      index = PARENT(index);  //Move up index
    } else
      break;
  }
  queue->elems[index] = key;  //Replace element
  queue->setPos (queue->elems[index], index);
  return 1;    
}

// If an element cost increases, then we can call this function
// to push the elment later in the queue
int 
prioHeapify (PrioQueue* queue, int rootPos)
{
    int child;
    void *key = queue->elems[rootPos];

    assert(rootPos >= 0);
    assert(rootPos < queue->qSize);

    child = rootPos * 2 + 1;
    while (child < queue->qSize) {
      // Both child exist, pick the smaller one
      if (child+1 < queue->qSize) 
      	if (queue->compFunc (queue->elems[child], queue->elems[child+1]) > 0)
	  child++;

      // again compare it with root, and if required, push it down
      if (queue->compFunc (key, queue->elems[child]) > 0) {
        // swap root and child, along with position nos
        queue->elems[rootPos] = queue->elems[child]; 
        queue->setPos (queue->elems[rootPos], rootPos);
        rootPos = child;	//Move down index
      } else 
        break; //No need to push down further	

      //Check if left child exists
      child = rootPos * 2 + 1;
    }
    queue->elems[rootPos] = key; //Replace element 	
    queue->setPos (key, rootPos);
    return 1;
}

// Frees up all the structures / allocation. Queue an be reused only
// after initializing again.
int 
prioCleanup (PrioQueue* queue)
{
  queue->qSize = 0;
  // free(queue->elems);
  return 1;
}

int
prioQueuePrint (PrioQueue* queue)
{
#ifndef NDEBUG
#ifdef DEBUG_PRIO_QUEUE
  // print the priority Queue, in debug mode
  for (int i=0; i < queue->qSize; i++) {
    /* if (i > 0)
      assert (queue->compFunc(queue->elems[i], queue->elems[PARENT(i)]) >= 0); */
    printf ("Queue position %d -> ", i);
    queue->printF(queue->elems[i]);
    printf("\n");
  } 
#endif
#endif
  return 1;
}
