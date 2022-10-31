#include <assert.h>
#include "prioQueue.h"
#include "mad.h"
#include "extern.h"

// XXX: Could not figure out who is allocating this, be careful
int *Tree_pred;        //Tree_pred[j] is the predecessor node for j

// This is stop going into infinite loop. Also, this value should be equal to 
// the maximum capacity of an edge, should not be required
#define MIN_MIN_RESOURCE -100


// static MadNodeQueue lmQueue = {0, NULL, NULL, 0};
static PrioQueue lQueue;

void printEdgeCost (NodeCost* srcCost, MadEdge* edge);

void
MAD (int s, TNET * CurNet, float r_0, int N, int N_All, TNODE * Node_All,
     int *PMinResource)
{
  /* Clean up the existing node cap values, and initialize */
  return MAD_base (s, CurNet, r_0, N, N_All, Node_All, PMinResource);
}

#ifdef USE_NEW_IMAD
void
IMAD_q (int s, TNET * CurNet, float r_0, int N, int N_All, TNODE * Node_All,
     int PMinResource, float cap_weight)
{

  return MAD_base (s, CurNet, r_0, N, N_All, Node_All, &PMinResource, cap_weight);
}
#endif

void
MAD_base (int s, TNET * CurNet, float r_0, int N, int N_All, TNODE * Node_All,
     int *PMinResource, float cap_weight)
{
  MadNode* nodes;
  MadNode* rootNode;
  int      i, u, v, k;
  int      minCapacity;
  int      maxCapacity;
  int      minResource = *PMinResource;
  MadEdgeCost* edge;
  MadEdge *newEdge, *nextEdge;
  float    dist;
  unsigned short int* degree;
  int      sinkCount;
  int      status;
  float    *lastSubTreeCapWeight;

  int edgeCount;

  // EnterFunc(MAD_base);
  nodes = (MadNode *) malloc ((N_All+1) * sizeof(MadNode));
  for (i = 1; i <= N_All; i++) {
    nodes[i].cost = (NodeCost*) malloc (sizeof(NodeCost));
  }

  /****
  if (cap_weight > 0.0) {
    lastSubTreeCapWeight = (float*) malloc ((N_All+1)*sizeof(float));
    for (i=1; i <= N_All ; i++) {
      if (Tree_pred[i] > 0) {
        nodes[i].cost->lastTreeCapWeight = Node_All[i].SubtreeCap * cap_weight;
      } else {
        nodes[i].cost->lastTreeCapWeight = 0.0;
      }
    }
  }
  ****/

  /* MinResource is the starting point for us to use only the nodes 
     which has more that given capacity. If we can not find a complete tree
     we need to take more congested area and then create the tree. 
  */
  do { 
#ifndef NDEBUG
    printf ("****** Processing net %d @ %d resource *******\n", s, minResource);
#endif
    sinkCount = 0;
    status = 0;
    edgeCount = 0;
    rootNode = NULL;

    for (i = 1; i <= N_All; i++) {
      nodes[i].nodeId =  Node_All[i].No; 
      nodes[i].nodeIndex = i;
      nodes[i].edgeList = NULL;
      nodes[i].status = MAD_TREE_FREE ;
      nodes[i].sinkCount = 0;
    }

    for (i = 1; i <= N_All; i++) {

      /*
      if (Node_All[i].type >= 0) {
        printf("Adding Node %d, (%d,%d,%d) type %s\n",
             nodes[i].nodeIndex, Node_All[i].x, Node_All[i].y, Node_All[i].z,
             Node_All[i].type == 0? "source" :  "sink");
      }
      */
      nodes[i].predEdge = NULL;
      INIT_NODECOST(nodes[i].cost);
      nodes[i].cost->cap = Node_All[i].cap; 
      if (Tree_pred[i] > 0 && Tree_pred[i] != i) 
        nodes[i].cost->lastTreeCapWeight = 
          Node_All[i].SubtreeCap * cap_weight;

      // nodes[i].cost->treeCap = nodes[i].cost->cap;
  
      if (Node_All[i].type == 0) {
        rootNode = &nodes[i];
        nodes[i].cost->treeRes = r_0;
        nodes[i].cost->treeCost = r_0 * nodes[i].cost->cap;
      }
  
      if (Node_All[i].type == 1) {
        for (k = 1; k <= N; k++) {
          if (Node_All[i].x == CurNet->Sink[k].x && 
              Node_All[i].y == CurNet->Sink[k].y && 
              Node_All[i].z == CurNet->Sink[k].z) {
                nodes[i].sinkCount++;
                sinkCount++;
              }
        }
      }
  
      // node updated before this node in BFS
      nodes[i].lastNode = NULL;

#ifndef NDEBUG
      // printf ("Node %d (ID %d) :(%d,%d,%d)\n", i, nodes[i].nodeId, Node_All[i].x, Node_All[i].y, Node_All[i].z);
#endif

      // check and create an edge at its north side
      if (Node_All[i].n > 0) {
        v = Node_All[i].n;
        minCapacity = MAX_INT;
        maxCapacity = -1 * MAX_INT;
  
        for (k = Node_All[i].No; k < Node_All[v].No; k += Size_X +1) {
          if (GlobalNode[k].NorthResource < minResource)
          {
            /* 
            printf ("NS edge %d - %d capacity (global %d, %d) fails min resource check\n",  Node_All[i].No, Node_All[Node_All[i].n].No, k, GlobalNode[k].NorthResource);
            */
            break; 
          } else {
            if (GlobalNode[k].NorthResource < minCapacity)
              minCapacity = GlobalNode[k].NorthResource;
  
            if (GlobalNode[k].NorthResource > maxCapacity)
              maxCapacity = GlobalNode[k].NorthResource;
          }
        }
  
        if (k >= Node_All[v].No) {
          int lay;
          // create an edge to its north neighbour
          edge = (MadEdgeCost*) malloc (sizeof(MadEdgeCost));
          lay = Node_All[i].z;
          dist = Node_All[v].y - Node_All[i].y;
          edge->capVal = Layer[lay].cap * dist;
          edge->resVal = Layer[lay].res * dist;
          edge->delay = 0.0;
          edge->minCapacity = minCapacity;
          edge->maxCapacity = maxCapacity;
          edge->parentNode = NULL;
 
          /* 
          printf("NS Edge between %d(%d) and %d(%d)\n", nodes[i].nodeIndex,
                 nodes[i].nodeId, nodes[v].nodeIndex, nodes[v].nodeId);
          */
          
          // add the edge in both the nodes
          newEdge = (MadEdge*) malloc (sizeof (MadEdge));
          newEdge->sourceNode = &nodes[i];
          newEdge->otherNode = &nodes[v];
          newEdge->direction = north;
          newEdge->nextEdge = nodes[i].edgeList;
          nodes[i].edgeList = newEdge;
          newEdge->cost = edge;
  
          newEdge = (MadEdge*) malloc (sizeof (MadEdge));
          newEdge->sourceNode = &nodes[v];
          newEdge->otherNode = &nodes[i];
          newEdge->direction = south;
          newEdge->nextEdge = nodes[v].edgeList;
          nodes[v].edgeList = newEdge;
          newEdge->cost = edge;
  
          edgeCount++;
        }
      }
    
      // Now create the edges in east direction
      if (Node_All[i].e > 0) {
        v = Node_All[i].e;
        minCapacity = MAX_INT;
        maxCapacity = -1 * MAX_INT;
  
        for (k = Node_All[i].No; k < Node_All[v].No; k++) {
          if (GlobalNode[k].EastResource < minResource) {
            /*
            printf ("EW edge %d - %d capacity (global %d, %d) fails min resource check\n",  Node_All[i].No, Node_All[Node_All[i].e].No, k, GlobalNode[k].EastResource);
            */

            break; 
          } else {
            if (GlobalNode[k].EastResource < minCapacity)
              minCapacity = GlobalNode[k].EastResource;
  
            if (GlobalNode[k].EastResource > maxCapacity)
              maxCapacity = GlobalNode[k].EastResource;
          }
        }
  
        if (k >= Node_All[v].No) {
          int lay;
  
          /* 
          printf("EW Edge between %d(%d) and %d(%d)\n", 
                 nodes[i].nodeIndex, nodes[i].nodeId, 
                 nodes[v].nodeIndex, nodes[v].nodeId);
          */
          
  
          // create an edge to its east neighbour
          edge = (MadEdgeCost*) malloc (sizeof(MadEdgeCost));
          lay = Node_All[i].z;
          dist = Node_All[v].x - Node_All[i].x;
          edge->capVal = Layer[lay].cap * dist;
          edge->resVal = Layer[lay].res * dist;
          edge->delay = 0.0;
          edge->minCapacity = minCapacity;
          edge->maxCapacity = maxCapacity;
          edge->parentNode = NULL;
  
          // add the edge in both the nodes
          newEdge = (MadEdge*) malloc (sizeof (MadEdge));
          newEdge->sourceNode = &nodes[i];
          newEdge->otherNode = &nodes[v];
          newEdge->direction = east;
          newEdge->nextEdge = nodes[i].edgeList;
          nodes[i].edgeList = newEdge;
          newEdge->cost = edge;
    
          newEdge = (MadEdge*) malloc (sizeof (MadEdge));
          newEdge->sourceNode = &nodes[v];
          newEdge->otherNode = &nodes[i];
          newEdge->direction = west;
          newEdge->nextEdge = nodes[v].edgeList;
          nodes[v].edgeList = newEdge;
          newEdge->cost = edge;
  
          edgeCount++;
        }
      }
    
      if (Node_All[i].u > 0) {
        v = Node_All[i].u;
        minCapacity = MAX_INT;
        maxCapacity = -1 * MAX_INT;
  
        float res = 0.0;
        float cap = 0.0;
  
        for (k = Node_All[i].No; k < Node_All[v].No; k += (Size_X+1)*(Size_Y+1)) {
          if (GlobalNode[k].UpResource < minResource) { 
            /* 
            printf ("UD edge %d - %d capacity (global %d, %d) fails min resource check\n",  Node_All[i].No, Node_All[Node_All[i].u].No, k, GlobalNode[k].UpResource);
            */
            break; 
          } else {
            if (GlobalNode[k].UpResource < minCapacity)
              minCapacity = GlobalNode[k].UpResource;
  
            if (GlobalNode[k].UpResource > maxCapacity)
              maxCapacity = GlobalNode[k].UpResource;
          }
        }
  
        if (k >= Node_All[v].No) {
          int lay;
  
          /*
          printf("UD Edge between %d(%d) and %d(%d)\n", 
                 nodes[i].nodeIndex, nodes[i].nodeId, 
                 nodes[v].nodeIndex, nodes[v].nodeId);
          */
          
  
          // create an edge to its up neighbour
          edge = (MadEdgeCost*) malloc (sizeof(MadEdgeCost));
          edge->delay = 0.0;
          edge->minCapacity = minCapacity;
          edge->maxCapacity = maxCapacity;
          edge->parentNode = NULL;
         
          edge->capVal = 0.0;
          edge->resVal = 0.0;
  
          for (lay = Node_All[i].z; lay < Node_All[v].z; lay++) {
            edge->capVal += Layer[lay].up_cap;
            edge->resVal += Layer[lay].up_res;
          }
  
          // add the edge in both the nodes
          newEdge = (MadEdge*) malloc (sizeof (MadEdge));
          newEdge->sourceNode = &nodes[i];
          newEdge->otherNode = &nodes[v];
          newEdge->direction = up;
          newEdge->nextEdge = nodes[i].edgeList;
          nodes[i].edgeList = newEdge;
          newEdge->cost = edge;
  
          newEdge = (MadEdge*) malloc (sizeof (MadEdge));
          newEdge->sourceNode = &nodes[v];
          newEdge->otherNode = &nodes[i];
          newEdge->direction = down;
          newEdge->nextEdge = nodes[v].edgeList;
          nodes[v].edgeList = newEdge;
          newEdge->cost = edge;

#ifndef NDEBUG
          if (CheckNode(nodes + v) > 0) {
            printf("Failed node %d, (%d,%d,%d)\n", nodes[v].nodeId, 
                    Node_All[nodes[v].nodeIndex].x, Node_All[nodes[v].nodeIndex].y, 
                    Node_All[nodes[v].nodeIndex].z);
            printf("Current node %d, (%d,%d,%d)\n", nodes[i].nodeId, 
                    Node_All[nodes[i].nodeIndex].x, Node_All[nodes[i].nodeIndex].y, 
                    Node_All[nodes[i].nodeIndex].z);
          }
#endif
  
          edgeCount++;
        }  // there is an edge upwards
      }    // up neighbor exists

#ifndef NDEBUG
      CheckNode(nodes + i);
#endif
    }      // for each node

#ifndef NDEBUG  
    printf ("Total #of nodes %d, # of edges %d\n", N_All, edgeCount);
#endif
  
    // Now call Dijkstra to actually create the tree
    status = runDijkstra(nodes, rootNode, sinkCount);
  
    fflush(stdout);
    if (!status) {
      --minResource;
      // printf("Could not get a tree with current minimum resources, reducing to %d\n", minResource);

      // free up the edges
      freeNodeGraph (nodes, N_All, 1);
    }
  } while (status == 0 && minResource > MIN_MIN_RESOURCE);
  // while (status == 0 && minResource > 0);

  // update this only for the non-iteration mode
  if (cap_weight <= 1e-10)
    *PMinResource = minResource;

  if (!status) {
    printf ("Could not even get a routing with %d minResource\n", minResource);
    return;
  }

#ifndef NDEBUG
  // printSubTree (Node_All, rootNode);
#endif

  //------------------------------------------------------------------------
  //prune the leaf paths and get the final Tree_pred[i]
  degree = (unsigned short int*)malloc ((N_All+1)*sizeof(unsigned short int));
 
  for (i = 1; i <= N_All; i++) {
    Tree_pred[i] = -1;
    Node_All[i].pred_dir = norout;
    degree[i] = 0;
  }

  // Copy the predecessor information into global variables, calculate degree
  for (i = 1; i <= N_All; i++) {
    if (InSinkPath(nodes+i) && nodes[i].predEdge) {
      Node_All[i].pred = Tree_pred[i] =  
          nodes[i].predEdge->sourceNode->nodeIndex;

      switch (nodes[i].predEdge->direction) {
        case east : Node_All[i].pred_dir = west; break;
        case west : Node_All[i].pred_dir = east; break;
        case north : Node_All[i].pred_dir = south; break;
        case south : Node_All[i].pred_dir = north; break;
        case up : Node_All[i].pred_dir = down; break;
        case down : Node_All[i].pred_dir = up; break;
      }
      degree[Tree_pred[i]]++;
    } else {
      Node_All[i].pred = Tree_pred[i] = -1 ;
    } 
  }

  // root node predecessor is itself, so mark it
  i = rootNode->nodeIndex;
  Tree_pred[i] = i;
  Node_All[i].pred_dir = norout ;

  //delete intermediate nodes in simple paths
  for (i = 1; i <= N_All; i++)
    if (Node_All[i].type == 1)    //node i is sink
    {
      v = i;
      do {
        int Pred_dir;
        u = Tree_pred[v];
        Pred_dir = Node_All[v].pred_dir;
        while (degree[u] < 2 && Node_All[u].pred_dir == Pred_dir && 
               Node_All[u].type < 0)
        {
          // u is intermediate and degree[u]=1
          Tree_pred[v] = Tree_pred[u];
          Tree_pred[u] = -1;
          u = Tree_pred[v];
        }
        v = u;
      } while (nodes[v].predEdge);
    }

  // Update the Node_All array as required to plugin the code.
  for (i=1; i <= N_All; i++) {
    Node_All[i].delay = nodes[i].cost->treeCost;
    Node_All[i].SubtreeCap = nodes[i].cost->treeCap;
    if (nodes[i].predEdge) {
      Node_All[i].EdgeDelay = nodes[i].predEdge->cost->delay;
    } else {
      Node_All[i].EdgeDelay = nodes[i].cost->treeCost;
    }
  }

#ifndef NDEBUG
  printf("***** Tree found for net %d min resource %d %s ********\n", s, minResource, cap_weight > 0.0 ? "IMAD" : "MAD");
  PRINT_TNODE(Node_All, N_All, i)
#endif
  freeNodeGraph (nodes, N_All, 0);
  free (degree);
}

void 
freeNodeGraph (MadNode *nodes, int N_All, int incremental)
{
  MadEdge *edge, *nextEdge;
  int     i;

  for (i=1; i <= N_All; i++) {
    // free the cost, if it is not incremental 
    if (!incremental)
      free (nodes[i].cost);

    edge = nodes[i].edgeList;
    while (edge) {
      nextEdge = edge->nextEdge;

      // As the edgecost is shared by two nodes, make sure the first does 
      // not free it
      switch (edge->direction) {
        case east :
        case north :
        case up : free (edge);
             break;

        case west :
        case south :
        case down : free(edge->cost);
               free(edge);
               break;
      }
      edge = nextEdge;
    }
  }

  if (!incremental)
    free (nodes);
}

static int 
compPrioNode (void* elem1, void* elem2)
{
  return compareCost (((MadNode*)elem1)->cost, ((MadNode*)elem2)->cost); 
}

static int 
getPrioPos (void *elem) 
{ 
  return ((MadNode*)elem)->queuePos;
}

static int 
setPrioPos (void *elem, int newPos)
{
  ((MadNode*)elem)->queuePos = newPos;
  return 1;
}

static int
printNode (void *elem)
{
  MadNode* node = (MadNode*)elem;
  printf("index %d, Id %d, cost %f,",  node->nodeIndex, node->nodeId, node->cost->treeCost);
  if (InQueue(node)) 
    printf(" queue position %d, ", node->queuePos); 
  else {
    if (InSinkPath(node)) 
      printf (" in sink path, ");
    else if (InTree(node)) 
      printf(" tree node, ");
  }
  return 1;
}


int 
runDijkstra (MadNode* nodes, MadNode* rootNode, int numSinks)
{
  int       sinksFound = 0;
  MadNode*  curNode = NULL;
  MadEdge*  edge = NULL;
  MadNode*  otherNode = NULL;
  static PrioQueue* queue = NULL;
  MadNode*  lastNode = NULL;

  // EnterFunc(runDijkstra);

  MadNode* tree;

  if (queue == 0) {
    queue = &lQueue;
    InitPrioQueue (queue, compPrioNode, getPrioPos, setPrioPos, printNode);
  }

  assert (queue->qSize == 0);

  // find the root of the tree, push the tree in the prioQueue
  prioQueueAdd (queue, rootNode);

  // now run the dijkstra
  while (queue->qSize > 0) {

#ifndef NDEBUG
#ifdef DEBUG_DIJKSTRA
    printf("Current status of queue - will pop the top\n");
    prioQueuePrint (queue);
#endif
#endif

    curNode = (MadNode*)prioQueueHead(queue);

#ifndef NDEBUG
    // Making sure the graph does not have any bad property
    CheckNode(curNode);

#ifdef DEBUG_DIJKSTRA
    printf ("Adding node %d (Id %d) in BFS tree, cost %f\n", 
            curNode->nodeIndex, curNode->nodeId, curNode->cost->treeCost);
#endif
#endif
    prioQueuePop(queue);
    ResetInQueue(curNode);
    SetInTree(curNode);
    curNode->lastNode = lastNode;
    lastNode = curNode;

    if (curNode->sinkCount > 0) {
      sinksFound += curNode->sinkCount;
      SetInTree(curNode);

      // a big task here - need to update the cost due to this sink, and
      // update most of the things upward
      updateTreeForSink (queue, curNode);
    }

    edge = curNode->edgeList;

    while (edge != NULL) {
      otherNode = edge->otherNode;

      if (curNode->predEdge && curNode->predEdge->sourceNode == otherNode) {
        // other node is parent of current node, so skip
        edge = edge->nextEdge;
        continue;
      }
      
#ifndef NDEBUG       
#ifdef DEBUG_DIJKSTRA
      char edgeDir = 'X';
      switch (edge->direction) {
        case 1 : edgeDir = 'W'; break ;
        case 2 : edgeDir = 'N'; break ;
        case 3 : edgeDir = 'E'; break ;
        case 4 : edgeDir = 'S'; break ;
        case 5 : edgeDir = 'U'; break ;
        case 6 : edgeDir = 'D'; break ;
        default : edgeDir = 'X'; break ;
      }
       
      printf ("Exploring edge (%d->%d) dir %c, edge cost (R %f, C %f), dest cost %f\n", 
              edge->sourceNode->nodeIndex, edge->otherNode->nodeIndex,
              edgeDir, edge->cost->resVal, edge->cost->capVal,
              otherNode->cost->treeCost);
#endif
#endif 
      

      if (InTree(otherNode)) {

#ifndef NDEBUG
  /*
        // There can be multiple path to a node, and some path may have larger cap
        // making alternate path faster.
        NodeCost cost;
        calculateCost (curNode->cost, edge, &cost);
        // assert (compareCost (&cost, otherNode->cost) > 0);
        if (compareCost (&cost, otherNode->cost) < 0) {
          printf ("Found an edge which could have reduce tree cost, so investigate\n");
          // print the source node, edge cost and destination node
          printf("Current node: "); printNode(curNode);
          printf(" :: Edge cost: "); printEdgeCost(curNode->cost, edge);
          printf(" :: Other node status: "); printNode(otherNode);
          printf ("\n");
          // exit(0);
        }
  */
#endif
        edge = edge->nextEdge;
        // printf ("node %d (Id %d) is already in tree\n", otherNode->nodeIndex, otherNode->nodeId);
        continue;
      }

      if (InQueue(otherNode)) {
#ifndef NDEBUG
#ifdef DEBUG_DIJKSTRA
        printf("Considering node %d (Id %d) with cost %f\n", otherNode->nodeIndex,
               otherNode->nodeId, otherNode->cost->treeCost);
#endif
#endif
        // in the queue, so still being explored, update the cost
        if (updateCost (curNode, edge, otherNode) < 0)  {
          prioReduceCost (queue, otherNode);
          otherNode->predEdge = edge;
#ifndef NDEBUG
#ifdef DEBUG_DIJKSTRA
          printf("relaxed cost of node %d (Id %d) to %f\n", otherNode->nodeIndex, 
                 otherNode->nodeId, otherNode->cost->treeCost);
          prioQueuePrint (queue);
#endif
#endif
        }
      } else {
        // new node, put it in the queue, after updating it
        setCost (curNode, edge, otherNode);
        SetInQueue(otherNode);
        prioQueueAdd (queue, otherNode);
        otherNode->predEdge = edge ;
#ifndef NDEBUG        
#ifdef DEBUG_DIJKSTRA
        printf("Adding new node %d (Id %d), cost %f in queue\n", otherNode->nodeIndex,
                otherNode->nodeId, otherNode->cost->treeCost);
        prioQueuePrint (queue);
#endif
#endif
        
      }
      edge = edge->nextEdge;
    }

    // if we have found all the sinks, then there is no need to conntinue
    if (sinksFound == numSinks)
      break; 
  }

  // error out if not all the sinks are found
  if (sinksFound < numSinks) {
    // printf ("Could not get all the sinks (%d out of %d)!!! Loss of connectivity\n",sinksFound, numSinks);
  } else {

    // prune the tree, and then clean up
    pruneTree (nodes, lastNode, queue);

    // In updateTreeForSink, we also consider the capacitance coming from the
    // edges/nodes which are in tentative tree, but not leading to a path to 
    // any sink. So unless that code also performs pruning, we need to update
    // the cost and caps after performing pruning.
    updateTreeForSink (queue, lastNode);
  }

  prioCleanup (queue);

  assert (queue->qSize == 0);
  return (sinksFound == numSinks);
}


// sinkNode is a sink node, and also the last node discovered 
// by Dijkstra algorithm.
int 
updateTreeForSink (PrioQueue* queue, MadNode* sinkNode)
{
  MadNode* curNode = sinkNode;
  MadNode* lastHead = NULL;
  MadNode* nextHead = NULL;
  MadNode* otherNode = NULL;
  MadEdge* edge = NULL;
  MadEdge* predEdge = NULL;
  int      i = 0;

#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
  printf ("Updating for sink node %d (Id %d)\n", sinkNode->nodeIndex, sinkNode->nodeId);
#endif
#endif

  while (curNode) {
#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
    printf("Current path node %d (Id %d), status %d", curNode->nodeIndex,
           sinkNode->nodeId, curNode->status);
#endif
#endif
    if (!InSinkPath(curNode)) {
      SetInSinkPath(curNode);
      SetInTree(curNode);
#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
      printf (" -> status %d\n", curNode->status);
#endif
#endif
      if (curNode->predEdge)
        curNode = curNode->predEdge->sourceNode;
      else
        break;
    } else {
#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
      printf (" -> in multiple sink path\n");
#endif
#endif
      break;
    }
  }

  // All the nodes in the queue are still not in the tree, but they must be
  // only one node away from the tree. So update the treeCap
  for (int i = 0; i < queue->qSize; i++) {
    curNode = (MadNode*)queue->elems[i];
    curNode->cost->treeCap = curNode->cost->cap;
  }

  // sink node does not have any child, so this is OK.
  // also we use the link traversal technique to use the same space 
  // for coming back again.
  nextHead = NULL;
  lastHead = sinkNode;
  while (lastHead) {
    // Reverse the link list using lastHead
    curNode = lastHead;
    lastHead = curNode->lastNode;
    curNode->lastNode = nextHead ;
    nextHead = curNode;
   
    // Now process the current node 
    curNode->cost->treeCap = curNode->cost->cap;

    
#ifndef NDEBUG     
#ifdef DEBUG_UPDATE_SINK
    printf ("Backward cap updt, node %d (Id %d), C %f, treeCap %f, cost %f", 
            curNode->nodeIndex, curNode->nodeId, curNode->cost->cap, 
            curNode->cost->treeCap, curNode->cost->treeCost);
#endif
#endif
    
    edge = curNode->edgeList;
    while (edge) {
      otherNode = edge->otherNode;
      // The node or branch cap will be considered only if the node is
      // in a path to sink node. This way we can give preference to 
      // paths which are using existing path to sink node
      // if (InTree(otherNode) || InQueue(otherNode))
      if (InSinkPath(otherNode)) {
        predEdge = otherNode->predEdge;
        if (predEdge && predEdge->sourceNode == curNode) {
          curNode->cost->treeCap += otherNode->cost->treeCap +
                     predEdge->cost->capVal;
#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
          printf (" - %d to %d (E %f + N %f) -> %f",  
                 predEdge->sourceNode->nodeIndex, otherNode->nodeIndex, 
                 predEdge->cost->capVal, otherNode->cost->treeCap, curNode->cost->treeCap);
#endif
#endif
        }
      }
      edge = edge->nextEdge;
    }
#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
    printf (" -> %f\n", curNode->cost->treeCap);
#endif
#endif
  }

  // update the cost of root node
  nextHead->cost->treeCost = nextHead->cost->treeRes * nextHead->cost->treeCap;

#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
  printf ("\nReached root node %d, R %f, C %f, Delay %f\n\n", 
          nextHead->nodeIndex, nextHead->cost->treeRes, 
          nextHead->cost->treeCap, nextHead->cost->treeCost);
#endif
#endif

  // All the nodes in the queue needs to be recalculated, but as 
  // they are already having a lower cost, just blindly updating
  // will not help. Here we are blindly setting the cost to be 
  // the maximum, and each relaxation from tree node will bring 
  // it to proper place. As we are setting all the nodes cost to max,
  // it still follows a Priority Queue
  for (i = 0 ; i < queue->qSize; i++) {
    curNode = (MadNode*)(queue->elems[i]);
    curNode->cost->treeCost = MAX_FLOAT;
  }

  // Now go from root to sinks/leafs, and update the cost
  while (nextHead) {
    // Now reset the lastNode pointer to the original form
    curNode = nextHead;
    nextHead = curNode->lastNode;
    curNode->lastNode = lastHead ;
    lastHead = curNode;

#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
    printf ("\nForward updating delay for sink, root to leaf from: ");
    printNode(curNode);
    printf(" tR %f, tC %f\n", curNode->cost->treeRes, curNode->cost->treeCap);
#endif
#endif

    edge = curNode->edgeList;
    while (edge) {
      otherNode = edge->otherNode;

      // otherNode is the sink node
      if (otherNode == curNode) continue;

#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
      printf("Updating through edge to node: ");
      printNode(otherNode);
#endif
#endif

      if (InTree(otherNode)) {
        // curNode is in tree, and otherNode is also in tree, then there 
        // are two possibilities, either curNode is parent of otherNode, or
        // vice versa. If otherNode is parent of this, it is already processed
        // as part of otherNode processing. We should handle the children of 
        // curNode now.
        predEdge = otherNode->predEdge;
	// assert(predEdge != NULL); This is false for root
        if (predEdge && predEdge->sourceNode == curNode) {
          // update the cost of the node, and adjust in the priority queue
          // as this moves the delay upward, the cost has to be directly set
          // and not updateCost, also treeCap is already updated for dest

          // if the other node is on sink path, then use tree cap based update
          // otherwise if it is queue/already processed node, use delay based 
          // on res + delta delay
          if (InSinkPath(otherNode))
            setFinalCost (curNode, edge, otherNode);
          else {
#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
            printf("Updating other nodes cost: ");
            printEdgeCost(curNode->cost, edge);
#endif
#endif
            calculateCost (curNode->cost, edge, otherNode->cost);
          } 
        }
      } else if (InQueue(otherNode)) {
        // As this is reachable from the tree, and otherNode is not in tree, 
        // it must be in the queue
        // assert(InQueue(otherNode));

        if (updateCost (curNode, edge, otherNode) < 0)  {
          prioReduceCost (queue, otherNode);
          otherNode->predEdge = edge;
#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
          printf("relaxed cost of node %d to %f\n", otherNode->nodeIndex, 
                 otherNode->cost->treeCost);
#endif
#endif
        }
      } else {

#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
        printf("Tree node is connected to a non tree/queue node, edge not updated\n");
#endif
#endif
      }
 
#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
      printNode(otherNode);
      printf("\n");
#endif
#endif

      edge = edge->nextEdge;
    } // for each edge from curNode

  }

#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
      // just making sure the tree is good
      printf("After sink update, queue status:\n");
      prioQueuePrint(queue);
      printf("\n");
#endif
#endif

  return 1;
}


// The cost of a destination node has the following components
// 1. The initial delay in the source node
// 2. Delay in the edge (edge res * (dest_cap + edge_cap/2))
// 3. Delay degradation in source node due to additional cap
//         res_to_root_from src_node * (dest_cap + edge_cap)
// 4. For incremental mode, we penalize the node which is already in 
//    the previous tree - the cap_wight determines by how much
//

int
calculateCost (NodeCost* srcCost, MadEdge* edge, NodeCost* retCost) 
{
  float   deltaDel, delDegrade, delPenalty;
  // edge delay, delay due to edge res/cap and out node's cap. 
  deltaDel = edge->cost->resVal * 
                (edge->cost->capVal/2 + edge->otherNode->cost->cap);

  // delay degrade at source node due to additional cap fromedge and dest node
  delDegrade = srcCost->treeRes * (edge->cost->capVal + edge->otherNode->cost->cap);

  // penalty for the previous tree participation in iterative mode, for non-iterative, the
  // cap value is 0.0. This is a heuristic and we can play with this penalty method
  delPenalty = edge->cost->resVal * edge->otherNode->cost->lastTreeCapWeight;

  retCost->treeCost = srcCost->treeCost + deltaDel + delDegrade + delPenalty;
  retCost->treeRes = srcCost->treeRes + edge->cost->resVal;
  return 1;
}

#ifndef NDEBUG
void
printEdgeCost (NodeCost* srcCost, MadEdge* edge)
{
  float   deltaDel, delDegrade, delPenalty;
  // edge delay, delay due to edge res/cap and out node's cap. 
  deltaDel = edge->cost->resVal * 
                (edge->cost->capVal/2 + edge->otherNode->cost->cap);

  // delay degrade at source node due to additional cap fromedge and dest node
  delDegrade = srcCost->treeRes * (edge->cost->capVal + edge->otherNode->cost->cap);

  // penalty for the previous tree participation in iterative mode, for non-iterative, the
  // cap value is 0.0. This is a heuristic and we can play with this penalty method
  delPenalty = edge->cost->resVal * edge->otherNode->cost->lastTreeCapWeight;

  printf (" EC: (ER=%f, EC=%f, tR=%f), dD %f, dDg %f, penalty %f = %f, Cost= %f -> %f,", 
          edge->cost->resVal, edge->cost->capVal, srcCost->treeRes, deltaDel, delDegrade, delPenalty, 
          deltaDel+delDegrade+delPenalty, srcCost->treeCost, 
          srcCost->treeCost + deltaDel + delDegrade + delPenalty);
}
#endif

// returns -1 if the cost has been reduced, otherwise +1
int 
updateCost (MadNode* source, MadEdge* edge, MadNode* dest)
{
  NodeCost cost;
  calculateCost (source->cost, edge, &cost);
  if (compareCost(dest->cost, &cost) > 0) {
    // update the dest cost
    edge->cost->delay = edge->cost->resVal * 
                (edge->cost->capVal/2 + edge->otherNode->cost->cap);
    dest->cost->treeCost = cost.treeCost;
    dest->cost->treeRes = cost.treeRes;
    return -1;
  }

  return 1;
}

int 
initCost (MadNode* source, MadEdge* edge, MadNode* dest)
{
  return calculateCost (source->cost, edge, dest->cost);
}

// This function assumes that each node has the treeRes and treeCap 
// updated, hence the calculation is easier
int 
setFinalCost (MadNode* fromNode, MadEdge* edge, MadNode* toNode)
{
  // edge delay, delay due to edge res/cap and out node's subtree cap. 
  edge->cost->delay = edge->cost->resVal * 
                (edge->cost->capVal/2 + edge->otherNode->cost->treeCap);

  // dest nodes cost is source node cost + edge cost
  toNode->cost->treeCost = fromNode->cost->treeCost + edge->cost->delay;
  toNode->cost->treeRes = fromNode->cost->treeRes + edge->cost->resVal;
#ifndef NDEBUG
#ifdef DEBUG_UPDATE_SINK
  printf(" FC: D=%f + SC %f = NC %f", edge->cost->delay, fromNode->cost->treeCost, toNode->cost->treeCost);
#endif
#endif
  return 1;
}

int 
setCost (MadNode* fromNode, MadEdge* edge, MadNode* toNode)
{
  calculateCost (fromNode->cost, edge, toNode->cost);
  edge->cost->delay = edge->cost->resVal * 
                (edge->cost->capVal/2 + edge->otherNode->cost->treeCap);
  return 1;
}

int
compareCost (NodeCost* node1, NodeCost* node2)
{
  
  if (node1->treeCost < node2->treeCost)
    return -1;
  else if (node1->treeCost > node2->treeCost)
    return 1;
  else 
    return 0;
}

// This function removes all the hanging nodes, which are not 
// in a path to sink
void
pruneTree (MadNode* nodes, MadNode* lastNode, PrioQueue* queue)
{
  MadNode *tempNode, *nextNode;
  MadNode *curNode = lastNode;

#ifndef NDEBUG
  int     i = 0;
#endif

#ifndef NDEBUG
    printf ("PruneTree::Removing nodes from queue (Id) : ");
#endif
  // all the nodes that are still in the queue, reset their prededge,
  // they are not accessible through lastNode links
  while (queue->qSize > 0) {
    curNode = (MadNode*)prioQueueHead (queue);
#ifndef NDEBUG
    i++;
    // printf ("%d, ", curNode->nodeId);
    // if (i % 10 == 0) printf ("\n");
#endif
    curNode->cost->treeRes = 0.0;
    curNode->cost->treeCost = 0.0;
    curNode->cost->treeCap = curNode->cost->cap;
    curNode->predEdge = NULL;
    ResetInQueue(curNode);
    prioQueuePop(queue);
  }

#ifndef NDEBUG
    printf ("\nTotal queue nodes %d\nRemoving nodes from tree (Id) : ", i);
    i = 0;
#endif

  curNode = lastNode;
  while (curNode) {
    nextNode = curNode->lastNode;

    while (nextNode && !InSinkPath(nextNode)) {
      // reset the cost
      nextNode->cost->treeRes = 0.0;
      nextNode->cost->treeCost = 0.0;
      nextNode->cost->treeCap = nextNode->cost->cap;
      nextNode->predEdge = NULL;

#ifndef NDEBUG
      i++;
      // printf ("%d, ", nextNode->nodeId);
      // if (i % 10 == 0) printf ("\n");
#endif
      // Now remove it from the tree
      ResetInTree(nextNode);
      tempNode = nextNode;
      nextNode = nextNode->lastNode;
      tempNode->lastNode = NULL;
    }
    curNode->lastNode = nextNode;
    curNode = curNode->lastNode;
  }
#ifndef NDEBUG
    printf ("\nTotal tree nodes removed %d\n", i);
#endif
}

#ifndef NDEBUG

// Checks the correcness of the node
int
CheckNode (MadNode* curNode)
{
  MadEdge* edges[7];
  MadEdge* edge;
  int i = 1;
  int k = 0;


  // There should not be more than 6 edges, and there should not be 
  // two edges in the same direction
  for (i=1; i<7; i++) edges[i] = NULL;

  edge = curNode->edgeList;
  while (edge != NULL) {
    if (edges[edge->direction] != NULL) {
      printf ("Node %d has multiple edges towards %d !!\n", curNode->nodeId, edge->direction);
      return edge->direction;
    } else {
      edges[edge->direction] = edge;
    }
    edge = edge->nextEdge;
    k++;
  }
  if (k > 6) {
    printf ("Node %d has %d edges!!\n",  curNode->nodeId, k);
    return k;
  }

  return 0;
}


void
printSubTree (TNODE* Node_All, MadNode* node)
{
  MadNode* otherNode = NULL;
  MadEdge* edge = node->edgeList;
  
  while (edge) {
    otherNode = edge->otherNode;
    if (InTree(otherNode) && otherNode->predEdge && 
        otherNode->predEdge->sourceNode == node) {
      printf ("Edge %d -> %d (R %f, C %f, D %f), nc %f %s\n",
              node->nodeId, otherNode->nodeId,
              edge->cost->resVal, edge->cost->capVal, edge->cost->delay, 
              otherNode->cost->treeCost, otherNode->sinkCount? "T" : "");

      printSubTree (Node_All, otherNode);
    }
    edge = edge->nextEdge;
  }
}
#endif
