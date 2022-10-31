#ifndef PRIOQUEUE_H

#define PRIOQUEUE_H

// comparison function between two elements
typedef int (*prioCompFunc)(void* elem1, void* elem2);
typedef int (*prioGetPos)(void *elem);
typedef int (*prioSetPos)(void *elem, int newPos);
typedef int (*prioPrintElem)(void *elem);

#define INIT_QUEUE_SIZE 5000

typedef struct PrioQueue {
  int      qSize;    /* Number of elements in the queue */
  int      aSize;    /* Array size, space allocated     */
  void **  elems;    /* Pointer to the array, dynamically managed */
  prioCompFunc  compFunc;  /* Function pointer to compare two elements */
  prioGetPos    getPos;    /* Function to get elem position in the queue */
  prioSetPos    setPos;    /* Function to set queue position in the element */
  prioPrintElem printF;    /* Function to print the element */
} PrioQueue;
  

// Functions supported by Priority Queue
int InitPrioQueue (PrioQueue* queue, prioCompFunc cmpf, prioGetPos gpf, 
                   prioSetPos spf, prioPrintElem prf);

// Returns the pointer to the head of the queue, do not remove it 
// from the queue
void *prioQueueHead (PrioQueue* queue);

// Removes the top of the queue, and reduce the queue size. Returns 1 
// for success
int  prioQueuePop (PrioQueue* queue);

// Adding a new element. The aSize gets increased automatically.
// There should not be any pointers from outside to the top of the queue
int prioQueueAdd (PrioQueue* queue, void *elem);

// If an element cost is reduced, then this function can get the
// element pushed up through the tree
int prioReduceCost (PrioQueue* queue, void *elem);

// If an element's cost increases, then we can call this function
// to push the elment later in the queue
int prioHeapify (PrioQueue* queue, int rootPos);

// Frees up all the structures / allocation. Queue an be reused only
// after initializing again.
int prioCleanup (PrioQueue* queue);

// printf the priority queue in stdout
int prioQueuePrint (PrioQueue* queue);


#define PARENT(x) ((x-1)/2)

#endif
