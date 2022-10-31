#include "routing.h"

//Global variables
extern TLAYER *Layer;
extern TGLOBALNODE *GlobalNode; //global nodes array
extern TNET *Net_Tree;          //array with Steiner tree information
extern TNET **TreeList;         //Array Q^s
extern int Number_of_Nets;
extern int *Trees_in_Q;         //array, Trees_in_Q[s]=cardinality of Q^s
extern int Number_of_glob_nodes;        //number of nodes in global graph
extern int Size_X, Size_Y;      //plain dimension
extern short *Selected;
extern int SelectedNumber;

TNODELIST **GE;                 //GE[s] - list of global edges for tree s

//------------------------------------------------------------------------------
// Local function

void
AddGE (int t, int NewEdge)      //Add global edge in list GE[t]
{
  TNODELIST *Node1, *CurNode;

  CurNode = (TNODELIST *) calloc (1, sizeof (TNODELIST));
  CurNode->Node = NewEdge;
  CurNode->next = NULL;
  Node1 = GE[t];
  if (Node1 != NULL) {
    CurNode->next = Node1;
    GE[t] = CurNode;
  } else {                      //CurNode->next=NULL;
    GE[t] = CurNode;
  }
}

//------------------------------------------------------------------------------
void
Gradient (float accuracy)       // selects one tree from each Q^s=TreeList[s]
                              // by gradient method
{
  int *NetAddr;                 // [NetAddr[s-1],NetAddr[s])- trees for network s
  int *OldNum;                  // OldNum[s] - initial index of network s
  int *B;                       // Resources of global edges
  short *IndGE;                 // IndGE[k]=1 if global edge k is used by at least one tree
  TNET **Trees;                 // array of pointers for all trees
  TNET *CurTree;
  int news;                     // number of nets in Gradient method
  int NumTrees;                 //number of trees in Gradient method
  int s, t, curind, NTrees;
  int i, j, k, v1, v2;
  short New, Pred_dir, Stop, FirstRec, FirstIntRec;
  float *VarX, *VarY, *VarZ, *G;
  int *VarYInt;
  float LBound, NewBound, FCur;
  int FCurInt, FBest;
  float CurX, CurG, GA, GAmin, GZ, GY, ZZ;
  float StepT, UmStepT;
  int *JS, *BestX;
  TNODELIST *CurEdge;
  int NumIter;


  printf ("%s \n", "Start Gradient");

  OldNum = (int *) calloc (Number_of_Nets + 1, sizeof (int));
  NetAddr = (int *) calloc (Number_of_Nets + 1, sizeof (int));

  //if Trees_in_Q[s]==1 take the unique tree Net_Tree[s].
  for (s = 1; s <= Number_of_Nets; s++)
    if (Selected[s] != 1 && Trees_in_Q[s] == 1) {
      Net_Tree[s] = *TreeList[s];
      Selected[s] = 1;
      SelectedNumber++;
      GlobalResource (s);       //recalculate resources after selection of Net_Tree[s]
    }

  news = 0;
  NetAddr[0] = 0;
  for (s = 1; s <= Number_of_Nets; s++) {
    NTrees = Trees_in_Q[s];
    if (Selected[s] != 1 && NTrees > 1) {
      news++;
      OldNum[news] = s;
      NetAddr[news] = NetAddr[news - 1] + NTrees;
    }
  }

  NumTrees = NetAddr[news];
  Trees = (TNET **) calloc (NumTrees + 1, sizeof (TNET **));
  curind = 0;
  for (j = 1; j <= news; j++) {
    s = OldNum[j];
    NTrees = Trees_in_Q[s];
    CurTree = TreeList[s];
    for (i = 1; i <= NTrees; i++) {
      Trees[curind] = CurTree;
      CurTree = CurTree->next;
      curind++;
    }
  }

  GE = (TNODELIST **) calloc (NumTrees + 1, sizeof (TNODELIST **));
  for (s = 0; s <= NumTrees; s++)
    GE[s] = NULL;

  B = (int *) calloc (Number_of_glob_nodes + 1, sizeof (int));
  IndGE = (short *) calloc (Number_of_glob_nodes + 1, sizeof (short));
  for (k = 0; k <= Number_of_glob_nodes; k++)
    IndGE[k] = 0;

  //calculate list of global edges for each tree
  for (t = 0; t < NumTrees; t++) {
    CurTree = Trees[t];
    for (j = 1; j <= CurTree->Number; j++)      //j is the # of node in tree
      if (CurTree->Sink[j].pred > 0) {
        New = 1;
        for (i = 1; i < j; i++)
          if (CurTree->Sink[j].x == CurTree->Sink[i].x &&
              CurTree->Sink[j].y == CurTree->Sink[i].y &&
              CurTree->Sink[j].z == CurTree->Sink[i].z)
            New = 0;
        if (New == 1) {
          v1 = CurTree->Sink[j].pred;
          v2 = CurTree->Sink[j].No;
          Pred_dir = CurTree->Sink[j].pred_dir;
          if (v1 != v2 && Pred_dir < 5)
            switch (Pred_dir) {
            case west:
              for (k = v1; k < v2; k++) {
                AddGE (t, k);
                if (IndGE[k] == 0) {
                  IndGE[k] = 1;
                  B[k] = GlobalNode[k].EastResource;
                }
              }
              break;
            case north:
              k = v2;
              do {
                AddGE (t, k);
                if (IndGE[k] == 0) {
                  IndGE[k] = 1;
                  B[k] = GlobalNode[k].NorthResource;
                }
                k = k + Size_X + 1;
              } while (k != v1);
              break;
            case east:
              for (k = v2; k < v1; k++) {
                AddGE (t, k);
                if (IndGE[k] == 0) {
                  IndGE[k] = 1;
                  B[k] = GlobalNode[k].EastResource;
                }
              }
              break;
            case south:
              k = v1;
              do {
                AddGE (t, k);
                if (IndGE[k] == 0) {
                  IndGE[k] = 1;
                  B[k] = GlobalNode[k].NorthResource;
                }
                k = k + Size_X + 1;
              } while (k != v2);
              break;
            }                   //end switch
        }                       //end if
      }                         //end for j
  }                             //for (t=0;t<NumTrees;t++)

  // start Gradient method
  // initialization of variables
  VarX = (float *) calloc (NumTrees + 1, sizeof (float));
  VarY = (float *) calloc (Number_of_glob_nodes + 1, sizeof (float));
  VarYInt = (int *) calloc (Number_of_glob_nodes + 1, sizeof (int));
  VarZ = (float *) calloc (Number_of_glob_nodes + 1, sizeof (float));
  G = (float *) calloc (Number_of_glob_nodes + 1, sizeof (float));
  JS = (int *) calloc (news + 1, sizeof (int));
  BestX = (int *) calloc (news + 1, sizeof (int));

  for (i = 1; i <= Number_of_glob_nodes; i++)
    VarY[i] = 0.0;

  for (s = 1; s <= news; s++) {
    CurX = 1.0 / Trees_in_Q[OldNum[s]];
    for (j = NetAddr[s - 1]; j < NetAddr[s]; j++) {
      VarX[j] = CurX;
      CurEdge = GE[j];
      do {
        VarY[CurEdge->Node] += CurX;
        CurEdge = CurEdge->next;
      } while (CurEdge != NULL);
    }
  }

  LBound = 0.0;
  FirstIntRec = 1;

  // main loop
  Stop = 0;
  NumIter = 0;
  do {
    // calculate g_i, f(y), and GY
    FCur = 0.0;
    GY = 0.0;
    NumIter++;
    for (i = 1; i <= Number_of_glob_nodes; i++)
      if (IndGE[i] > 0) {
        CurG = VarY[i] - B[i];
        if (CurG > 0) {
          G[i] = CurG;
          GY += CurG * VarY[i];
          FCur += CurG * CurG;
        } else
          G[i] = 0.0;
      }
    // calculate GA, j_s, and GZ
    GZ = -GY;
    for (s = 1; s <= news; s++) {       //calculate j_s and GA_js for each net s
      FirstRec = 1;
      for (j = NetAddr[s - 1]; j < NetAddr[s]; j++) {
        GA = 0.0;
        CurEdge = GE[j];
        do {                    // calculate GA_j
          GA += G[CurEdge->Node];
          CurEdge = CurEdge->next;
        } while (CurEdge != NULL);
        if (FirstRec == 1) {
          FirstRec = 0;
          GAmin = GA;
          JS[s] = j;
        } else if (GA < GAmin) {
          GAmin = GA;
          JS[s] = j;
        }
      }
      GZ += GAmin;
    }

    // evaluate current integer solution based on j_s
    for (i = 1; i <= Number_of_glob_nodes; i++)
      VarYInt[i] = 0;
    for (s = 1; s <= news; s++) {
      j = JS[s];
      CurEdge = GE[j];
      do {
        VarYInt[CurEdge->Node]++;
        CurEdge = CurEdge->next;
      } while (CurEdge != NULL);
    }
    FCurInt = 0;
    for (i = 1; i <= Number_of_glob_nodes; i++)
      if (IndGE[i] > 0) {
        t = VarYInt[i] - B[i];
        if (t > 0) {            // выбор типа функционала для целочисленного решения
          // - оставить одну из трех строк!
//          FCurInt+=t*t;
//          FCurInt+=t;
          if (t > FCurInt)
            FCurInt = t;
        }
      }
    if (FirstIntRec == 1) {     // define first integer solution
      FirstIntRec = 0;
      FBest = FCurInt;
      for (s = 1; s <= news; s++)
        BestX[s] = JS[s];
    } else if (FCurInt < FBest) {       // change best unteger solution
      FBest = FCurInt;
      for (s = 1; s <= news; s++)
        BestX[s] = JS[s];
    }
    // calculate new lower bound
    NewBound = FCur + 2 * GZ;
    if (NewBound > LBound)
      LBound = NewBound;

    // check stop criteria
    if (FCur < 1)
      CurX = accuracy;
    else
      CurX = accuracy * FCur;
    if ((FCur - LBound) <= CurX)
      Stop = 1;
    else {                      // calculate Z, t, and go to new point
      for (i = 1; i <= Number_of_glob_nodes; i++)
        VarZ[i] = 0.0;
      for (s = 1; s <= news; s++) {
        i = JS[s];
        for (j = NetAddr[s - 1]; j < NetAddr[s]; j++) {
          if (j == i)
            CurX = 1 - VarX[j];
          else
            CurX = -VarX[j];
          CurEdge = GE[j];
          do {
            VarZ[CurEdge->Node] += CurX;
            CurEdge = CurEdge->next;
          } while (CurEdge != NULL);
        }
      }

      ZZ = 0.0;
      for (i = 1; i <= Number_of_glob_nodes; i++)
        if (IndGE[i] > 0)
          ZZ += VarZ[i] * VarZ[i];
      CurX = -GZ / ZZ;
      if (CurX < 1)
        StepT = CurX;
      else
        StepT = 1.0;

      // calculate new y_i
      for (i = 1; i <= Number_of_glob_nodes; i++)
        if (IndGE[i] > 0)
          VarY[i] += VarZ[i] * StepT;

      // calculate new x_j
      UmStepT = 1.0 - StepT;
      for (s = 1; s <= news; s++) {
        i = JS[s];
        for (j = NetAddr[s - 1]; j < NetAddr[s]; j++) {
          VarX[j] *= UmStepT;
          if (j == i)
            VarX[j] += StepT;
        }
      }
    }                           // calculate delta, t, and go to new point

  } while (Stop == 0);

  // try rounding solution to get the best one
  for (s = 1; s <= news; s++) { //find max x_s for each net s
    GA = 0.0;
    for (j = NetAddr[s - 1]; j < NetAddr[s]; j++)
      if (VarX[j] > GA) {
        GA = VarX[j];
        JS[s] = j;
      }
  }
  // evaluate current integer solution based on j_s
  for (i = 1; i <= Number_of_glob_nodes; i++)
    VarYInt[i] = 0;
  for (s = 1; s <= news; s++) {
    j = JS[s];
    CurEdge = GE[j];
    do {
      VarYInt[CurEdge->Node]++;
      CurEdge = CurEdge->next;
    } while (CurEdge != NULL);
  }
  //FCurInt = 0.0;
  FCurInt = 0;
  for (i = 1; i <= Number_of_glob_nodes; i++)
    if (IndGE[i] > 0) {
      t = VarYInt[i] - B[i];
      if (t > 0) {              // выбор типа функционала для целочисленного решения
        // - оставить одну из трех строк!
//          FCurInt+=t*t;
//          FCurInt+=t;
        if (t > FCurInt)
          FCurInt = t;
      }
    }
  if (FCurInt < FBest) {        // change best unteger solution
    FBest = FCurInt;
    for (s = 1; s <= news; s++)
      BestX[s] = JS[s];
  }

  printf ("%s \n", "Gradient finished ");
  printf ("%s %d\n", "Number of iterations =", NumIter);
  printf ("%s %d\n", "Integer Solution =", FBest);
  printf ("%s %f\n", "Fractional Solution =", FCur);

  // recalculate resources after selection of Net_Tree[s]
  for (s = 1; s <= news; s++) {
    i = OldNum[s];
    Net_Tree[i] = *Trees[BestX[s]];
    GlobalResource (i);
  }

}                               //void Gradient()
