#include "routing.h"

//Global variables
extern TGLOBALNODE *GlobalNode; //global nodes array
extern TNET *Net_Tree;          //array with Steiner tree information
extern int Number_of_Nets;
extern int Size_X, Size_Y;      //plain dimension
extern short Size_Z;            //number of layers

//------------------------------------------------------------------------------

void
GlobalResource (int s)          //recalculate the resources of global edges (nodes)
{
  int i, j, k, v1, v2;
  short New, Pred_dir;

  for (j = 1; j <= Net_Tree[s].Number; j++)     //j is the # of node in tree
    if (Net_Tree[s].Sink[j].pred > 0
        && Net_Tree[s].Sink[j].pred != Net_Tree[s].Sink[j].No) {
      New = 1;
      for (i = 1; i < j; i++)
        if (Net_Tree[s].Sink[j].x == Net_Tree[s].Sink[i].x &&
            Net_Tree[s].Sink[j].y == Net_Tree[s].Sink[i].y &&
            Net_Tree[s].Sink[j].z == Net_Tree[s].Sink[i].z)
          New = 0;
      if (New == 1) {
        v1 = Net_Tree[s].Sink[j].pred;
        v2 = Net_Tree[s].Sink[j].No;
        Pred_dir = Net_Tree[s].Sink[j].pred_dir;
        if (v1 != v2)           // && Pred_dir<5)
          switch (Pred_dir) {
          case west:
            for (k = v1; k < v2; k++)
              GlobalNode[k].EastResource--;
            break;
          case north:
            k = v2;
            do {
              GlobalNode[k].NorthResource--;
              k = k + Size_X + 1;
            } while (k != v1);
            break;
          case east:
            for (k = v2; k < v1; k++)
              GlobalNode[k].EastResource--;
            break;
          case south:
            k = v1;
            do {
              GlobalNode[k].NorthResource--;
              k = k + Size_X + 1;
            } while (k != v2);
            break;
          case up:
            k = v2;
            do {
              GlobalNode[k].UpResource--;
              k = k + (Size_X + 1) * (Size_Y + 1);
            } while (k != v1);
            break;
          case down:
            k = v1;
            do {
              GlobalNode[k].UpResource--;
              k = k + (Size_X + 1) * (Size_Y + 1);
            } while (k != v2);
            break;
          }                     //end switch
      }                         //end if
    }                           //end for j
}
