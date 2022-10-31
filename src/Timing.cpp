#include <string.h>
#include "input_handler.h"
//-------------------------------------------------------------
// Global variables
extern TLAYER *Layer;		//Layer[l] - all data for layer l
extern TSNET *InpNet;		//InpNet - network input info
extern TGATE *Gate;		// Array with gate information
extern TSTDGATE *StdGate;	// Aray with srandard gate information
extern int *NetPrty;		// Net priority array
extern int *SortedNet;		// Sorted net array (index only)
extern int Number_of_Nets;
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
// calculate arrival and required time for all pins
//-------------------------------------------------------------
void
Timing()
{
  FILE *InpFile, *OutFile;
  InpStr str, reststr;
  InpStr str1, str2, str3;
  NameStr nstr;
  TSNET CurNet;
  TSNODE CurNode;
  TSTDPIN CurPin, SourcePin;
  TSTDGATE CurStdGate;
  TGATE CurGate;
  char *ptr, *ptr1, *ptr2, *ptrs, tmpc;
  short CurPinNumber, CurSink, CurStdGateN;
  float SourceATR, SourceATF, SinkATR, SinkATF, CurATR, CurATF;
  float SourceRTR, SourceRTF, SinkRTR, SinkRTF, CurRTR, CurRTF;
  float R0C0, C0, PathDelay, r;
  float CurNS, CurNSR, CurNSF, WNS, WNSR, WNSF;
  float TNS, TNSR, TNSF, TNSPI, TNSPO, TNSPIR, TNSPIF, TNSPOR, TNSPOF,
      MinSlack;
  int CurGateNumber, CurNetNum;
  int CurInp, CurOut;
  int NetID, CurGateID, CurPinID, CritOut;
  int k, i;
  int LNetNumber;
  
  int ret;
  char *ret1;

  printf("\n%s\n", "Calculate AT, RT, and slacks");

  //-------------------------------------------------------------
  // input capacity and delay information for nets

  InpFile = fopen(OutFName, "r");
  printf("%s %s\n", "Read capacity and delay information from", OutFName);

  do {
    ret1 = fgets(str, MaxInpStrLength, InpFile);
    ptr = strtok(str, " ");
    strcpy(str1, ptr);
    if (!strncmp(str1, "*Net_Tree_Id", 12)) {	// input NetID
      ptr = strtok(NULL, " ");
      ret = sscanf(ptr, "%d", &NetID);
      ret = fscanf(InpFile, "%s %f", str1, &InpNet[NetID].Cload);
      for (i = 1; i < InpNet[NetID].SinkNumber; i++) {
	ret = fscanf(InpFile, "%s %hd", str1, &CurSink);
	ret = fscanf(InpFile, "%s %f", str1, &InpNet[NetID].Sink[CurSink].Delay);
      }
    }
  }
  while (!feof(InpFile));
  fclose(InpFile);

  //-------------------------------------------------------------
  // calculate AT for each net
  printf("\n%s\n", "Calculate AT");
  for (k = 1; k <= Number_of_Nets; k++) {
    CurNet = InpNet[SortedNet[k]];
    C0 = CurNet.Cload;
    //calculate AT for the current net source
    CurNode = CurNet.Sink[0];
    CurStdGateN = Gate[CurNode.GateID].LibID;
    CurStdGate = StdGate[CurStdGateN];
    SourcePin = CurStdGate.Pins[CurNode.PinID];
    CurGateID = CurNode.GateID;
    CurGate = Gate[CurGateID];
    if (CurStdGateN == 0) {	//source is a primary input
      SourceATR = AT_rise;
      SourceATF = AT_fall;
    } else {			//calculate AT for gate output
      SourceATR = 0;
      SourceATF = 0;
      for (i = 1; i <= CurStdGate.PinNum; i++) {
	CurPin = CurStdGate.Pins[i];
	CurATR = CurGate.ATF[i] + CurPin.delay_r + C0 * CurPin.res_r;
	CurATF = CurGate.ATR[i] + CurPin.delay_f + C0 * CurPin.res_f;
	if (CurATR > SourceATR)
	  SourceATR = CurATR;
	if (CurATF > SourceATF)
	  SourceATF = CurATF;
      }
    }

    Gate[CurNode.GateID].ATR[CurNode.PinID] = SourceATR;
    Gate[CurNode.GateID].ATF[CurNode.PinID] = SourceATF;
#ifdef VERBOSE
    printf("\n%s %s\n%s", "Net", CurNet.Name, "Source");
    printf(" %s%10.4f %s%10.4f", "ATR=", SourceATR, "ATF=", SourceATF);
    if (CurGateID <= PrInpNumber)
      printf("  %s\n", CurGate.Name);
    else
      printf("  o::%s", CurGate.Name);
#endif

    //calculate AT for all sinks of the net
    for (i = 1; i < CurNet.SinkNumber; i++) {
      CurNode = CurNet.Sink[i];
      PathDelay = CurNode.Delay;
      //extract res_r and res_f
      SinkATR = SourceATR + C0 * SourcePin.res_r + PathDelay;
      SinkATF = SourceATF + C0 * SourcePin.res_f + PathDelay;
      Gate[CurNode.GateID].ATR[CurNode.PinID] = SinkATR;
      Gate[CurNode.GateID].ATF[CurNode.PinID] = SinkATF;
#ifdef VERBOSE
      printf("%s", "Sink  ");
      printf(" %s%10.4f %s%10.4f", "ATR=", SinkATR, "ATF=", SinkATF);
      if (CurNode.GateID > PrInpNumber + GateNumber)
	printf("  %s\n", Gate[CurNode.GateID].Name);
      else
	printf("  %s::%s",
	       StdGate[Gate[CurNode.GateID].LibID].Pins[CurNode.
							PinID].
	       Name, Gate[CurNode.GateID].Name);
#endif
    }

  }				//for (k=1; k<=Number_of_Nets; k++)

  //-------------------------------------------------------------
  // calculate RT

  //set RT for primary outputs
  for (i = PrInpNumber + GateNumber + 1; i <= VertexNumber; i++) {
    Gate[i].RTR[0] = RT_rise;
    Gate[i].RTF[0] = RT_fall;
  }

  printf("\n%s\n", "Calculate RT");

  // calculate RT for each net
  for (k = Number_of_Nets; k > 0; k--) {
    CurNet = InpNet[SortedNet[k]];
    C0 = CurNet.Cload;
    //calculate RT for the current net source (gate output)
    CurNode = CurNet.Sink[0];
    CurStdGateN = Gate[CurNode.GateID].LibID;
    CurStdGate = StdGate[Gate[CurNode.GateID].LibID];
    SourcePin = CurStdGate.Pins[CurNode.PinID];
    SourceRTR = BigFloat;
    SourceRTF = BigFloat;
    for (i = 1; i < CurNet.SinkNumber; i++) {
      CurNode = CurNet.Sink[i];
      PathDelay = CurNode.Delay;
      //extract res_r and res_f
      CurRTR = Gate[CurNode.GateID].RTR[CurNode.PinID] - PathDelay;
      if (CurRTR < SourceRTR)
	SourceRTR = CurRTR;
      CurRTF = Gate[CurNode.GateID].RTF[CurNode.PinID] - PathDelay;
      if (CurRTF < SourceRTF)
	SourceRTF = CurRTF;
    }

    CurNode = CurNet.Sink[0];
    SourceRTR -= C0 * SourcePin.res_r;
    SourceRTF -= C0 * SourcePin.res_f;
    Gate[CurNode.GateID].RTR[CurNode.PinID] = SourceRTR;
    Gate[CurNode.GateID].RTF[CurNode.PinID] = SourceRTF;

#ifdef VERBOSE
    printf("\n%s %s\n%s", "Net", CurNet.Name, "Gate output");
    printf(" %s%10.4f %s%10.4f", "RTR=", SourceRTR, "RTF=", SourceRTF);
    if (CurNode.GateID <= PrInpNumber)
      printf("  %s\n", Gate[CurNode.GateID].Name);
    else
      printf("  o::%s", Gate[CurNode.GateID].Name);
#endif


    //calculate RT for all input pins of the gate
    CurStdGateN = Gate[CurNode.GateID].LibID;
    CurStdGate = StdGate[CurStdGateN];
    if (CurStdGateN > 0) {	//calculate RT for gate inputs
      for (i = 1; i <= CurStdGate.PinNum; i++) {
	CurPin = CurStdGate.Pins[i];
	Gate[CurNode.GateID].RTF[i] =
	    SourceRTR - CurPin.delay_r - C0 * CurPin.res_r;
	Gate[CurNode.GateID].RTR[i] =
	    SourceRTF - CurPin.delay_f - C0 * CurPin.res_f;
#ifdef VERBOSE
	printf("%s", "Gate input ");
	printf(" %s%10.4f %s%10.4f",
	       "RTR=", Gate[CurNode.GateID].RTR[i],
	       "RTF=", Gate[CurNode.GateID].RTF[i]);
	if (CurNode.GateID <= PrInpNumber)
	  printf("  %s\n", Gate[CurNode.GateID].Name);
	else
	  printf("  %s::%s",
		 StdGate[Gate[CurNode.GateID].LibID].Pins[i].
		 Name, Gate[CurNode.GateID].Name);
#endif
      }
    }

  }				//for (k=1; k<=Number_of_Nets; k++)



  //-------------------------------------------------------------
  // Write AT, RT and slack for all primary outputs
  WNS = 0;
  WNSR = 0;
  WNSF = 0;
  TNSPI = 0;
  TNSPO = 0;
  TNSPIR = 0;
  TNSPIF = 0;
  TNSPOR = 0;
  TNSPOF = 0;
  InpFile = fopen(TimeFName, "w");
  printf("\n%s %s\n", "Write timing information to the file", TimeFName);

//  fprintf(InpFile,"\n\n Timing for primary inputs (ps)\n");
//  fprintf(InpFile," PinID   AT_rise    RT_rise    Slack_r");
//  fprintf(InpFile,"    AT_fall    RT_fall    Slack_f  Gate name \n");
  for (i = 1; i <= PrInpNumber; i++) {
    CurGate = Gate[i];
    CurNSR = CurGate.RTR[0] - CurGate.ATR[0];
    CurNSF = CurGate.RTF[0] - CurGate.ATF[0];
//    fprintf(InpFile,"%5d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f  %s\n",
//                      i,
//            CurGate.ATR[0],CurGate.RTR[0],CurNSR,
//            CurGate.ATF[0],CurGate.RTF[0],CurNSF,
//            CurGate.Name);
    if (CurNSR > 0)
      CurNSR = 0;
    if (CurNSR < WNSR)
      WNSR = CurNSR;
    if (CurNSF > 0)
      CurNSF = 0;
    if (CurNSF < WNSF)
      WNSF = CurNSF;
    if (CurNSR < CurNSF)
      CurNS = CurNSR;
    else
      CurNS = CurNSF;
    if (CurNS < WNS)
      WNS = CurNS;

    TNSPI += CurNS;
    TNSPIR += CurNSR;
    TNSPIF += CurNSF;
  }


  TNS = TNSPI;
  TNSR = TNSPIR;
  TNSF = TNSPIF;
//  fprintf(InpFile,"\n\n Timing for gates (ps)\n");
//  fprintf(InpFile," PinID   AT_rise    RT_rise    Slack_r");
//  fprintf(InpFile,"    AT_fall    RT_fall    Slack_f  Gate name \n");
  for (i = PrInpNumber + 1; i <= PrInpNumber + GateNumber; i++) {
    CurGate = Gate[i];
    CurStdGate = StdGate[CurGate.LibID];
    CurNSR = CurGate.RTR[0] - CurGate.ATR[0];
    CurNSF = CurGate.RTF[0] - CurGate.ATF[0];
//    fprintf(InpFile,"%5d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f  o::%s",
//                      i,
//            CurGate.ATR[0],CurGate.RTR[0],CurNSR,
//            CurGate.ATF[0],CurGate.RTF[0],CurNSF,
//            CurGate.Name);
    if (CurNSR > 0)
      CurNSR = 0;
    if (CurNSR < WNSR)
      WNSR = CurNSR;
    if (CurNSF > 0)
      CurNSF = 0;
    if (CurNSF < WNSF)
      WNSF = CurNSF;
    if (CurNSR < CurNSF)
      CurNS = CurNSR;
    else
      CurNS = CurNSF;
    if (CurNS < WNS)
      WNS = CurNS;

    TNS += CurNS;
    TNSR += CurNSR;
    TNSF += CurNSF;

    for (k = 1; k <= CurStdGate.PinNum; k++) {
      CurNSR = CurGate.RTR[k] - CurGate.ATR[k];
      CurNSF = CurGate.RTF[k] - CurGate.ATF[k];
#ifdef VERBOSE
      fprintf(InpFile,
	      "%5d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f  %s::%s",
	      i, CurGate.ATR[k], CurGate.RTR[k], CurNSR,
	      CurGate.ATF[k], CurGate.RTF[k], CurNSF,
	      CurStdGate.Pins[k].Name, CurGate.Name);
#endif

      if (CurNSR > 0)
	CurNSR = 0;
      if (CurNSR < WNSR)
	WNSR = CurNSR;
      if (CurNSF > 0)
	CurNSF = 0;
      if (CurNSF < WNSF)
	WNSF = CurNSF;
      if (CurNSR < CurNSF)
	CurNS = CurNSR;
      else
	CurNS = CurNSF;
      if (CurNS < WNS)
	WNS = CurNS;

      TNS += CurNS;
      TNSR += CurNSR;
      TNSF += CurNSF;

    }
  }
//  fprintf(InpFile,"\n\n Timing for primary outputs (ps)\n" );
//  fprintf(InpFile," PinID   AT_rise    RT_rise    Slack_r");
//  fprintf(InpFile,"    AT_fall    RT_fall    Slack_f  Gate name \n");
  MinSlack = BigFloat;
  for (i = PrInpNumber + GateNumber + 1; i <= VertexNumber; i++) {
    CurGate = Gate[i];
    CurNSR = CurGate.RTR[0] - CurGate.ATR[0];
    CurNSF = CurGate.RTF[0] - CurGate.ATF[0];
//    fprintf(InpFile,"%5d %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f  %s\n",
//                      i,
//            CurGate.ATR[0],CurGate.RTR[0],CurNSR,
//            CurGate.ATF[0],CurGate.RTF[0],CurNSF,
//            CurGate.Name);

    if (CurNSR < CurNSF)
      CurNS = CurNSR;
    else
      CurNS = CurNSF;
    if (CurNS < MinSlack) {
      MinSlack = CurNS;
      CritOut = i;
    }

    if (CurNSR > 0)
      CurNSR = 0;
    if (CurNSR < WNSR)
      WNSR = CurNSR;
    if (CurNSF > 0)
      CurNSF = 0;
    if (CurNSF < WNSF)
      WNSF = CurNSF;
    if (CurNSR < CurNSF)
      CurNS = CurNSR;
    else
      CurNS = CurNSF;
    if (CurNS < WNS)
      WNS = CurNS;

    TNSPO += CurNS;
    TNSPOF += CurNSF;
    TNSPOR += CurNSR;
  }
  TNS += TNSPO;
  TNSR += TNSPOR;
  TNSF += TNSPOF;
  printf("\nWorst negative slack for all ports (ps)\n");
  printf(" %s%10.4f %s%10.4f %s%10.4f\n",
	 "WNS=", WNS, "WNS_rise=", WNSR, "WNS_fall=", WNSF);
  printf("Total negative slack for all ports (ps)\n");
  printf(" %s%10.4f %s%10.4f %s%10.4f\n",
	 "TNS=", TNS, "TNS_rise=", TNSR, "TNS_fall=", TNSF);
  printf("Total negative slack for primary ports (ps)\n");
  printf(" %s%10.4f %s%10.4f %s%10.4f\n",
	 "TNS=", TNSPO + TNSPI, "TNS_rise=", TNSPOR + TNSPIR,
	 "TNS_fall=", TNSPOF + TNSPIF);
  printf("Total negative slack for primary inputs (ps)\n");
  printf(" %s%10.4f %s%10.4f %s%10.4f\n",
	 "TNS=", TNSPI, "TNS_rise=", TNSPIR, "TNS_fall=", TNSPIF);
  printf("Total negative slack for primary outputs (ps)\n");
  printf(" %s%10.4f %s%10.4f %s%10.4f\n",
	 "TNS=", TNSPO, "TNS_rise=", TNSPOR, "TNS_fall=", TNSPOF);


  fprintf(InpFile, "\nWorst negative slack for all ports (ps)\n");
  fprintf(InpFile, " %s%10.4f %s%10.4f %s%10.4f\n",
	  "WNS=", WNS, "WNS_rise=", WNSR, "WNS_fall=", WNSF);
  fprintf(InpFile, "Total negative slack for all ports (ps)\n");
  fprintf(InpFile, " %s%10.4f %s%10.4f %s%10.4f\n",
	  "TNS=", TNS, "TNS_rise=", TNSR, "TNS_fall=", TNSF);
  fprintf(InpFile, "Total negative slack for primary ports (ps)\n");
  fprintf(InpFile, " %s%10.4f %s%10.4f %s%10.4f\n",
	  "TNS=", TNSPO + TNSPI, "TNS_rise=", TNSPOR + TNSPIR,
	  "TNS_fall=", TNSPOF + TNSPIF);
  fprintf(InpFile, "Total negative slack for primary inputs (ps)\n");
  fprintf(InpFile, " %s%10.4f %s%10.4f %s%10.4f\n",
	  "TNS=", TNSPI, "TNS_rise=", TNSPIR, "TNS_fall=", TNSPIF);
  fprintf(InpFile, "Total negative slack for primary outputs (ps)\n");
  fprintf(InpFile, " %s%10.4f %s%10.4f %s%10.4f\n",
	  "TNS=", TNSPO, "TNS_rise=", TNSPOR, "TNS_fall=", TNSPOF);


  //-------------------------------------------------------------
  // initialize net priority array
  //-------------------------------------------------------------
  NetPrty = (int *) calloc(Number_of_Nets + 1, sizeof(int));
  for (k = 1; k <= Number_of_Nets; k++)
    NetPrty[k] = Number_of_Nets + 1;

  //-------------------------------------------------------------
  // calculate critical path
  //-------------------------------------------------------------

  //Fill Pin2Net array for each gate
  for (k = 1; k <= Number_of_Nets; k++) {
    CurNet = InpNet[k];
    for (i = 0; i < CurNet.SinkNumber; i++) {
      CurNode = CurNet.Sink[i];
      CurGateID = CurNode.GateID;
      CurPinID = CurNode.PinID;
      Gate[CurGateID].Pin2Net[CurPinID] = k;
    }
  }

  fprintf(InpFile, "\nCritical path:\n");
  CurGate = Gate[CritOut];
  CurStdGate = StdGate[CurGate.LibID];
  CurPinID = 0;
  while (CurGate.LibID > 0) {
    CurNetNum = CurGate.Pin2Net[CurPinID];

    //move the net in the head of SortedNet
    k = 1;
    while (SortedNet[k] != CurNetNum)
      k++;
    for (i = k; i > 1; i--) {
      SortedNet[i] = SortedNet[i - 1];
    }
    SortedNet[1] = CurNetNum;

    NetPrty[CurNetNum] = 0;
    CurNet = InpNet[CurNetNum];
    if (CurGate.LibID == StdGateNumber)
      fprintf(InpFile, "ATR=%10.4f ATF=%10.4f   %s\n   %s (%d) \n",
	      CurGate.ATR[0], CurGate.ATF[0], CurGate.Name,
	      CurNet.Name, CurNetNum);
    else
      fprintf(InpFile, "ATR=%10.4f ATF=%10.4f   %s::%s   %s (%d)\n",
	      CurGate.ATR[CurPinID], CurGate.ATF[CurPinID],
	      CurStdGate.Pins[CurPinID].Name, CurGate.Name,
	      CurNet.Name, CurNetNum);
    CurGate = Gate[CurNet.Sink[0].GateID];
    CurStdGate = StdGate[CurGate.LibID];
    //choose most critical input port of the gate
    WNS = BigFloat;
    if (CurStdGate.PinNum > 0) {
      fprintf(InpFile, "ATR=%10.4f ATF=%10.4f   O::%s",
	      CurGate.ATR[0], CurGate.ATF[0], CurGate.Name);
      for (k = 1; k <= CurStdGate.PinNum; k++) {
	CurNSR = CurGate.RTR[k] - CurGate.ATR[k];
	CurNSF = CurGate.RTF[k] - CurGate.ATF[k];
	if (CurNSR < CurNSF)
	  CurNS = CurNSR;
	else
	  CurNS = CurNSF;
	if (CurNS < WNS) {
	  WNS = CurNS;
	  CurPinID = k;
	}
      }
    } else
      fprintf(InpFile, "%s", CurGate.Name);

  }
  fclose(InpFile);




}				//void Timing()
