#include "include.h"
#include "Info.cpp"
#include<stdlib.h>

typedef char InpStr[MaxInpStrLen];
typedef char NameStr[MaxNameStrLen];

#define dprint printf
#define testFile "desmal_1.out"
#define outFile "output.txt"

extern FILE *filePtr;
extern ofstream out;
extern Info info;

void scan_input(const char *fileName)
{
	filePtr = NULL;
	InpStr inpStr1, inpStr2, inpStr3;
	NameStr namestr1, namestr2, namestr3, namestr4, namestr5, namestr6, namestr7, namestr8;
	short stmp, ctmp, l;
	int int1, int2, int3, int4, int5, int6, int7;
	char char1,char2;
		
	//filePtr = fopen (fileName, "r");
	filePtr = fopen (testFile, "r");
	if (!filePtr) {
		printf ("Input file %s could not be opened - check the file\n", testFile);
		exit(1);
	}
	
	printf ("Successfully opened %s for reading data\n", testFile); 
	
	/* Row */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %s %s %s %s %c %d", &namestr1, &namestr2, &namestr3, &namestr4, &namestr5, &char1, &int1);  /* Columns <No of nodes in X = 64>*/
	
	/* Column */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %s %s %s %s %c %d", &namestr1, &namestr2, &namestr3, &namestr4, &namestr5, &char1, &int2);  /* Rows <No of nodes in Y = 64>*/
	
	/* Layer */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %s %s %c %d", &namestr1, &namestr2, &namestr3, &char1, &int3);  /* Layer <No of Layers = 2>*/
	
	/* Net */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %d", &namestr1, &int4);  /* Layer <Number_of_nets= 11507>*/
	
	/* Trees */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %d", &namestr1, &int5);  /* Layer <Max_Number_of_Trees= 8>*/
	
	/* Iterations */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %d", &namestr1, &int6);  /* Layer <MaxIter= 11>*/
	
	/* qq */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %d", &namestr1, &int7);  /* Layer <Max_qq= 2>*/

	info.setInfo(int1, int2, int3, int4, int5, int6, int7);
}
