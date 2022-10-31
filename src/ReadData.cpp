#include <string.h>
#include "routing.h"
//-------------------------------------------------------------
// Global variables
extern TLAYER *Layer;           //Layer[l] - all data for layer l
extern TNET *Net;               //Net[s] - network i info
extern int Size_X;
extern int Size_Y;
extern short Size_Z;
extern int Number_of_Nets;

void
InitDataStructure (const char *inputFileName)   //read data from file *.dat
{
  FILE *InpFile;
  InpStr str, str1, str2;
  NameStr nstr;
  short stmp, ctmp, l;
  int i, j, itmp, NumSinks, ix, iy, iz, iw, in, ie, is, iu, id, indp;
  float ftmp;
  
  int ret;
  char *ret1;

  //-------------------------------------------------------------
  // open input file
  InpFile = fopen (inputFileName, "r");
  if (!InpFile) {
    printf ("Input file %s could not be opened - check the file\n",
            inputFileName);
    exit (1);
  }
  // read input file
  //read grid Size_X - Size_Y
  printf ("Successfully opened %s for reading data\n", inputFileName); //verbose
  /* Marker - *Routing area */
  ret1 = fgets (str, 80, InpFile); 
  ret1 = fgets (str, 80, InpFile);
  ret = sscanf (str, "%s %d", str2, &itmp);  /* Columns */
  Size_X = itmp - 1;
  ret1 = fgets (str, 80, InpFile);
  ret = sscanf (str, "%s %d", str2, &itmp);   /* Rows */
  Size_Y = itmp - 1;
  //Layer information
  ret1 = fgets (str, 80, InpFile);
  ret = sscanf (str, "%s %hd", str2, &stmp);  /* Number of Layers */
  Size_Z = stmp;

  printf ("# of Rows %d, # of Columns %d, # of Layers %d\n", Size_X+1, Size_Y+1, Size_Z);

  //create data array for layer information
  Layer = (TLAYER *) calloc (Size_Z + 1, sizeof (TLAYER));

  for (l = 1; l <= Size_Z; l++) {       //input information about layer l
    ret1 = fgets (str, 80, InpFile);               /* Ignore this */
    ret1 = fgets (str, 80, InpFile);               /* Layer Id    */
    ret = sscanf (str, "%s %hd", str2, &stmp);
    Layer[l].Id = stmp;
    ret1 = fgets (str, 80, InpFile);               /* Direction */
    ret = sscanf (str, "%s %hd", str2, &stmp);
    Layer[l].dir = stmp;
    ret1 = fgets (str, 80, InpFile);              /* tracks */
    ret = sscanf (str, "%s %d", str2, &itmp);
    Layer[l].Tracks = itmp;
    ret1 = fgets (str, 80, InpFile);               /* uptracks */
    ret = sscanf (str, "%s %d", str2, &itmp);
    Layer[l].UpTracks = itmp;
    ret1 = fgets (str, 80, InpFile);               /* capacitance */
    ret = sscanf (str, "%s %f", str2, &ftmp);
    Layer[l].cap = ftmp;
    ret1 = fgets (str, 80, InpFile);               /* resistance */
    ret = sscanf (str, "%s %f", str2, &ftmp);
    Layer[l].res = ftmp;
    ret1 = fgets (str, 80, InpFile);               /* up_capacitance ??*/
    ret = sscanf (str, "%s %f", str2, &ftmp);
    Layer[l].up_cap = ftmp;                  /* up_resistance ??*/
    ret1 = fgets (str, 80, InpFile);
    ret = sscanf (str, "%s %f", str2, &ftmp);
    Layer[l].up_res = ftmp;
  }

  //Net information
  ret1 = fgets (str, 80, InpFile);
  ret1 = fgets (str, 80, InpFile);
  ret = sscanf (str, "%s %d", str2, &itmp);
  Number_of_Nets = itmp;

  //create data array for net information
  Net = (TNET *) calloc (Number_of_Nets + 1, sizeof (TNET));
  for (i = 1; i <= Number_of_Nets; i++) {
    ret1 = fgets (str, 80, InpFile);
    ret1 = fgets (str, 80, InpFile);
    ret = sscanf (str, "%s %d %hd", str2, &itmp, &ctmp);
    Net[i].Id = itmp;
    Net[i].critical = ctmp;
    Net[i].CriticalDelay = 0.0;
    Net[i].Delay_in_critical_sink = 0.0;
    ret1 = fgets (str, 80, InpFile);
    ret = sscanf (str, "%s %f", str2, &ftmp);
    Net[i].drive = ftmp;
    ret1 = fgets (str, 80, InpFile);
    ret = sscanf (str, "%s %d", str2, &itmp);
    Net[i].Number = itmp;

    Net[i].Sink = (TNODE *) calloc (itmp + 1, sizeof (TNODE));

    //sink info
    for (j = 1; j <= itmp; j++) {
      ret1 = fgets (str, 80, InpFile);
      ret = sscanf (str, "%s %hd", str2, &stmp);
      Net[i].Sink[j].PinId = stmp;
      ret1 = fgets (str, 80, InpFile);
      ret = sscanf (str, "%d %d %d", &ix, &iy, &iz);
      Net[i].Sink[j].x = ix;
      Net[i].Sink[j].y = iy;
      Net[i].Sink[j].z = iz;
      ret1 = fgets (str, 80, InpFile);
      ret = sscanf (str, "%s %f", str2, &ftmp);
      Net[i].Sink[j].cap = ftmp;
      ret1 = fgets (str, 80, InpFile);
      ret = 	sscanf (str, "%s %hd %hd", str2, &stmp, &ctmp);
      Net[i].Sink[j].type = stmp;
      Net[i].Sink[j].critical = ctmp;
      Net[i].Sink[j].No = -1;
      Net[i].Sink[j].w = -1;
      Net[i].Sink[j].n = -1;
      Net[i].Sink[j].e = -1;
      Net[i].Sink[j].s = -1;
      Net[i].Sink[j].u = -1;
      Net[i].Sink[j].d = -1;
      Net[i].Sink[j].pred = -1;
      Net[i].Sink[j].pred_dir = -1;
      Net[i].Sink[j].delay = -1;
      Net[i].Sink[j].SubtreeCap = -1;
      Net[i].Sink[j].EdgeDelay = -1;
    }                           //end for j
  }                             //end for i
  fclose (InpFile);             // close input file
//print on screen the data
  printf ("Completed getting input\n");
  printf ("Design has %d initial nets\n", Number_of_Nets);
  printf ("%s %hd\n", "the number of layers ", Size_Z);
  printf ("%s %d %s %d\n", "grid (cols x rows) ", Size_X + 1, " x ",
          Size_Y + 1);
  /*for (l = 1; l <= Size_Z; l++) {
    printf ("%s %d %s\n", "  *layer ", l, ":");
    printf ("%s %d\n", "number of tracks ", Layer[l].Tracks);
    printf ("%s %d\n", "number of up via tracks ", Layer[l].UpTracks);
    printf ("%s %f\n", "layer capacitance ", Layer[l].cap);
    printf ("%s %f\n", "layer resistance ", Layer[l].res);
    printf ("%s %f\n", "up via capacitance ", Layer[l].up_cap);
    printf ("%s %f\n", "up via resistance ", Layer[l].up_res);
  }*/
}
