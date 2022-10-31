#include "include.h"
#include "Info.cpp"

	
#define testFile "desmal_1.out"
#define outFile "output.txt"

extern FILE *filePtr;
extern ofstream out;
extern Info info;

void write_output()
{	
	out.open(outFile);
	if(!out)
	{
		cerr << "Cannot open output File " << outFile << ".\n";
	}
	else
	{
		cout << "Successfully opened file " << outFile << ". Printing data..." << endl; 
	}

	info.printInfo();
}
