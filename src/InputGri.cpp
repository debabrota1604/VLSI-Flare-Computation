#include <string.h>
#include "input_handler.h"
//-------------------------------------------------------------
// Global variables
extern TLAYER *Layer;		//Layer[l] - all data for layer l
extern TSNET *InpNet;		//InpNet - network input info
extern TGATE *Gate;		// Array with gate information
extern TSTDGATE *StdGate;	// Aray with standard gate information
extern int Size_X;
extern int Size_Y;
extern short Size_Z;
extern int Number_of_Nets;
extern int StdGateNumber;	// number of standard gates
extern int GateNumber;		// number of gates
extern int PrInpNumber;		// number of primary inputs
extern int PrOutNumber;		// number of primary outputs
extern int VertexNumber;	// total number of gates and primary in/out
extern int EdgeNumber;		// number of edges

void
InputGri()
{
  FILE *InpFile;
  InpStr str, reststr;
  FieldStr str1, str2, str3;
  NameStr nstr;
  char *ptr, *ptr1, *ptr2, *ptrs, ceq = '=', cdd = ':', tmpc;
  short CurSink, stmp, l;
  short CurLayerNumber;
  int CurNetNumber, HorTracks, VerTracks;
  int i, j, numb, itmp, NumSinks, ix, iy, iz, iw, in, ie, is, iu, id, indp;
  float ftmp;
  int CurGateNum;
  TGATE CurGate;
  TSTDGATE CurStdGate;
  char *ret;
  int ret1;

  //-------------------------------------------------------------

  Size_Z = 0;
  Number_of_Nets = 0;

  printf("\n%s\n", "Read layers and nets data");
  InpFile = fopen(GriFName, "r");
  printf("%s %s %s\n", "File", GriFName, "was opened");

  // count layers and nets number
  do {
    ret = fgets(str, MaxInpStrLength, InpFile);
    ptr = strtok(str, " ");
    strcpy(str1, ptr);
    if (!strcmp(str1, "Metal")) {
      Size_Z++;
      continue;
    }
    if (!strcmp(str1, "Net")) {
      Number_of_Nets++;
      continue;
    }
  } while (!feof(InpFile));
  fclose(InpFile);

  //create data array for layer information
  Layer = (TLAYER *) calloc(Size_Z + 1, sizeof(TLAYER));
  //create data array for net information
  InpNet = (TSNET *) calloc(Number_of_Nets + 1, sizeof(TSNET));

  CurNetNumber = 0;
  CurLayerNumber = 0;
  EdgeNumber = 0;

  InpFile = fopen(GriFName, "r");
  do {
    ret = fgets(str, MaxInpStrLength, InpFile);
    ptr = strtok(str, " ");
    strcpy(str1, ptr);

    if (!strcmp(str1, "BinGrid")) {	//read grid size
      for (i = 0; i < 2; i++) {
	ptr = strtok(NULL, " ");
	strcpy(str2, ptr);
	strncpy(nstr, str2, 4);
	ptrs = strchr(str2, ceq);
	sscanf(ptrs, "%c%d", &tmpc, &numb);
	if (!strncmp(nstr, "cols", 4))
	  Size_X = numb;
	else
	  Size_Y = numb;
      }
      continue;
    }				//if(!strcmp(str1,"BinGrid"))

    if (!strcmp(str1, "Metal")) {	//read layer description
      CurLayerNumber++;
      do {
	ptr = strtok(NULL, " ");
	if (ptr == NULL)
	  break;
	strcpy(str2, ptr);
	if (!strncmp(str2, "id", 2)) {
	  ptrs = strchr(str2, ceq);
	  sscanf(ptrs, "%c%hd", &tmpc, &l);
	  Layer[CurLayerNumber].Id = l;
	  continue;
	}
	if (!strncmp(str2, "direction", 9)) {
	  ptrs = strchr(str2, ceq);
	  sscanf(ptrs, "%c%s", &tmpc, str3);
	  if (!strncmp(str3, "ver", 3))
	    Layer[CurLayerNumber].dir = 2;
	  else
	    Layer[CurLayerNumber].dir = 1;
	  continue;
	}
	if (!strncmp(str2, "routable", 8)) {
	  ptrs = strchr(str2, ceq);
	  sscanf(ptrs, "%c%s", &tmpc, str3);
	  if (!strncmp(str3, "no", 2))
	    Layer[CurLayerNumber].dir = 0;
	  continue;
	}
	if (!strncmp(str2, "tracks_per_bin", 14)) {
	  ptrs = strchr(str2, ceq);
	  sscanf(ptrs, "%c%d", &tmpc, &numb);
	  Layer[CurLayerNumber].Tracks = numb;
	  continue;
	}
	if (!strncmp(str2, "resistance_per_bin", 18)) {
	  ptrs = strchr(str2, ceq);
	  sscanf(ptrs, "%c%f", &tmpc, &ftmp);
	  //normalize to kOhms
	  Layer[CurLayerNumber].res = ftmp * 0.001;	//normalize to kOhms
	  continue;
	}
	if (!strncmp(str2, "capacitance_per_bin", 19)) {
	  ptrs = strchr(str2, ceq);
	  sscanf(ptrs, "%c%f", &tmpc, &ftmp);
	  Layer[CurLayerNumber].cap = ftmp;
	  continue;
	}
	if (!strncmp(str2, "via_up_resistance", 17)) {
	  ptrs = strchr(str2, ceq);
	  sscanf(ptrs, "%c%f", &tmpc, &ftmp);
	  Layer[CurLayerNumber].up_res = ftmp * 0.001;	//normalize to kOhms
	  continue;
	}
      } while (ptr != NULL);
      if (Layer[CurLayerNumber].dir == 1)
	HorTracks = Layer[CurLayerNumber].Tracks;
      else if (Layer[CurLayerNumber].dir == 2)
	VerTracks = Layer[CurLayerNumber].Tracks;
      continue;
    }

    if (!strcmp(str1, "Net")) {	//start new net description
      InpNet[CurNetNumber].SinkNumber = CurSink;
      CurNetNumber++;
      ptr = strtok(NULL, " \n");
      InpNet[CurNetNumber].Id = CurNetNumber;
      strcpy(InpNet[CurNetNumber].Name, ptr);
      InpNet[CurNetNumber].Critical = 0;
      CurSink = 0;
      continue;
    }

    if (!strcmp(str1, "source")) {	//read source information
      ptr = strtok(NULL, " ");
      //check pin type
      ptr1 = strchr(ptr, cdd);
      if (ptr1 != NULL) {	//extract gate name
	l = strcspn(ptr, "::");
	strncpy(ptr2, ptr, l);
	//extract gate ID
	for (i = PrInpNumber + 1; i <= PrInpNumber + GateNumber; i++)
	  if (!strncmp(ptr2, Gate[i].Name, l)) {
	    CurGateNum = i;
	    CurGate = Gate[i];
	    InpNet[CurNetNumber].Sink[CurSink].GateID = i;
	    InpNet[CurNetNumber].Sink[CurSink].Critical = 0;
	    break;
	  }
      } else			//primary input
      {				//extract primary input ID
	for (i = 1; i <= PrInpNumber; i++)
	  if (!strcmp(ptr, Gate[i].Name)) {
	    CurGateNum = i;
	    CurGate = Gate[i];
	    InpNet[CurNetNumber].Sink[CurSink].GateID = i;
	    InpNet[CurNetNumber].Sink[CurSink].Critical = 0;
	    break;
	  }
      }
      InpNet[CurNetNumber].drive = StdGate[CurGate.LibID].r_0;
      InpNet[CurNetNumber].Sink[CurSink].PinID = 0;
      //define gate coordinates
      for (i = 0; i <= 2; i++) {
	ptr = strtok(NULL, " ");
	strcpy(str2, ptr);
	stmp = strcspn(str2, "=");
	strncpy(nstr, str2, stmp);
	ptrs = strchr(str2, ceq);
	sscanf(ptrs, "%c%hd", &tmpc, &l);
	if (!strncmp(nstr, "col", 3))
	  CurGate.x = l;
	else if (!strncmp(nstr, "row", 3))
	  CurGate.y = l;
	else
	  CurGate.z = l;
      }
      Gate[CurGateNum] = CurGate;
      CurSink++;
      continue;
    }

    if (!strcmp(str1, "sink")) {	//read sink information
      EdgeNumber++;		// count total number of sinks
      ptr = strtok(NULL, " ");
      //check pin type
      ptr1 = strchr(ptr, cdd);
      if (ptr1 != NULL) {	//extract gate name and pin name
	l = strcspn(ptr, "::");
	strncpy(ptr2, ptr, l);
	//extract gate ID
	for (i = PrInpNumber + 1; i <= PrInpNumber + GateNumber; i++)
	  if (!strncmp(ptr2, Gate[i].Name, l)) {
	    CurGateNum = i;
	    CurGate = Gate[i];
	    InpNet[CurNetNumber].Sink[CurSink].GateID = i;
	    InpNet[CurNetNumber].Sink[CurSink].Critical = 0;
	    break;
	  }
	//extract PinID
	sscanf(ptr1, "%c%c%s", &tmpc, &tmpc, ptr2);
	CurStdGate = StdGate[CurGate.LibID];
	for (i = 1; i <= CurStdGate.PinNum; i++)
	  if (!strcmp(ptr2, CurStdGate.Pins[i].Name)) {
	    InpNet[CurNetNumber].Sink[CurSink].PinID = i;
	    break;
	  }
      } else			//primary output
      {				//extract primary output ID
	for (i = PrInpNumber + GateNumber + 1; i <= VertexNumber; i++)
	  if (!strcmp(ptr, Gate[i].Name)) {
	    CurGateNum = i;
	    CurGate = Gate[i];
	    InpNet[CurNetNumber].Sink[CurSink].GateID = i;
	    InpNet[CurNetNumber].Sink[CurSink].Critical = 0;
	    break;
	  }
	InpNet[CurNetNumber].Sink[CurSink].PinID = 0;
      }
      //define gate coordinates
      for (i = 0; i <= 2; i++) {
	ptr = strtok(NULL, " ");
	strcpy(str2, ptr);
	stmp = strcspn(str2, "=");
	strncpy(nstr, str2, stmp);
	ptrs = strchr(str2, ceq);
	sscanf(ptrs, "%c%hd", &tmpc, &l);
	if (!strncmp(nstr, "col", 3))
	  CurGate.x = l;
	else if (!strncmp(nstr, "row", 3))
	  CurGate.y = l;
	else
	  CurGate.z = l;
      }
      Gate[CurGateNum] = CurGate;
      CurSink++;
      continue;
    }

  } while (!feof(InpFile));

  InpNet[CurNetNumber].SinkNumber = CurSink;

  for (i = 1; i < Size_Z; i++) {
    Layer[i].UpTracks = HorTracks * VerTracks;
    Layer[i].up_cap = 0.0;
  }
  Layer[Size_Z].UpTracks = 0;
  Layer[Size_Z].up_cap = 0.0;

  printf("%s %d %s %d\n", "Grid size: cols=", Size_X, "rows=", Size_Y);
  printf("%s %d\n", "Number of layers is", Size_Z);
  for (i = 1; i <= Size_Z; i++) {
    printf("%s %d ", "Layer", i);
    switch (Layer[i].dir) {
    case 0:
      printf("%s ", "dir=n/r");
      Layer[i].Tracks = 0;
      break;
    case 1:
      printf("%s ", "dir=hor");
      break;
    case 2:
      printf("%s ", "dir=ver");
    }
    printf("%s%d ", "tracks=", Layer[i].Tracks);
    printf("%s%8.4f ", "res=", Layer[i].res * 1000);
    printf("%s%8.6f ", "cap=", Layer[i].cap);
    printf("%s%8.4f\n", "via_up_res=", Layer[i].up_res * 1000);
  }
  printf("%s %d\n", "Number of nets is", Number_of_Nets);

  // close input file
  fclose(InpFile);
}
