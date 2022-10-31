#ifndef ROUTING_H
#define ROUTING_H

// Include basic Header File
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

// Define File Names
#define InpFName  "data/desmal.dat"            // Input file
//#define InpFName  "ibm01_mad.dat"            // Input file
#define OutFName1 "output/desmal_1.out"          // 1 output file
#define OutFName2 "output/desmal_2.out"          // 2 output file
#define OutFName3 "output/desmal_3.out"          // 3 output file
//-------------------------------------------------------------
// Global constants

#define Max_Number_of_Trees 8  //max number of trees in Q^s
//#define Max_Number_of_Trees 5  //max number of trees in Q^s(original)
//#define MaxIter 3              //max number of iterations(original)
#define MaxIter 11              //max number of iterations
//#define Max_qq 3              //number of q's values(original)
#define Max_qq 2              //number of q's values
#define MaxInpStrLength 80          //max string length in input file
#define MaxNameLength 25            //max name length
#define hor 1                       //routing directions 0x
#define ver 2                       //routing directions 0y
#define norout 0                    //no routing
#define west 1
#define north 2
#define east 3
#define south 4
#define up 5
#define down 6
#define MaxFloat 99999999.9    //big float
#define MaxInt   99999         //big int
#define Limit_Overflow -90     //max possible congestion overflow
//--------------------------------------------------------------
// Type definitions

typedef char InpStr[MaxInpStrLength];
typedef char NameStr[MaxNameLength];

// Layer description
typedef struct
{ short Id;       //layer's #
  short dir;      //direction of routing (1=hor or 2=ver)
  int   Tracks;   //max number of wires per bin
  int   UpTracks; //max number of wires per up via
  float cap;      //capacitance
  float res;      //resistance
  float up_cap;   //up via capacitance. I the 1st version = 0
  float up_res;   //up via resistance
} TLAYER;

// Global node description
typedef struct
{ int   x;             //x-coordinate
  int   y;             //y-coordinate
  short z;             //z-coordinate (layer #)
  int   Id;            //node's #
  int   EastResource;  //east edge capacity (resource)
  int   NorthResource; //north edge capacity (resource)
  int   UpResource;    //up edge capacity (resource)
} TGLOBALNODE;

// Node description (for net)
typedef struct
{ short PinId;      //Pin # in net
  int   No;         //node's # in global graph
  int   x;          //x-coordinate
  int   y;          //y-coordinate
  short z;          //z-coordinate (layer #)
  float cap;        //node capacitance=sum of capacirences of sinks in this node
  int   w;          //# of west neighbor node
  int   n;          //# of north neighbor node
  int   e;          //# of east neighbor node
  int   s;          //# of south neighbor node
  int   u;          //# of up neighbor node
  int   d;          //# of down neighbor node
  int   pred;       //# of predecessor node in tree. First all pred = -1
  short pred_dir;   //direction to the predecessor node (west,norht,...)
  short type;       //node's type (-1 - Hanan's node, 0 - root, 1 - sink)
  short critical;   //critical=1 if the sink is critical
  float delay;      //delay to node in the last routing tree
  float SubtreeCap; //total capacitance of subtree with root in the node
  float EdgeDelay;  //d_{ij} - edge delay pred[node]->node
  float D_k;        //D_k=\sum_{(i,j)\in P_k} d_{ij}
} TNODE;

#define PRINT_TNODE(tnodes, N, i) \
  for (i=1; i<=N; i++) { \
    if (tnodes[i].pred > 0) { \
      printf ("Edge %d %s -> %d %s (nc %f, C %f) (ec %f)\n", \
              tnodes[tnodes[i].pred].No, tnodes[tnodes[i].pred].type == 0 ? "(S)" : "", \
              tnodes[i].No, tnodes[i].type == 1 ? "(T)" : "(I)", tnodes[i].delay, \
              tnodes[i].SubtreeCap, tnodes[i].EdgeDelay); \
    } \
  }

// Net description
typedef struct tnetwork
{ int   Id;                      //net #
  float drive;                   //root resistance r_0
  float C_0;                     //total tree capacitance
  int   Number;                  //total number of sinks (nodes)
  float CriticalDelay;           //critical delay in tree
  float Delay_in_critical_sink;  //if net is critical, then this is delay in cr sink
  short critical;                //critical=1 if the net is critical
  TNODE *Sink;                   //array of sinks (nodes)
  struct tnetwork *next;         //next tree for the same net
} TNET;

typedef struct tnodelist //list of adjacent nodes
{ int              Node; //# of 1st adjacent node
  struct tnodelist *next;//pointer to the next adjacent node in list
} TNODELIST;

//------------------------------------------------------------------------------
// Function prototype declaration

//ReadData.cpp
void InitDataStructure (const char*);

//GenTrees.cpp
void Gen_Trees(int,TNET*);
void AddTree(int,TNET*);
void DeleteTree(int,TNET*);

//GlobalResource.cpp
void GlobalResource(int);

//MAD.cpp
void MAD(int,TNET*,float,int,int,TNODE*,int*);

//IMAD_q.cpp
void IMAD_q(int,TNET*,float,int,int,TNODE*,int,float);

//WriteSol.cpp
void WriteSol();

//Gradient.cpp
void Gradient(float);

//Debug.cpp
void PrintNode (TNODE* node, FILE* fptr);
void PrintNet (TNET* net, FILE* fptr);

#ifdef NDEBUG
#define EnterFunc(funcName)
#define EndFunc(funcName) 
#define PrintDebug(string) 
#else
#define EndFunc(funcName) printf("Exiting function " #funcName " %s:%d\n", __FILE__, __LINE__)
#define EnterFunc(funcName) printf("Entering function " #funcName " %s:%d\n", __FILE__, __LINE__)
#define PrintDebug(string) printf("%s", string);
#endif

#endif
