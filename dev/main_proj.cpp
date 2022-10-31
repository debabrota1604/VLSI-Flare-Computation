#include "include.h"
#include "class_definitions.cpp"
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>

FILE *filePtr;
ofstream out;
Info info;//global info object which holds the scanned data
int grid_x,grid_y,grid_z;

int main(int argc, const char *argv[])
{
	int i, j, k, l, no, s, v1, v2;
	int Max_Overflow;             //maximal overflow of global edge
	int Number_of_overflows;      //number of global edges with overflow
	int Wire_length;              //total (of all trees) wire length
	int Total_overflow;           //the sum of overflows of all global edges
	const char *fileName = NULL;
		
	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	double start_time,end_time;	
	
	//capture the system time before the beginning of scanning input
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	start_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)
	
	scanInput(fileName);//passing reference to info object
	
	//capture the system time after the scanning is done
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	end_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)
	//print the difference
	cout << "Input file Scanning completed at " << end_time-start_time << " seconds." << endl;
	
	info.printInfo();
	
	
	/*//capture the system time before the beginning of writing input
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	start_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)
	
	writeOutput();
	
	//capture the system time after the writing is done
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	end_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)
	//print the difference
	cout << "Output writing completed at " << end_time-start_time << " seconds." << endl;*/
	
	//capture the system time before the beginning computation
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	start_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)
	
	computeMinCDconfig();
	
	//capture the system time after the writing is done
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	end_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)
	//print the difference
	cout << "Minimum distance flare computation completed at " << end_time-start_time << " seconds." << endl;
	
	//capture the system time before the beginning computation
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	start_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)
	
	computeMinDelayConfig();
	
	//capture the system time after the writing is done
	gettimeofday(&tv, &tz);
	tm=localtime(&tv.tv_sec);
	end_time = tm->tm_mday*24*3600 + tm->tm_hour*3600 + tm->tm_min*60 + tm->tm_sec + tv.tv_usec*0.000001; //(gives you time in seconds upto the 6th decimal place)
	//print the difference
	cout << "Minimum Delay flare computation completed at " << end_time-start_time << " seconds." << endl;	

	
	return 0;
}


