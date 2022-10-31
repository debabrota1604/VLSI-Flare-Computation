#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

// Include basic Header File
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define VERBOSE

#define MaxInpStrLength 256 // max string length in input file
#define MaxFieldLength 50   // max field length
#define MaxNameLength 25    // max name length
#define MaxLayerNumber 7    // max layers number
#define MaxStdPins 7        // max pin number per gate
#define MaxSinkNumber 40    // max sink number per net
#define BigFloat 9999999.0


// Define File Names

// small example
/*
#define InpFName "desmal.dat" // Input file for MAD
#define OutFName "desmal_2.out" // Output file
#define GriFName "desmal.gri" // Grid, layers and nets
#define GenlibFName "desmal.genlib" //  Standard gate library
#define SchFName "desmal.sch"       //  Gate to nets file
#define BlifFName "desmal.blif"     //  Primary inputs/outputs file
#define TimeFName "desmal.time"     //  Primary inputs/outputs file
*/



// large example

#define InpFName "desm.dat" // Input file for MAD
#define OutFName "desm.out" // Output file
#define GriFName "des.gri" // Grid, layers and nets
#define GenlibFName "des.genlib" //  Standard gate library
#define SchFName "des.sch"       //  Gate to nets file
#define BlifFName "des.blif"     //  Primary inputs/outputs file
#define TimeFName "desm.time"     //  Primary inputs/outputs file


//-------------------------------------------------------------
// Global constants

#define hor 1    //routing directions 0x
#define ver 2    //routing directions 0y
#define norout 0 //no routing
#define west 1
#define north 2
#define east 3
#define south 4
#define up 5
#define down 6
#define MaxFloat 99999999.9
#define MaxInt   99999
#define INV 1
#define NOINV 2
#define UNKNOWN 0
//--------------------------------------------------------------
// Type definitions

typedef char InpStr[MaxInpStrLength];
typedef char FieldStr[MaxFieldLength];
typedef char NameStr[MaxNameLength];

// Layer description
typedef struct
{ short Id;       //layer's #
  short dir;      //direction of routing (1=hor or 2=ver)
  int   Tracks;   //max number of wires per bin
  int   UpTracks; //max number of wires per up via
  float cap;      //capacitance
  float res;      //resistance
  float up_cap;   //up via capacitance. In the 1st version = 0
  float up_res;   //up via resistance
} TLAYER;

typedef struct
{
  char Name[3];   // pin name
  short type;     // type (0=UNKNOWN, 1=INV, 2=NOINV)
  float cap;      // input capacitance
  float res_r;    // rise resistance
  float res_f;    // fall resistance
  float delay_r;  // rise delay
  float delay_f;  // fall delay
} TSTDPIN;

//Standard gate description
typedef struct
{
  char Name[MaxNameLength];    // standard gate name
  float r_0;                   // drice resistance
  short PinNum;                // number of pins (0 - output)
  TSTDPIN Pins[MaxStdPins];    // array of pins
} TSTDGATE;

// Gate instance description
typedef struct
{
  int   x;        //x-coordinate
  int   y;        //y-coordinate
  short z;        //z-coordinate (layer #)
  short LibID;    //standard library number
  int Pin2Net[MaxStdPins]; // reference net number for each pin
  float ATR[MaxStdPins];  //arrival time - rise
  float ATF[MaxStdPins];  //arrival time - fall
  float RTR[MaxStdPins];  //required time - rise
  float RTF[MaxStdPins];  //required time - fall
  char Name[MaxNameLength];    //gate name
} TGATE;

// Node description (for input net)
typedef struct
{
  int   GateID;   //Gate ID
  short PinID;    //Pin ID in the gate
  short Critical; //Criticality of the node
  float Delay;    //Pin routing delay
} TSNODE;

// Net description
typedef struct
{ int   Id;                    //net #
  float drive;                 //root resistance
  float Cload;                 //load capacitance
  short Critical;           //criticality of the net
  short SinkNumber;            //number of sinks including the source
  TSNODE Sink[MaxSinkNumber];  //array of sinks
  char Name[MaxNameLength];    //net name
} TSNET;

// Edge description
typedef struct
{ int inp;   // start gate ID
  int out;   // finish gate ID
  float delay;
} TEDGE;

//------------------------------------------------------------------------------
// Function prototype declaration

//Groute.cpp
int main();

//InputGenlib.cpp
void InputGenlib();
void InputBlif();

//InputSch.cpp
void InputSch();

//InputGri.cpp
void InputGri();

//RankCalc.cpp
void RankCount();
void NetOrderRank();
void NetOrderPrty();

//WriteDat.cpp
void WriteDat();

//Timing.cpp
void Timing();

#endif
