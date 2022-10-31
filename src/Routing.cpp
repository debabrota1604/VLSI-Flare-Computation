#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "routing.h"

// Global variables
TLAYER *Layer;                  //Array with data for layers
TGLOBALNODE *GlobalNode;        //Array with data for nodes in global graph
TNET *Net;                      //Array with nets data
TNET *Net_Tree;                 //Array with infor for net's tree
TNET **TreeList;                //Array of pointers on the first tree in Q^s
TNODELIST **Adj_Node_List;

int *Trees_in_Q;                //array, Trees_in_Q[s]=cardinality of Q^s
int Size_X;                     //grid of...
int Size_Y;                     // ...(Size_X+1) x (Size_Y+1) x Size_Z...
short Size_Z;                   //...dimension
int Number_of_Nets;             //number of nets in logical network
int N_Glob_Arcs;                //number of global north-east-up arcs
int Number_of_glob_nodes;       //number of nodes in global graph
int SelectedNumber;
short *Selected;

int main (int argc, const char *argv[])
{
  int i, j, k, l, no, s, v1, v2;
  int Max_Overflow;             //maximal overflow of global edge
  int Number_of_overflows;      //number of global edges with overflow
  int Wire_length;              //total (of all trees) wire length
  int Total_overflow;           //the sum of overflows of all global edges
  TNET *CurTree, *NextTree;
  TNODELIST *CurNode, *NextNode;

  const char *inpFileName = NULL;

	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	double start_time,end_time;
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	start_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)
 

 i = 1;
  while (i < argc) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'i':
        i++;
        inpFileName = argv[i];
      default:
        i++;
      }
      i++;
    }
  }

  if (!inpFileName) {
    printf ("Please provide the input file name with -i option\n");
    printf ("Usage \n\t%s -i <inpFile>\n", argv[0]);
    exit (0);
  }

  InitDataStructure (inpFileName);      //read input data about layers and nets

  PrintDebug ("\n\n**********************************************************\n");
  PrintDebug ("Completed tool data structure intialization\n\n");


//------------------------------------------------------------------------------
//begin global nodes initialization
  Number_of_glob_nodes = (Size_X + 1) * (Size_Y + 1) * Size_Z;
  GlobalNode =
    (TGLOBALNODE *) calloc (Number_of_glob_nodes + 1, sizeof (TGLOBALNODE));
  no = 0;                       //node's #
  for (l = 1; l <= Size_Z; l++) {
    for (j = 0; j <= Size_Y; j++) {
      for (i = 0; i <= Size_X; i++) {
        no++;
        GlobalNode[no].Id = no;
        GlobalNode[no].x = i;
        GlobalNode[no].y = j;
        GlobalNode[no].z = l;
        if (Layer[l].dir == hor || Layer[l].dir == ver) //routable layer
        {
          if (Layer[l].dir == hor)      //horizontal direction
          {
            if (i < Size_X)
              GlobalNode[no].EastResource = Layer[l].Tracks;
            else
              GlobalNode[no].EastResource = -MaxInt;
            GlobalNode[no].NorthResource = -MaxInt;
          } else                //vertical direction
          {
            if (j < Size_Y)
              GlobalNode[no].NorthResource = Layer[l].Tracks;
            else
              GlobalNode[no].NorthResource = -MaxInt;
            GlobalNode[no].EastResource = -MaxInt;
          }
        } else                  //not routable layer
        {
          GlobalNode[no].EastResource = -MaxInt;
          GlobalNode[no].NorthResource = -MaxInt;
        }
        if (l < Size_Z)
          GlobalNode[no].UpResource = Layer[l].UpTracks;
        else
          GlobalNode[no].UpResource = -MaxInt;
      }                         //end of global nodes initialization
    }
  }

  //------------------------------------------------------------------------------
  //construct Net_Tree for each net
  Net_Tree = (TNET *) calloc (Number_of_Nets + 1, sizeof (TNET));
  //******************************************************************************
  //construct the sets Q^s=TreeList[s],
  TreeList = (TNET **) calloc (Number_of_Nets + 1, sizeof (TNET **));
  Trees_in_Q = (int *) calloc (Number_of_Nets + 1, sizeof (int));
  Selected = (short *) calloc (Number_of_Nets + 1, sizeof (short));
  for (s = 1; s <= Number_of_Nets; s++) {
    TreeList[s] = NULL;
    Selected[s] = 0;
  }
  SelectedNumber = 0;
  for (s = 1; s <= Number_of_Nets; s++) //s is the # of net
  {
    Gen_Trees (s, &Net[s]);     //Yields TreeList[s]
    if (Trees_in_Q[s] == 1) {
      Net_Tree[s] = *TreeList[s];
      Selected[s] = 1;
#ifndef NDEBUG
      printf ("  single tree for net %d, selected by default\n", s);
#endif
      SelectedNumber++;
      GlobalResource (s);
    } else {
#ifndef NDEBUG
      printf ("For net %hd, number of steiner trees %d\n", Net[s].Id, Trees_in_Q[s]);
#endif
      /* for (int i = 0; i < Trees_in_Q[s]; i++)
        PrintNet (TreeList[s]+i, stdout);
      */
    }
  }
  printf ("%d trees are selected initially\n", SelectedNumber);

  //start verbose
  //---------------------------------------------------------------------------
  Max_Overflow = 0;
  Number_of_overflows = 0;
  Wire_length = 0;
  Total_overflow = 0;
  for (i = 1; i <= Number_of_glob_nodes; i++) {
    if (GlobalNode[i].EastResource < 0 && GlobalNode[i].EastResource > -99999) {
      Number_of_overflows++;
      Total_overflow = Total_overflow - GlobalNode[i].EastResource;
      if (Max_Overflow < -GlobalNode[i].EastResource)
        Max_Overflow = -GlobalNode[i].EastResource;
    }
    if (GlobalNode[i].NorthResource < 0
        && GlobalNode[i].NorthResource > -99999) {
      Number_of_overflows++;
      Total_overflow = Total_overflow - GlobalNode[i].NorthResource;
      if (Max_Overflow < -GlobalNode[i].NorthResource)
        Max_Overflow = -GlobalNode[i].NorthResource;
    }

    if (GlobalNode[i].UpResource < 0 && GlobalNode[i].UpResource > -99999) {
      Number_of_overflows++;
      if (Max_Overflow < -GlobalNode[i].UpResource)
        Max_Overflow = -GlobalNode[i].UpResource;
    }
    //Wire_length
    if (GlobalNode[i].EastResource > -99999)
      Wire_length = Wire_length + Layer[GlobalNode[i].z].Tracks -
        GlobalNode[i].EastResource;
    if (GlobalNode[i].NorthResource > -99999)
      Wire_length = Wire_length + Layer[GlobalNode[i].z].Tracks - GlobalNode[i].NorthResource;  //verbose
  }
  printf ("\n*********** Initial Result quality ***************\n");
  printf ("Max_Overflow = %d\n", Max_Overflow);
  printf ("Number_of_overflows = %d\n", Number_of_overflows);
  printf ("Total_overflow = %d\n", Total_overflow);
  printf ("Wire_length = %d\n", Wire_length);
  //---------------------------------------------------------------------------


  for (s = 1; s <= Number_of_Nets; s++) { //s is the # of net
    if (Selected[s] != 1) {
      free (Net_Tree[s].Sink);
      TreeList[s] = NULL;
      Gen_Trees (s, &Net[s]);
    }
  }
  //select only one tree Net_Tree[s] for each set Q^s
  if (Max_Number_of_Trees > 1)
    Gradient (0.1);             //run this procedure if Max_Number_of_Trees > 1

  WriteSol ();                  //writing solution in files *_1.out and *_2.out

  // free memory
  // free tree information
  for (s = 1; s <= Number_of_Nets; s++) {
    CurTree = TreeList[s];
    while (CurTree != NULL) {
      NextTree = CurTree->next;
      free (CurTree->Sink);
      free (CurTree);
      CurTree = NextTree;
    }
  }
  free (TreeList);

  for (s = 1; s <= Number_of_Nets; s++) {
    free (Net[s].Sink);
  }
  free (Net_Tree);
  free (Net);
  free (GlobalNode);
  free (Layer);
  free (Trees_in_Q);
  free (Selected);
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	end_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)

       printf("Program run time in second:%f\n",end_time-start_time);
  return 0;
}
