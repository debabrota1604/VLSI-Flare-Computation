#ifndef MAD_H
#define MAD_H

#include "routing.h"

#define MAD_TREE_FREE  0x00
#define MAD_TREE_USED  0x01
#define MAD_TREE_INQUEUE 0x02
#define MAD_TREE_IN_SINK_PATH 0x04

#define MAX_INT    32657
#define MAX_FLOAT  1e10

#define InTree(node)          ((node)->status & MAD_TREE_USED)
#define SetInTree(node)       (node)->status |= MAD_TREE_USED
#define ResetInTree(node)     (node)->status &= !MAD_TREE_USED

#define InQueue(node)         ((node)->status & MAD_TREE_INQUEUE)
#define SetInQueue(node)      (node)->status |= MAD_TREE_INQUEUE
#define ResetInQueue(node)    (node)->status &= !MAD_TREE_INQUEUE

#define InSinkPath(node)      ((node)->status & MAD_TREE_IN_SINK_PATH)
#define SetInSinkPath(node)   (node)->status |= MAD_TREE_IN_SINK_PATH
#define ResetInSinkPath(node) (node)->status &= !MAD_TREE_IN_SINK_PATH

struct mad_edge;

typedef struct nodeCost {
  float treeRes;
  float treeCap;
  float treeCost;  // effectively the delay from source
  float cap;
  float lastTreeCapWeight;
} NodeCost;

#define INIT_NODECOST(cost)  \
  (cost)->treeRes = 0.0 ; \
  (cost)->treeCap = 0.0;  \
  (cost)->treeCost = MAX_FLOAT; \
  (cost)->cap = 0.0; \
  (cost)->lastTreeCapWeight = 0.0;
  


typedef struct mad_node 
{
  int              nodeId;
  int              nodeIndex;
  short            status;
  short            sinkCount;
  short            queuePos;
  struct mad_edge* edgeList;
  struct mad_edge* predEdge;
  struct mad_node* lastNode;
  NodeCost*        cost;
} MadNode;

typedef struct mad_edge_cost {
  short            minCapacity;
  short            maxCapacity;
  MadNode*         parentNode;
  float            resVal;
  float            capVal;
  float            delay;
} MadEdgeCost;

typedef struct mad_edge {
  MadNode*         sourceNode;
  MadNode*         otherNode;
  short            direction;
  MadEdgeCost*     cost;
  struct mad_edge* nextEdge;
} MadEdge;

typedef int (*CompNode)(MadNode* node1, MadNode* node2);

typedef struct priorityQueue {
  int           queueSize;
  MadNode**     entries;
  CompNode*     compareFunc;
  int           arraySize;
} MadNodeQueue;


/* function definitions */
extern int      addNode (MadNodeQueue* queue, MadNode* newNode);
extern MadNode* getHead (MadNodeQueue* queue);
extern MadNode* removeHead (MadNodeQueue* queue);
extern void     updatePosition (MadNodeQueue* queue, int position);

/* actual functions for creating the tree */
extern MadNode* initMad (int netNo, TNET* tnetwork, float drive_res,
                          int N, int N_All, TNODE* globalNodes, 
                          int* minResource);

extern int 
runDijkstra (MadNode* nodes, MadNode* rootNode, int numSinks);

extern void
pruneTree (MadNode* nodes, MadNode* lastNode, PrioQueue* queue);

extern int convertToGlobal();

extern void 
MAD (int, TNET*, float, int, int, TNODE*, int*);

extern void 
IMAD_q (int, TNET*, float, int, int, TNODE*, int, float);

extern void 
MAD_base (int, TNET*, float, int, int, TNODE*, int*, float=0.0);

extern int
calculateCost (NodeCost* srcCost, MadEdge* edge, NodeCost* retCost);

extern int 
updateCost (MadNode* source, MadEdge* edge, MadNode* dest);


extern int 
updateTreeForSink (PrioQueue* queue, MadNode* sinkNode);

extern int
setCost (MadNode* fromNode, MadEdge* edge, MadNode* toNode);

extern int
setFinalCost (MadNode* fromNode, MadEdge* edge, MadNode* toNode);

extern int
compareCost (NodeCost* cost1, NodeCost* cost2);

extern void
printSubTree (TNODE* Node_All, MadNode* node);

extern int
CheckNode (MadNode* curNode);

extern void 
freeNodeGraph (MadNode *nodes, int N_All, int incremental);

#endif  /* MAD_H */
