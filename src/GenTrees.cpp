#include <math.h>
#include "routing.h"

//Global variables
extern TLAYER *Layer;           //array with layer information
extern TNET *Net_Tree;          //array with Steiner tree information
extern TNET **TreeList;         //Array of pointers for Q^s
extern int *Tree_pred;          //Tree_pred[j] - predecessor of j  --> out
extern int Size_X, Size_Y;      //plain dimension
extern short Size_Z;            //number of layers
extern int *Trees_in_Q;         //array, Trees_in_Q[s]=cardinality of Q^s
int N, N_All;                   //N_All is the number of all nodes in Hanan graph

//------------------------------------------------------------------------------

void
Gen_Trees (int s, TNET * CurNet)        //input: CurNet - current net
{                               // Construction of trees
  TNODE *Node_All;              //array with information about all nodes in Hanan graph
  int i, j, k, no, GlobNo, A_All, e, VerLines, HorLines;
  int Root, CurNet_Id, Number_of_Trees;
  short *Line_X;                //Line_X[i]==1 if there is a sink in the west-east line i
  short *Line_Y;                //Line_Y[i]==1 if there is a sink in the north-south line i
  short l, New;
  float r_0, q, CriticalDelay, MaxCriticalDelay;
  int Nodes_in_tree;            //the number of nodes included in tree
  int MinResource, lay, Iter, num;
  int *PMinResource;
  TNET *CurTree, *NextTree, *WorstTree;
  int MinX, MaxX, MinY, MaxY, qq;

  Line_X = (short *) calloc (Size_Y + 1, sizeof (short));
  Line_Y = (short *) calloc (Size_X + 1, sizeof (short));

#ifndef NDEBUG
  fprintf (stdout, "Initial net information from Gen_Trees\n");
  PrintNet (CurNet, stdout);
#endif

  N = CurNet->Number;           //the number of sinks in CurNet
  r_0 = CurNet->drive;          //root resistance, assuming single driver

  // Subgraph construction
  for (i = 0; i <= Size_Y; i++)
    Line_X[i] = 0;
  for (i = 0; i <= Size_X; i++)
    Line_Y[i] = 0;

  // routing in Hanan graph H, build the hanan graph,
  if (CurNet->critical == 1) {
    // stay only on sink lines, no detours, and the no of
    // hanan points are less
    for (k = 1; k <= N; k++) {
      Line_X[CurNet->Sink[k].y] = 1;
      Line_Y[CurNet->Sink[k].x] = 1;
    } 
  } else {
    // Find out min and max in X and Y. All the cross 
    // points are declared as hanan points to have more options
    MinX = Size_X;
    MinY = Size_Y;
    MaxX = 0;
    MaxY = 0;

    for (k = 1; k <= N; k++) {
      if (CurNet->Sink[k].x < MinX)
        MinX = CurNet->Sink[k].x;
      if (CurNet->Sink[k].x > MaxX)
        MaxX = CurNet->Sink[k].x;
      if (CurNet->Sink[k].y < MinY)
        MinY = CurNet->Sink[k].y;
      if (CurNet->Sink[k].y > MaxY)
        MaxY = CurNet->Sink[k].y;
    }
/******** I want to use only Hanan Grid 
    // routing inside rectangle R (bounding box)
    for (i = MinX; i <= MaxX; i++)
      Line_Y[i] = 1;
    for (i = MinY; i <= MaxY; i++)
      Line_X[i] = 1;
***********/
   // routing inside Hanan graph H
for(k=1;k<=N;k++) {Line_X[CurNet->Sink[k].y]=1; Line_Y[CurNet->Sink[k].x]=1;}  

  }

  // Calculate the number of hor. & ver. lines in subgraph
  HorLines = 0;
  VerLines = 0;                 //number of hor & ver lines
  for (i = 0; i <= Size_X; i++)
    if (Line_Y[i] == 1)
      VerLines++;
  for (j = 0; j <= Size_Y; j++)
    if (Line_X[j] == 1)
      HorLines++;
  N_All = VerLines * HorLines * Size_Z; // Number of nodes in subgraph

  //---------------------------------------------------------------------------
  //begin node initialization in subgraph
  Node_All = (TNODE *) calloc (N_All + 1, sizeof (TNODE));

  //The numbering - from left to right line by line, layer by layer
  GlobNo = 0;                   //node's # in global graph
  no = 0;                       //nodes' # in subgraph
  for (l = 1; l <= Size_Z; l++) {
    for (j = 0; j <= Size_Y; j++) {
      for (i = 0; i <= Size_X; i++) {
        GlobNo++;               //node # in global grapg
        if (Line_Y[i] == 1 && Line_X[j] == 1) {
          no++;
          // set the coordinates, id
          Node_All[no].x = i;
          Node_All[no].y = j;
          Node_All[no].z = l;
          Node_All[no].No = GlobNo;     //node's # in global graph

          // set the neighbours of the current node, including boundaries
          if (Layer[l].dir == hor && ((no - 1) % VerLines > 0))
            Node_All[no].w = no - 1;    //west neighbor node
          else
            Node_All[no].w = -1;        //there is no west neighbor node

          if (Layer[l].dir == ver &&
              no <= VerLines * (HorLines - 1) + VerLines * HorLines * (l - 1))
            Node_All[no].n = no + VerLines;     //north neighbor node
          else
            Node_All[no].n = -1;        //there is no north neighbor node

          if (Layer[l].dir == hor && no % VerLines > 0)
            Node_All[no].e = no + 1;    //east neighbor node
          else
            Node_All[no].e = -1;        //there is no east neighbor node

          if (Layer[l].dir == ver &&
              no > VerLines + VerLines * HorLines * (l - 1))
            Node_All[no].s = no - VerLines;     //south neighbor node
          else
            Node_All[no].s = -1;        //there is no south neighbor node

          if (l < Size_Z)
            Node_All[no].u = no + HorLines * VerLines;  //up neighbor node
          else
            Node_All[no].u = -1;        //no up neghbor

          if (l > 1)
            Node_All[no].d = no - HorLines * VerLines;  //down neighbor node
          else
            Node_All[no].d = -1;        //no down neghbor
        }
      }                         //end node initialization in subgraph
    }
  }

  //---------------------------------------------------------------------------
  //node type definition and properties
  for (no = 1; no <= N_All; no++) {
    Node_All[no].type = -1;     //intermediate...
    Node_All[no].cap = 0.0;     //node
    Node_All[no].SubtreeCap = 0.0;
    Node_All[no].EdgeDelay = 0.0;
    Node_All[no].critical = 0;
    Node_All[no].PinId = -1;

    // initilize the sink nodes properties
    for (k = 1; k <= N; k++) {
      if (Node_All[no].x == CurNet->Sink[k].x && 
          Node_All[no].y == CurNet->Sink[k].y && 
          Node_All[no].z == CurNet->Sink[k].z)
      {    
        // node no contains at least one sink or root
        if (CurNet->Sink[k].type == 0)
          Node_All[no].type = 0;        //root node
        else {
          if (Node_All[no].type < 0) {
            Node_All[no].type = 1;      //sink node
            Node_All[no].PinId = CurNet->Sink[k].PinId;
            if (CurNet->Sink[k].critical == 1)
              Node_All[no].critical = 1;
          }
          Node_All[no].cap = Node_All[no].cap + CurNet->Sink[k].cap;
        }    // else
      }      // for if
    }
  }          // end node type definition

  //---------------------------------------------------------------------------
  free (Line_X);
  free (Line_Y);
  Tree_pred = (int *) calloc (N_All + 1, sizeof (int));
  MinResource = 0;
  for (lay = 1; lay <= Size_Z; lay++) {
    if (Layer[lay].Tracks > MinResource) {
      MinResource = Layer[lay].Tracks;
    }
  }

#ifndef NDEBUG
  printf ("Find trees for net %d\n", s);
#endif
  Number_of_Trees = 0;
  for (qq = 0; qq <= Max_qq; qq++) {
    q = 1.0 / (1.0 + 1.4 * qq);
    Iter = 0;
    /* Now generate steiner trees for the given net. If it is first iteration
     * then ist uses basic MAD algorithm. For subsequent iteration, it uses 
     * MAD_q alogorithm, and also uses criticality of the net 
     */
    do {
      Iter++;
      // PrintDebug ("Starting new iteration");
      if (Number_of_Trees == 0) {
        PMinResource = &MinResource;
        MAD (s, CurNet, r_0, N, N_All, Node_All, PMinResource);
        MinResource = *PMinResource;
      } else if (CurNet->critical != 1) {
        IMAD_q (s, CurNet, r_0, N, N_All, Node_All, MinResource - Iter + 1, q);
      } else {
        IMAD_q (s, CurNet, r_0, N, N_All, Node_All, -20, q);
      }

      // Tree presentation for net s as Net_Tree[s]
      Net_Tree[s].Id = CurNet->Id;
      Net_Tree[s].critical = CurNet->critical;
      Net_Tree[s].drive = r_0;
      Nodes_in_tree = 0;
      for (i = 1; i <= N_All; i++) {
        if (Tree_pred[i] > 0) {
          if (Node_All[i].type < 0)  // internal node
            Nodes_in_tree++;
          else
            for (k = 1; k <= N; k++)
              if (Node_All[i].x == CurNet->Sink[k].x &&
                  Node_All[i].y == CurNet->Sink[k].y &&
                  Node_All[i].z == CurNet->Sink[k].z)
                Nodes_in_tree++;
        }
      }

      Net_Tree[s].Number = Nodes_in_tree;
      //total number of nodes in tree...
      //...(but not all of these nodes are in the different global nodes...)
      Net_Tree[s].Sink = (TNODE *) calloc (Nodes_in_tree + 1, sizeof (TNODE));
      no = 0;
      for (i = 1; i <= N_All; i++) {
        if (Tree_pred[i] > 0)   // node i in tree
        {
          if (Node_All[i].type < 0)     // i is intermediate node
          {
            no++;
            Net_Tree[s].Sink[no].x = Node_All[i].x;
            Net_Tree[s].Sink[no].y = Node_All[i].y;
            Net_Tree[s].Sink[no].z = Node_All[i].z;
            Net_Tree[s].Sink[no].cap = 0.0;
            Net_Tree[s].Sink[no].type = -1;
            Net_Tree[s].Sink[no].critical = 0;
            Net_Tree[s].Sink[no].No = Node_All[i].No;
            Net_Tree[s].Sink[no].pred = Node_All[Tree_pred[i]].No;
            Net_Tree[s].Sink[no].pred_dir = Node_All[i].pred_dir;
            Net_Tree[s].Sink[no].delay = Node_All[i].delay;
            Net_Tree[s].Sink[no].SubtreeCap = Node_All[i].SubtreeCap;
            Net_Tree[s].Sink[no].EdgeDelay = Node_All[i].EdgeDelay;
          } else {
            // i is sink node
            for (k = 1; k <= N; k++) {
              if (Node_All[i].x == CurNet->Sink[k].x &&
                  Node_All[i].y == CurNet->Sink[k].y &&
                  Node_All[i].z == CurNet->Sink[k].z) {
                no++;
                Net_Tree[s].Sink[no].x = Node_All[i].x;
                Net_Tree[s].Sink[no].y = Node_All[i].y;
                Net_Tree[s].Sink[no].z = Node_All[i].z;
                Net_Tree[s].Sink[no].PinId = CurNet->Sink[k].PinId;
                Net_Tree[s].Sink[no].cap = CurNet->Sink[k].cap;
                Net_Tree[s].Sink[no].type = CurNet->Sink[k].type;
                Net_Tree[s].Sink[no].critical = CurNet->Sink[k].critical;
                Net_Tree[s].Sink[no].No = Node_All[i].No;
                Net_Tree[s].Sink[no].pred = Node_All[Tree_pred[i]].No;
                Net_Tree[s].Sink[no].pred_dir = Node_All[i].pred_dir;
                Net_Tree[s].Sink[no].delay = Node_All[i].delay;
                Net_Tree[s].Sink[no].SubtreeCap = Node_All[i].SubtreeCap;
                Net_Tree[s].Sink[no].EdgeDelay = Node_All[i].EdgeDelay;
                if (CurNet->Sink[k].type == 0)
                  Net_Tree[s].C_0 = Node_All[i].SubtreeCap;
              }
            }
          }
        }
      }

      // PrintDebug ("Updating critical delay for the generated tree\n");
      CriticalDelay = 0.0;
      for (k = 1; k <= Nodes_in_tree; k++) {
        if (Net_Tree[s].Sink[k].type == 1) {
          Net_Tree[s].Sink[k].D_k =
            Net_Tree[s].Sink[k].delay - r_0 * Net_Tree[s].C_0;
          if (CriticalDelay < Net_Tree[s].Sink[k].delay)
            CriticalDelay = Net_Tree[s].Sink[k].delay;
          if (Net_Tree[s].critical == 1 && Net_Tree[s].Sink[k].critical == 1)
            Net_Tree[s].Delay_in_critical_sink = Net_Tree[s].Sink[k].delay;
        } else
          Net_Tree[s].Sink[no].D_k = -1;
      }
      Net_Tree[s].CriticalDelay = CriticalDelay;

      //add NEW tree in TreeList
      if (Number_of_Trees == 0) //first tree added always
      {
        Number_of_Trees++;
        // PrintDebug ("Adding the first tree for the net\n");
        AddTree (s, &Net_Tree[s]);
      } else {
        //next tree added only if it is new
        CurTree = TreeList[s];
        New = 1;                //if New=1 then next tree is new
        while (CurTree != NULL) {
          if (CurTree->Number == Net_Tree[s].Number) {
            New = 0;
            for (i = 1; i <= CurTree->Number; i++)
              if (CurTree->Sink[i].pred != Net_Tree[s].Sink[i].pred) {
                New = 1;
                break;
              }
          }                     //if
          if (New == 0)
            break;              //the same tree is found
          NextTree = CurTree->next;
          CurTree = NextTree;
        }                       //while
        if (New == 1) {         
          // if Number_of_Trees < Max_Number_of_Trees => add new tree
          // if Number_of_Trees == Max_Number_of_Trees => compare the delays in new
          // and the worst tree. If new is better (with less delay), then the worst
          // tree is deleted, and new tree is added
          if (Number_of_Trees < Max_Number_of_Trees) {
            Number_of_Trees++;
            // PrintDebug ("Adding a new tree, within num tree limit\n");
            AddTree (s, &Net_Tree[s]);
          }                     //end if
          else                  //if Number_of_Trees==Max_Number_of_Trees
          {
            CurTree = TreeList[s];
            WorstTree = CurTree;
            MaxCriticalDelay = 0.0;
            while (CurTree != NULL) {
              if (CurNet->critical != 1)        //if net s is not critical
              {
                if (MaxCriticalDelay < CurTree->CriticalDelay) {
                  MaxCriticalDelay = CurTree->CriticalDelay;
                  WorstTree = CurTree;
                }
              } else            //if net s is critical
              {
                if (MaxCriticalDelay < CurTree->Delay_in_critical_sink) {
                  MaxCriticalDelay = CurTree->Delay_in_critical_sink;
                  WorstTree = CurTree;
                }
              }
              NextTree = CurTree->next;
              CurTree = NextTree;
            }                   //while
            //compare MaxCriticalDelay with critical delay in new tree
            if (CurNet->critical != 1) {
              if (MaxCriticalDelay > Net_Tree[s].CriticalDelay) {
                // PrintDebug ("Replacing old tree as it is providing less max critical delay\n");
                AddTree (s, &Net_Tree[s]);      //add Net_Tree[s] in TreeList[s]
                DeleteTree (s, WorstTree);      //delete WorstTree from TreeList[s]
              }
            } else {
              if (MaxCriticalDelay > Net_Tree[s].Delay_in_critical_sink) {
                // PrintDebug ("Replacing old tree as it is providing less delay in critical sink\n");
                AddTree (s, &Net_Tree[s]);      //add Net_Tree[s] in TreeList[s]
                DeleteTree (s, WorstTree);      //delete WorstTree from TreeList[s]
              }
            }
          }                     //end else
        }                       //end if(New==1)
      }                         //end add tree in TreeList[s]
    } while (Iter < MaxIter);
  }                             //for
  Trees_in_Q[s] = Number_of_Trees;
  free (Tree_pred);
  free (Node_All);
}

void
AddTree (int s, TNET * NewTree) //Add current tree in the list TreeList[s]
{
  TNET *Tree1, *CurTree;
  int i, num;

  CurTree = (TNET *) calloc (1, sizeof (TNET));
  CurTree->Id = NewTree->Id;
  CurTree->critical = NewTree->critical;
  CurTree->CriticalDelay = NewTree->CriticalDelay;
  CurTree->Delay_in_critical_sink = NewTree->Delay_in_critical_sink;
  CurTree->drive = NewTree->drive;
  CurTree->C_0 = NewTree->C_0;
  CurTree->Number = NewTree->Number;
  CurTree->next = NewTree->next;
  num = CurTree->Number;
  CurTree->Sink = (TNODE *) calloc (num + 1, sizeof (TNODE));
  for (i = 1; i <= num; i++) {
    CurTree->Sink[i].PinId = NewTree->Sink[i].PinId;
    CurTree->Sink[i].No = NewTree->Sink[i].No;
    CurTree->Sink[i].x = NewTree->Sink[i].x;
    CurTree->Sink[i].y = NewTree->Sink[i].y;
    CurTree->Sink[i].z = NewTree->Sink[i].z;
    CurTree->Sink[i].cap = NewTree->Sink[i].cap;
    CurTree->Sink[i].w = NewTree->Sink[i].w;
    CurTree->Sink[i].n = NewTree->Sink[i].n;
    CurTree->Sink[i].e = NewTree->Sink[i].e;
    CurTree->Sink[i].s = NewTree->Sink[i].s;
    CurTree->Sink[i].u = NewTree->Sink[i].u;
    CurTree->Sink[i].d = NewTree->Sink[i].d;
    CurTree->Sink[i].pred = NewTree->Sink[i].pred;
    CurTree->Sink[i].pred_dir = NewTree->Sink[i].pred_dir;
    CurTree->Sink[i].type = NewTree->Sink[i].type;
    CurTree->Sink[i].critical = NewTree->Sink[i].critical;
    CurTree->Sink[i].delay = NewTree->Sink[i].delay;
    CurTree->Sink[i].SubtreeCap = NewTree->Sink[i].SubtreeCap;
    CurTree->Sink[i].EdgeDelay = NewTree->Sink[i].EdgeDelay;
    CurTree->Sink[i].D_k = NewTree->Sink[i].D_k;
  }
  Tree1 = TreeList[s];
  if (Tree1 != NULL)            // list is not empty
  {                             //place CurTree to the first place
    CurTree->next = Tree1;
    TreeList[s] = CurTree;
  } else                        // first element in the list
  {                             //initialize sorted list
    CurTree->next = NULL;
    TreeList[s] = CurTree;
  }
}

void
DeleteTree (int s, TNET * WorstTree)    //delete WorstTree from TreeList[s]
{
  TNET *CurTree, *NextTree;

  CurTree = TreeList[s];
  while (CurTree != NULL) {
    if (CurTree->next == WorstTree)     //CurTree is left neighbour of WorstTree
    {
      CurTree->next = WorstTree->next;
      break;
    }
    NextTree = CurTree->next;
    CurTree = NextTree;
  }                             //while
  free (WorstTree->Sink);
  free (WorstTree);
}
