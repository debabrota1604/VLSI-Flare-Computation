#include <string.h>
#include "input_handler.h"
//-------------------------------------------------------------
// Global variables
extern TLAYER *Layer;		//Layer[l] - all data for layer l
extern TSNET *InpNet;		//InpNet - network input info
extern TGATE *Gate;		// Array with gate information
extern TSTDGATE *StdGate;	// Aray with srandard gate information
extern int StdGateNumber;	// number of standard gates
extern float AT_rise, AT_fall;	// arrival time for primary inputs
extern float RT_rise, RT_fall;	// required time for primary outputs
extern float RPI_rise, RPI_fall;	// resistance of primary inputs
extern float POCap;		// capacitance of primary outputs

//-------------------------------------------------------------
//
//-------------------------------------------------------------
void
InputGenlib()
{
  FILE *InpFile;
  InpStr str, reststr;
  FieldStr str1, str2, str3;
  NameStr nstr;
  char *ptr, *ptr1, *ptr2, *ptrs, tmpc;
  short CurPinNumber, CurSink, stmp, curtype;
  int CurGateNumber;
  int i, j, numb, itmp, NumSinks, ix, iy, iz, iw, in, ie, is, iu, id, indp;
  float ftmp, rmax_rise, rmax_fall;
  char *ret;
  //-------------------------------------------------------------

  StdGateNumber = 1;		//0 - primary input, StdGateNumber - primary output

  printf("\n%s\n", "Read Genlib information");

  InpFile = fopen(GenlibFName, "r");
  printf("%s %s %s\n", "File", GenlibFName, "was opened");
  // count number of standard gates
  do {
    ret = fgets(str, MaxInpStrLength, InpFile);
    ptr = strtok(str, " ");
    strcpy(str1, ptr);
    if (!strcmp(str1, "GATE"))
      StdGateNumber++;
  } while (!feof(InpFile));
  fclose(InpFile);
  printf("%s %d\n", "Number of standard gates is", StdGateNumber - 1);

  //create data array for standard gates information
  StdGate = (TSTDGATE *) calloc(StdGateNumber + 1, sizeof(TSTDGATE));

  CurGateNumber = 0;
  CurPinNumber = 0;
  rmax_rise = 0.0;
  rmax_fall = 0.0;

  InpFile = fopen(GenlibFName, "r");
  do {
    ret = fgets(str, MaxInpStrLength, InpFile);
    ptr = strtok(str, " ");
    strcpy(str1, ptr);

    if (!strcmp(str1, "GATE")) {	// complete current gate definition
      StdGate[CurGateNumber].PinNum = CurPinNumber;
      if (CurPinNumber != 0) {	//calculate r_0
	StdGate[CurGateNumber].r_0 = (rmax_rise + rmax_fall) / 2;
      }
      // start new gate definition
      CurGateNumber++;
      CurPinNumber = 0;
      rmax_rise = 0.0;
      rmax_fall = 0.0;
      ptr = strtok(NULL, " ");
      strcpy(StdGate[CurGateNumber].Name, ptr);
      continue;
    }				// if(!strcmp(str1,"GATE"))

    if (!strcmp(str1, "PIN")) {	//read pin description
      CurPinNumber++;

      //extract pin name
      ptr = strtok(NULL, " ");
      strcpy(StdGate[CurGateNumber].Pins[CurPinNumber].Name, ptr);

      //extract pin type
      ptr = strtok(NULL, " ");
      strcpy(str2, ptr);
      if (!strncmp(str2, "INV", 3))
	curtype = INV;
      else if (!strncmp(str2, "NOINV", 5))
	curtype = NOINV;
      else
	curtype = UNKNOWN;
      StdGate[CurGateNumber].Pins[CurPinNumber].type = curtype;

      //extract pin capacitance
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &ftmp);
      StdGate[CurGateNumber].Pins[CurPinNumber].cap = ftmp;
      //skip maximal capacitance
      ptr = strtok(NULL, " ");

      //extract rise delay
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &ftmp);
      StdGate[CurGateNumber].Pins[CurPinNumber].delay_r = ftmp;
      //extract rise resistance
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &ftmp);
      StdGate[CurGateNumber].Pins[CurPinNumber].res_r = ftmp;
      if (ftmp > rmax_rise)
	rmax_rise = ftmp;
      //skip rise slope
      ptr = strtok(NULL, " ");

      //extract fall delay
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &ftmp);
      StdGate[CurGateNumber].Pins[CurPinNumber].delay_f = ftmp;
      //extract fall resistance
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &ftmp);
      StdGate[CurGateNumber].Pins[CurPinNumber].res_f = ftmp;
      if (ftmp > rmax_fall)
	rmax_fall = ftmp;
      //skip fall slope
      ptr = strtok(NULL, " ");
    }				//if(!strcmp(str1,"PIN"))

  } while (!feof(InpFile));
  // complete the last gate definition
  StdGate[CurGateNumber].PinNum = CurPinNumber;
  if (CurPinNumber != 0) {	//calculate r_0
    StdGate[CurGateNumber].r_0 = (rmax_rise + rmax_fall) / 2;
  }
  // close input file
  fclose(InpFile);

}				//void InputGenlib()


//-------------------------------------------------------------
//
//-------------------------------------------------------------
void
InputBlif()
{
  FILE *InpFile;
  InpStr str, reststr;
  FieldStr str1, str2, str3;
  NameStr nstr;
  char *ptr, *ptr1, *ptr2, *ptrs, tmpc,*ret;

  printf("\n%s\n", "Read primary ports information");
  InpFile = fopen(BlifFName, "r");
  printf("%s %s %s\n", "File", BlifFName, "was opened");
  do {
    ret = fgets(str, MaxInpStrLength, InpFile);
    ptr = strtok(str, " ");

    if (!strncmp(ptr, ".default_input_arrival", 22)) {
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &AT_rise);
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &AT_fall);
      continue;
    }

    if (!strncmp(ptr, ".default_output_required", 25)) {
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &RT_rise);
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &RT_fall);
      continue;
    }

    if (!strncmp(ptr, ".default_input_drive", 20)) {
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &RPI_rise);
      ptr = strtok(NULL, " ");
      sscanf(ptr, "%f", &RPI_fall);
      continue;
    }

    if (!strncmp(ptr, ".default_output_load", 20)) {
      ptr = strtok(NULL, " ");
      if (ptr != NULL)
	sscanf(ptr, "%f", &POCap);
      continue;
    }
  } while (!feof(InpFile));

  printf("%s %6.2f %6.2f\n",
	 "Default arrival time for all primary inputs (ps)", AT_rise,
	 AT_fall);
  printf("%s %6.2f %6.2f\n",
	 "Default required time for all primary outputs (ps)", RT_rise,
	 RT_fall);
  printf("%s %6.2f %6.2f\n",
	 "Default effective resistance for primary inputs (kOhms)",
	 RPI_rise, RPI_fall);
  printf("%s %6.2f\n",
	 "default input capacitance for all primary outputs (fF)", POCap);

  //define primary input type
  StdGate[0].r_0 = (RPI_rise + RPI_fall) / 2;
  StdGate[0].PinNum = 0;
  StdGate[0].Pins[0].cap = 0;
  StdGate[0].Pins[0].res_r = RPI_rise;
  StdGate[0].Pins[0].res_f = RPI_fall;

  //define primary output type
  StdGate[StdGateNumber].r_0 = 0.0;
  StdGate[StdGateNumber].PinNum = 0;
  StdGate[StdGateNumber].Pins[0].cap = POCap;

  fclose(InpFile);

}				//void InputBlif()
