#include "routing.h"

// Global variables

extern TLAYER *Layer;           //Array with full layer information
extern TNET *Net_Tree;          //Array with net tree information
extern int Number_of_Nets;      //Number of nets
extern int Size_X;              // grid of...
extern int Size_Y;              // (Size_X+1) x (Size_Y+1) x Size_Z...
extern short Size_Z;            // dimension
extern int Number_of_glob_nodes;        //number of nodes in global graph
extern TGLOBALNODE *GlobalNode; //array with global nodes information
extern TNET **TreeList;
extern int *Trees_in_Q;

#define MAX_CAPACITY 100

void
WriteSol ()                     //write solution into the files *_1.out and *_2.out
{                               // Local variables
  FILE *OutFile;
  int s, i, j, NoTree;
  int Max_Overflow;             //maximal overflow of global edge
  int Number_of_overflows;      //number of global edges with overflow
  int Wire_length;              //total (of all trees) wire length
  int Total_overflow;           //the sum of overflows of all global edges
  TNET *CurTree, *NextTree;

  int  x, y, z;                 // temporary variables

  int  overflowDist[MAX_CAPACITY*2];
  int  cumOverflow = 0;

  //-------------------------------------------------------------
  // open first output file
  OutFile = fopen (OutFName1, "w");

  // header information
  fprintf (OutFile, "  No of nodes in X = %d\n", Size_X+1);
  fprintf (OutFile, "  No of nodes in Y = %d\n", Size_Y+1);
  fprintf (OutFile, "  No of Layers = %d\n", Size_Z);
  //  fprintf(OutFile,"%s\n","*Net_Trees");
  fprintf (OutFile, "%s %d\n", "  Number_of_nets=", Number_of_Nets);
  fprintf (OutFile, "%s %d\n", "  Max_Number_of_Trees=", Max_Number_of_Trees);
  fprintf (OutFile, "%s %d\n", "  MaxIter=", MaxIter);
  fprintf (OutFile, "%s %d\n", "  Max_qq=", Max_qq);

  for (s = 1; s <= Number_of_Nets; s++) {
    fprintf (OutFile, "%s %d\n", "    *Net_Id=", Net_Tree[s].Id);
    fprintf (OutFile, "%s %f\n", "      drive(r_0)= ", Net_Tree[s].drive);
    fprintf (OutFile, "%s %f\n", "      C_0= ", Net_Tree[s].C_0);
    fprintf (OutFile, "%s %d\n", "      Number_of_trees ", Trees_in_Q[s]);
    fprintf (OutFile, "%s\n", "     *Tree= 0 ");
    fprintf (OutFile, "%s %i\n", "      Number_of_nodes= ",
             Net_Tree[s].Number);
    for (j = 1; j <= Net_Tree[s].Number; j++) {
      fprintf (OutFile, "%s %i\n", "        *Node# ", j);
      fprintf (OutFile, "%s %hd\n", "        *PinId ",
               Net_Tree[s].Sink[j].PinId);
      fprintf (OutFile, "%s %i\n", "          Global# ",
               Net_Tree[s].Sink[j].No);
      fprintf (OutFile, "%s %i\n", "          Global#_of_node-predecessor ",
               Net_Tree[s].Sink[j].pred);
      fprintf (OutFile, "%s %hd\n",
               "          Node_type(0-root,1-sink,2-intermediate) ",
               Net_Tree[s].Sink[j].type);
      fprintf (OutFile, "%s", "          (x,y,z):");
      fprintf (OutFile, "%s %i %s %i %s %i\n", " ",
               Net_Tree[s].Sink[j].x, " ",
               Net_Tree[s].Sink[j].y, " ", Net_Tree[s].Sink[j].z);
      fprintf (OutFile, "%s %f\n", "          Node_capacitance= ",
               Net_Tree[s].Sink[j].cap);
      fprintf (OutFile, "%s %hd pred_node %d\n",
               "          direction_to_the_predecessor(1-west,2-north,3-east,4-south,5-up,6-down) ",
               Net_Tree[s].Sink[j].pred_dir, Net_Tree[s].Sink[j].pred);
      fprintf (OutFile, "%s %f\n", "          Delay_to_the_node ",
               Net_Tree[s].Sink[j].delay);
      fprintf (OutFile, "%s %f\n",
               "          Edge_delay_with_end_in_the_node ",
               Net_Tree[s].Sink[j].EdgeDelay);
      fprintf (OutFile, "%s %f\n",
               "          Capacitance_of_subtree_with_root_in_the_node ",
               Net_Tree[s].Sink[j].SubtreeCap);
    }
    if (Trees_in_Q[s] > 1) {
      CurTree = TreeList[s];
      NoTree = 0;
      while (CurTree != NULL) {
        NoTree++;
        fprintf (OutFile, "%s %d\n", "     *Tree= ", NoTree);
        fprintf (OutFile, "%s %i\n", "      Number_of_nodes ",
                 CurTree->Number);
        for (j = 1; j <= CurTree->Number; j++) {
          fprintf (OutFile, "%s %i\n", "        *Node# ", j);
          fprintf (OutFile, "%s %hd\n", "        *PinId ",
                   CurTree->Sink[j].PinId);
          fprintf (OutFile, "%s %i\n", "          Global# ",
                   CurTree->Sink[j].No);
          fprintf (OutFile, "%s %i\n",
                   "          Global#_of_node-predecessor ",
                   CurTree->Sink[j].pred);
          fprintf (OutFile, "%s %hd\n",
                   "          Node_type(0-root,1-sink,2-intermediate) ",
                   CurTree->Sink[j].type);
          fprintf (OutFile, "%s", "          (x,y,z):");
          fprintf (OutFile, "%s %i %s %i %s %i\n", " ",
                   CurTree->Sink[j].x, " ",
                   CurTree->Sink[j].y, " ", CurTree->Sink[j].z);
          fprintf (OutFile, "%s %f\n", "          Node_capacitance ",
                   CurTree->Sink[j].cap);
          fprintf (OutFile, "%s %hd pred_node %d\n",
                   "          direction_to_the_predecessor(1-west,2-north,3-east,4-south,5-up,6-down) ",
                   CurTree->Sink[j].pred_dir, Net_Tree[s].Sink[j].pred);
          fprintf (OutFile, "%s %f\n", "          Delay_to_the_node ",
                   CurTree->Sink[j].delay);
          fprintf (OutFile, "%s %f\n",
                   "          Edge_delay_with_end_in_the_node ",
                   CurTree->Sink[j].EdgeDelay);
          fprintf (OutFile, "%s %f\n",
                   "          Capacitance_of_subtree_with_root_in_the_node ",
                   CurTree->Sink[j].SubtreeCap);
        }
        NextTree = CurTree->next;
        CurTree = NextTree;
      }                         //while
    }                           //if
  }

//==============================================================================
//   fprintf(OutFile,"%s\n","***********************************************************");
  Max_Overflow = 0;
  Number_of_overflows = 0;
  Wire_length = 0;
  Total_overflow = 0;

  for (i=0; i < 2*MAX_CAPACITY; i++) {
    overflowDist[i] = 0;
  }
  
  for (i = 1; i <= Number_of_glob_nodes; i++) {
  /* 
    fprintf (OutFile, "Global_node %i (%3i,%3i,%3hd) has current capacity:\n", 
             GlobalNode[i].Id, GlobalNode[i].x, GlobalNode[i].y,
             GlobalNode[i].z);
    fprintf (OutFile, "  East = %i,  North = %i,  Up = %i\n",
             GlobalNode[i].EastResource, GlobalNode[i].NorthResource, GlobalNode[i].UpResource);

    printf ("%d %d %d %hd %d %d %d\n", GlobalNode[i].Id, GlobalNode[i].x, GlobalNode[i].y, GlobalNode[i].z,
            GlobalNode[i].EastResource, GlobalNode[i].NorthResource, GlobalNode[i].UpResource);
  */

    if (GlobalNode[i].EastResource > -99999) {
      Wire_length = Wire_length + Layer[GlobalNode[i].z].Tracks -
        GlobalNode[i].EastResource;
      overflowDist[MAX_CAPACITY-GlobalNode[i].EastResource] += 1;

      if (GlobalNode[i].EastResource < 0) {
        Number_of_overflows++;
        Total_overflow = Total_overflow - GlobalNode[i].EastResource;
        if (Max_Overflow < -GlobalNode[i].EastResource)
          Max_Overflow = -GlobalNode[i].EastResource;
      }
    }
    if (GlobalNode[i].NorthResource > -99999) {
      Wire_length = Wire_length + Layer[GlobalNode[i].z].Tracks -
        GlobalNode[i].NorthResource;
      // overflowDist[MAX_CAPACITY-GlobalNode[i].NorthResource] += 1;

      if (GlobalNode[i].NorthResource < 0) {
        Number_of_overflows++;
        Total_overflow = Total_overflow - GlobalNode[i].NorthResource;
        if (Max_Overflow < -GlobalNode[i].NorthResource)
          Max_Overflow = -GlobalNode[i].NorthResource;
      }
    }
    if (GlobalNode[i].UpResource > -99999) {
      if (GlobalNode[i].UpResource < 0) {
        Number_of_overflows++;
        if (Max_Overflow < -GlobalNode[i].UpResource)
          Max_Overflow = -GlobalNode[i].UpResource;
      }
    }
  }
  fprintf (OutFile, "***********************************************************\n");
  fprintf (OutFile, "Max_Overflow = %d Number_of_overflows = %d Total_overflow = %d Wire_length = %d\n", 
           Max_Overflow,  Number_of_overflows, Total_overflow, Wire_length);

  printf ("***********************************************************\n");
  printf ("Max_Overflow = %d Number_of_overflows = %d Total_overflow = %d Wire_length = %d\n", 
           Max_Overflow,  Number_of_overflows, Total_overflow, Wire_length);
  fprintf (OutFile, "\n*********** resource usage / overflow distribution ***************\n");
  cumOverflow = 0;
  
  for (i=0; i< MAX_CAPACITY*2; i++) {
    if (overflowDist[i] > 0) {
      cumOverflow += overflowDist[i];
      fprintf(OutFile, "%3d %4d %4d\n", i-MAX_CAPACITY, overflowDist[i], cumOverflow);
    }
  }
  fprintf(OutFile, "*********** end of resource usage distribution *****************\n\n");


  fprintf (OutFile, "\n*********** Edge usage info per node ***************\n");
  // writing down the congestion based on the global routing
  for (z = 0 ; z < Size_Z; z++) {
    for (y = 0; y <= Size_Y; y++) {
      for (x= 0; x <= Size_X; x++) {
        i = z * (Size_X+1) * (Size_Y+1) + y * (Size_X+1) + x + 1;
        fprintf (OutFile, "%4d %4d %4d %4hd %4d %4d %4d\n", 
                 GlobalNode[i].Id, GlobalNode[i].x, GlobalNode[i].y, GlobalNode[i].z, 
                 GlobalNode[i].EastResource > -99999? GlobalNode[i].EastResource : -100, 
                 GlobalNode[i].NorthResource > -99999? GlobalNode[i].NorthResource : -100,
                 GlobalNode[i].UpResource > -99999? GlobalNode[i].UpResource : -100);
      }
      fprintf (OutFile, "\n");
    }
    fprintf (OutFile, "\n");
  }
  fprintf (OutFile, "\n*********** End of edge usage info ***************\n");

  // close output file
  fclose (OutFile);
  //----------------------------------------------------------------------------
  // open second output file
  OutFile = fopen (OutFName2, "w");

  fprintf (OutFile, "%s\n", "*Net_Trees");
  fprintf (OutFile, "%s %d\n", "  Number_of_Net_Trees ", Number_of_Nets);
  for (s = 1; s <= Number_of_Nets; s++) {
    fprintf (OutFile, "%s %d\n", "    *Net_Tree_Id ", Net_Tree[s].Id);
    fprintf (OutFile, "%s %f\n", "      C_0 ", Net_Tree[s].C_0);
    for (j = 1; j <= Net_Tree[s].Number; j++)
      if (Net_Tree[s].Sink[j].type == 1) {
        fprintf (OutFile, "%s %hd\n", "        *PinId ",
                 Net_Tree[s].Sink[j].PinId);
        fprintf (OutFile, "%s %f\n", "          D_k ",
                 Net_Tree[s].Sink[j].D_k);
      }
  }
  // close output file
  fclose (OutFile);
}
