#include "input_handler.h"

//-------------------------------------------------------------
// Global variables
extern TLAYER *Layer;		//Layer[l] - all data for layer l
extern TSNET *InpNet;		//InpNet - network input info
extern TGATE *Gate;		// Array with gate information
extern TSTDGATE *StdGate;	// Aray with srandard gate information
extern int *SortedNet;		// Sorted net array (index only)
extern int Size_X;
extern int Size_Y;
extern short Size_Z;
extern int Number_of_Nets;
extern int StdGateNumber;	// number of standard gates
extern int GateNumber;		// number of gates
extern int PrInpNumber;		// number of primary inputs
extern int PrOutNumber;		// number of primary outputs
extern int VertexNumber;	// total number of gates and primary in/out


void
WriteDat()
{				// Local variables
  FILE *OutFile;
  TGATE CurGate;
  TSNODE CurNode;
  int i, j, k;
  short crit;

  //-------------------------------------------------------------
  // open output file
  OutFile = fopen(InpFName, "w");

  printf("\n%s %s\n", "Write intermediate file", InpFName);

  fprintf(OutFile, "%s\n", "*Routing_area");
  fprintf(OutFile, "%s %d\n", "  Cols", Size_X);
  fprintf(OutFile, "%s %d\n", "  Rows", Size_Y);
  fprintf(OutFile, "%s %d\n", "  Number_of_layers(Size_Z)", Size_Z);

  for (i = 1; i <= Size_Z; i++) {
    fprintf(OutFile, "%s\n", "*Layer");
    fprintf(OutFile, "%s %d\n", "  Layer_Id", i);
    fprintf(OutFile, "%s %d\n", "  dir", Layer[i].dir);
    fprintf(OutFile, "%s %d\n", "  Tracks", Layer[i].Tracks);
    fprintf(OutFile, "%s %d\n", "  UpTracks", Layer[i].UpTracks);
    fprintf(OutFile, "%s %10.6f\n", "  cap", Layer[i].cap);
    fprintf(OutFile, "%s %10.6f\n", "  res", Layer[i].res);
    fprintf(OutFile, "%s %10.6f\n", "  up_cap", Layer[i].up_cap);
    fprintf(OutFile, "%s %10.6f\n", "  up_res", Layer[i].up_res);

  }

  fprintf(OutFile, "%s\n", "*Nets");
  fprintf(OutFile, "%s %d\n", "  Number_of_Nets", Number_of_Nets);
  for (k = 1; k <= Number_of_Nets; k++) {
    i = SortedNet[k];
    fprintf(OutFile, "%s\n", "    *Net");
    fprintf(OutFile, "%s %d %1d\n", "      Id", InpNet[i].Id,
	    InpNet[i].Critical);
    fprintf(OutFile, "%s %10.6f\n", "      drive(r_0)", InpNet[i].drive);
    fprintf(OutFile, "%s %d\n", "      Number_of_sinks",
	    InpNet[i].SinkNumber);
    for (j = 0; j < InpNet[i].SinkNumber; j++) {
      fprintf(OutFile, "%s %d \n", "      *Sink(x,y,z)", j);
      CurNode = InpNet[i].Sink[j];
      CurGate = Gate[CurNode.GateID];
      fprintf(OutFile, "%s %d %d %d\n", "        ",
	      CurGate.x, CurGate.y, CurGate.z);
      fprintf(OutFile, "%s %10.6f\n", "         cap",
	      StdGate[CurGate.LibID].Pins[CurNode.PinID].cap);
      crit = CurNode.Critical;
      if (j == 0)
	fprintf(OutFile, "%s %1d\n", "         type 0", crit);
      else
	fprintf(OutFile, "%s %1d\n", "         type 1", crit);
    }
  }

  // close output file
  fclose(OutFile);
}
