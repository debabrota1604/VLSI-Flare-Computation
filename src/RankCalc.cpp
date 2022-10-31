#include <string.h>
#include "input_handler.h"
//-------------------------------------------------------------
// Global variables
  extern TLAYER *Layer;      //Layer[l] - all data for layer l
  extern TSNET *InpNet;      //InpNet - network input info
  extern TGATE *Gate;        // Array with gate information
  extern TSTDGATE *StdGate;  // Aray with srandard gate information
  extern TEDGE *Edge;        // Aray with edge information
  extern int *GateRank;      // Gate rank array
  extern int *NetRank;       // Net rank array
  extern int *NetPrty;       // Net priority array
  extern int *SortedNet;     // Sorted net array (index only)
  extern int Number_of_Nets;
  extern int GateNumber;     // number of gates
  extern int PrInpNumber;    // number of primary inputs
  extern int PrOutNumber;    // number of primary outputs
  extern int VertexNumber;   // total number of gates and primary in/out
  extern int EdgeNumber;     // number of edges
  extern float AT_rise, AT_fall;   // arrival time for primary inputs
  extern float RT_rise, RT_fall;   // required time for primary outputs
  extern float RPI_rise, RPI_fall; // resistance of primary inputs
  extern float POCap;              // capacitance of primary outputs

//-------------------------------------------------------------
//
//-------------------------------------------------------------
void RankCount()
{
  int CurEdge, CurInp, CurOut;
  int i,j,r,r1,changed;
  TSNET CurNet;

  //-------------------------------------------------------------

  // Create arrays
  Edge=(TEDGE *) calloc(EdgeNumber+1,sizeof(TEDGE));
  GateRank=(int *) calloc(VertexNumber+1,sizeof(int));
  NetRank=(int *) calloc(Number_of_Nets+1,sizeof(int));

  printf("\n%s\n","Calculate rank and sort nets");

  // Fill Edge array
  CurEdge=0;
  for (i=1; i<=Number_of_Nets; i++)
  {
    CurNet=InpNet[i];
    CurInp=CurNet.Sink[0].GateID;
    for (j=1; j<CurNet.SinkNumber; j++)
    {
      CurEdge++;
      Edge[CurEdge].inp=CurInp;
      Edge[CurEdge].out=CurNet.Sink[j].GateID;
    }
  }

  // Calculate gate rank
  changed=0;
  do
  {
    changed=0;
    for (CurEdge=1; CurEdge<=EdgeNumber; CurEdge++)
    {
      r=GateRank[Edge[CurEdge].inp]+1;
      if (r>GateRank[Edge[CurEdge].out])
      {
        changed=1;
        GateRank[Edge[CurEdge].out]=r;
      }
    }
  } while(changed>0);

  // Calculate net rank
  for (i=1; i<=Number_of_Nets; i++)
  {
    CurNet=InpNet[i];
/*    r=0;
    for (j=1; j<CurNet.SinkNumber; j++)
    {
      r1=GateRank[CurNet.Sink[j].GateID];
      if (r1>r)
        r=r1;
    }
    NetRank[i]=r;
*/
    NetRank[i]=GateRank[CurNet.Sink[0].GateID];
  }
}//void RankCount()

void NetOrderRank()
//Sorting net - rank increasing
{
  int j,i;
  int indi,indj,temp;
  int curvalue;

  SortedNet=(int *) calloc (Number_of_Nets+1, sizeof(int));

  for (i=1; i<=Number_of_Nets; i++)
  {
    SortedNet[i]=i;
  }

  for (j=2; j<=Number_of_Nets; j++)
  {
    indj=SortedNet[j];
    curvalue=NetRank[indj];
    if(curvalue>NetRank[SortedNet[j-1]])
      continue;
    i=j-1;
    while ((i>=1)&&(NetRank[SortedNet[i]]>curvalue))
    {
      SortedNet[i+1]=SortedNet[i];
      i--;
    }
    SortedNet[i+1]=indj;
  }
  i-1;
}


void NetOrderPrty()
//Sorting net - priority increasing
{
  int j,i;
  int indi,indj,temp;
  int curvalue,currank;

  for (i=1; i<=Number_of_Nets; i++)
  {
    SortedNet[i]=i;
  }

  for (j=2; j<=Number_of_Nets; j++)
  {
    indj=SortedNet[j];
    curvalue=NetPrty[indj];
    currank=NetRank[indj];
    if(curvalue>NetPrty[SortedNet[j-1]])
      continue;
    i=j-1;
    while ((i>=1)&&
           ((NetPrty[SortedNet[i]]>curvalue)||
            ((NetPrty[SortedNet[i]]==curvalue)&&(NetRank[SortedNet[i]]>currank))
           )
          )
    {
      SortedNet[i+1]=SortedNet[i];
      i--;
    }
    SortedNet[i+1]=indj;
  }
  i-1;
}

