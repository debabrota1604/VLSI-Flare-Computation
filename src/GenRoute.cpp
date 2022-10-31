#include <stdio.h>
#include "input_handler.h"

// Global variables
TLAYER *Layer;			// Array with data for layers
TSNET *InpNet;			// Array with input net data
TGATE *Gate;			// Array with gate information
TSTDGATE *StdGate;		// Aray with srandard gate information
TEDGE *Edge;			// Aray with edge information
int *GateRank;			// Gate rank array
int *NetRank;			// Net rank array
int *NetPrty;			// Net priority array
int *SortedNet;			// Sorted net array (index only)
int Size_X;			// grid of
int Size_Y;			// (Size_X+1) x (Size_Y+1) x Size_Z
short Size_Z;			// dimension
int Number_of_Nets;		// number of nets in logical network
int StdGateNumber;		// number of standard gates
int GateNumber;			// number of gates
int PrInpNumber;		// number of primary inputs
int PrOutNumber;		// number of primary outputs
int VertexNumber;		// total number of gates and primary in/out
int EdgeNumber;			// number of edges
float AT_rise, AT_fall;		// arrival time for primary inputs
float RT_rise, RT_fall;		// required time for primary outputs
float RPI_rise, RPI_fall;	// resistance of primary inputs
float POCap;			// capacitance of primary outputs

int
main()
{
  printf("%s\n", "Program GenRoute is started");
  InputGenlib();		// input standard gates data
  InputBlif();			// input arrival/arrival time and primary inputs data
  InputSch();			// input gates data
  InputGri();			// input layers and nets data
  RankCount();			// calculate net rank
  NetOrderRank();		// order nets by rank
// WriteRout();
  Timing();			// calculate AT and RT
// NetOrderPrty(); // order nets by priority
  WriteDat();			// write modified dat file
  printf("%s\n", "Program GenRoute is finished\n");
  return 0;
}
