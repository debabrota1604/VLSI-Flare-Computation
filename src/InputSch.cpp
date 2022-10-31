#include <string.h>
#include "input_handler.h"
//-------------------------------------------------------------
// Global variables
extern TLAYER *Layer;		//Layer[l] - all data for layer l
extern TSNET *InpNet;		//InpNet - network input info
extern TGATE *Gate;		// Array with gate information
extern TSTDGATE *StdGate;	// Aray with srandard gate information
extern int GateNumber;		// number of gates
extern int PrInpNumber;		// number of primary inputs
extern int PrOutNumber;		// number of primary outputs
extern int VertexNumber;	// total number of gates and primary in/out
extern int StdGateNumber;	// number of standard gates
extern float AT_rise, AT_fall;	// arrival time for primary inputs
extern float RT_rise, RT_fall;	// required time for primary outputs
extern float RPI_rise, RPI_fall;	// resistance of primary inputs
extern float POCap;		// capacitance of primary outputs

//-------------------------------------------------------------
//
//-------------------------------------------------------------
void
InputSch()
{
  FILE *InpFile;
  InpStr str, reststr;
  FieldStr str1, str2, str3;
  NameStr nstr;
  char *ptr, *ptr1, *ptr2, *ptrs, tmpc;
  short CurPinNumber, CurSink, stmp, curtype;
  int CurGateNumber;
  int CurGate, CurInp, CurOut;
  int i;
  int ret;
  char *ret1;

  //-------------------------------------------------------------

  GateNumber = 0;
  PrInpNumber = 0;
  PrOutNumber = 0;
  VertexNumber = 0;

  printf("\n%s\n", "Read gates data");
  InpFile = fopen(SchFName, "r");
  printf("%s %s %s\n", "File", SchFName, "was opened");

  // count number of gates, primary inputs and outputs
  do {
    ret1 = fgets(str, MaxInpStrLength, InpFile);
    ptr = strtok(str, " ");
    if (ptr[0] == 'S') {
      ptr = strtok(NULL, " ");
      ptr = strtok(NULL, " ");
      ptr = strtok(NULL, " ");
      strcpy(str1, ptr);
      if (!strncmp(str1, "(INPUT)", 7)) {	// count primary inputs
	do {
	  PrInpNumber++;
	  ret1 = fgets(str, MaxInpStrLength, InpFile);
	} while (strlen(str) > 1);
	PrInpNumber--;
	continue;
      }
      if (!strncmp(str1, "(OUTPUT)", 8)) {	// count primary outputs
	do {
	  PrOutNumber++;
	  ret1 = fgets(str, MaxInpStrLength, InpFile);
	} while (strlen(str) > 1);
	PrOutNumber--;
	continue;
      }
      if (!strncmp(str1, "(IOPUT)", 7)) {
	break;
      }
      // count gate
      GateNumber++;
    }
  } while (!feof(InpFile));
  fclose(InpFile);
  VertexNumber = PrInpNumber + PrOutNumber + GateNumber;

  printf("%s %d\n", "Number of primary inputs is    ", PrInpNumber);
  printf("%s %d\n", "Number of primary outputs is   ", PrOutNumber);
  printf("%s %d\n", "Number of intermediate gates is", GateNumber);
  printf("%s %d\n", "Total number of gates is       ", VertexNumber);

  //create data array for gate data
  Gate = (TGATE *) calloc(VertexNumber + 1, sizeof(TGATE));
  CurInp = 0;
  CurGate = PrInpNumber;
  CurOut = CurGate + GateNumber;

  InpFile = fopen(SchFName, "r");
  do {				// fill array Gate
    ret1 = fgets(str, MaxInpStrLength, InpFile);
    ptr = strtok(str, " ");
    if (ptr[0] == 'S') {
      ptr = strtok(NULL, " ");
      ptr = strtok(NULL, " ");
      ptr = strtok(NULL, " ");
      strcpy(str1, ptr);
      if (!strncmp(str1, "(INPUT)", 7)) {	// input primary inputs name
	for (i = 0; i < PrInpNumber; i++) {
	  CurInp++;
	  ret = fscanf(InpFile, "%s", Gate[CurInp].Name);
	  Gate[CurInp].LibID = 0;
	}
	continue;
      }
      if (!strncmp(str1, "(OUTPUT)", 8)) {	// input primary outputs name
	for (i = 0; i < PrOutNumber; i++) {
	  CurOut++;
	  ret = fscanf(InpFile, "%s", Gate[CurOut].Name);
	  Gate[CurOut].LibID = StdGateNumber;
	}
	continue;
      }
      if (!strncmp(str1, "(IOPUT)", 7)) {
	break;
      }
      // input gate type and name
      CurGate++;
      //extact standard gate name and find its ID
      ret = sscanf(str1, "%c%s", &tmpc, str2);
      stmp = strlen(str2) - 1;
      for (i = 1; i < StdGateNumber; i++) {
	if (!strncmp(str2, StdGate[i].Name, stmp)) {
	  Gate[CurGate].LibID = i;
	  break;
	}
      }
      //extract gate name
      ptr = strtok(NULL, " ");
      ret = sscanf(ptr, "%s", str2);
      str2[strlen(str2) - 1] = '\n';
      strcpy(Gate[CurGate].Name, str2);
    }
  } while (!feof(InpFile));



  // close input file
  fclose(InpFile);

}				//void InputSch()
