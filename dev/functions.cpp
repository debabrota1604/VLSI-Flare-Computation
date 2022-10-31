#include "include.h"
#include "class_definitions.cpp"
#include<math.h>
#define testFile "../output/desmal_1.out"
#define outFile "output.txt"
#define outfile_vacancy "output_vacancy_matrix.txt"
#define outfile_vacancy2 "output_vacancy_matrix2.txt"

typedef char InpStr[MaxInpStrLen];
typedef char NameStr[MaxNameStrLen];
extern FILE *filePtr;
extern ofstream out;
extern int grid_x,grid_y,grid_z;
extern Info info;

void scanInput(const char *fileName)
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
	sscanf (inpStr1, "%s %s %s %s %s %c %d", namestr1, namestr2, namestr3, namestr4, namestr5, &char1, &int1);  /* Columns <No of nodes in X = 64>*/
	
	/* Column */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %s %s %s %s %c %d", namestr1, namestr2, namestr3, namestr4, namestr5, &char1, &int2);  /* Rows <No of nodes in Y = 64>*/
	
	/* Layer */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %s %s %c %d", namestr1, namestr2, namestr3, &char1, &int3);  /* Layer <No of Layers = 2>*/
	
	/* Net */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %d", namestr1, &int4);  /* Layer <Number_of_nets= 11507>*/
	
	/* Trees */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %d", namestr1, &int5);  /* Layer <Max_Number_of_Trees= 8>*/
	
	/* Iterations */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %d", namestr1, &int6);  /* Layer <MaxIter= 11>*/
	
	/* qq */
	fgets (inpStr1, MaxInpStrLen, filePtr);
	sscanf (inpStr1, "%s %d", namestr1, &int7);  /* Layer <Max_qq= 2>*/
	
	
	grid_x = int1;
	grid_y = int2;
	grid_z = int3;

	info.setInfo(int1, int2, int3, int4, int5, int6, int7);
}

void writeOutput()
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
	out.close();
}

void computeMinDelayConfig()
{
	Min_delay_config m;
	m.set_min_delay_config(info);	
	
	//now m is a min_delay config based array of nets.
	//we now compute the occupancy of each node
	
	int nodeX, nodeY, nodeZ;
	
	double occupancy[grid_z][grid_y][grid_x],overall_delay=0.0;
	
	cout << "\nCalculating minimum delay flare...\n";
	
	for(int c1=0;c1<grid_x;c1++)
	{
		for(int c2=0;c2<grid_y;c2++)
		{
			for(int c3=0;c3<grid_z;c3++)
			{
				occupancy[c3][c2][c1]=0;
			}			
		}
	}	
	
	cout << "Calculating occupancy matrix and extending it to proper size...";
	
	int max_occupancy = 0;	
	for(int counter1=0;counter1<m.getMinDelayTotalNets();counter1++)
	{//this loop runs for each net
		
		Tree tree  = m.ot_net[counter1].get_tree();
		
		for(int counter2=0; counter2<tree.getNodeCount(); counter2++)
		{//this loop runs for each node of the one-tree net
			
			Node node = tree.getNodeAtIndex(counter2);
			
			overall_delay += node.getEdgeDelay();
			
			Dimension dimension = node.getDimension();
			//dimension.printDimension();
			nodeX = dimension.getX();
			nodeY = dimension.getY();
			nodeZ = dimension.getZ();
			
			//increment the corresponding counter in the occupancy matrix.
			occupancy[nodeZ-1][nodeY-1][nodeX-1]++;
			if(occupancy[nodeZ-1][nodeY-1][nodeX-1] > max_occupancy) max_occupancy = occupancy[nodeZ-1][nodeY-1][nodeX-1];
					
		}
	}
	//occupancy calculation is done!
	
	
	//calculating vacancy matrix and writing the data into a file
	for(int c3=0;c3<grid_z;c3++)
	{
		for(int c2=0;c2<grid_y;c2++)
		{
			for(int c1=0;c1<grid_x;c1++)
			{
				//calculate the occupancy which is allocation/available.
				//We approximate the available value to the max_occupancy as the desired value not available
				occupancy[c3][c2][c1]/=max_occupancy;
				
				//now we calculate the vacancy
				occupancy[c3][c2][c1] = 1 - occupancy[c3][c2][c1];
			}
		}
	}	
	//creating extended matrix	
	int ROW,COL,LAYER;
	ROW = grid_x;
	COL = grid_y;
	LAYER = grid_z;
	int doubleRow = 2 * ROW, doubleCol = 2 * COL;
	double temp[LAYER][doubleRow][doubleCol];
	

	
	for(int c3 =0;c3 < LAYER;c3 ++ )
	{
		for(int c2 = 0; c2 < doubleCol; c2++)
		{
			for(int c1 = 0; c1 < doubleRow ; c1++)
			{
				if(c1 < (ROW/2) || c1 >= (ROW + ROW/2)  ||  c2 < (COL/2) || c2 >= (COL + COL/2))
				{
					temp[c3][c2][c1] = 0;
				}
				else
				{
					temp[c3][c2][c1] = occupancy[c3][c2 - (COL/2)][c1 - (ROW/2)];
				}
			}
		}
	}		
    //cout << "Extended input matrix completed!!!!" <<endl;	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cout << "done! \nGenerating Kernel for convolution...";

	double kernel[grid_y][grid_x];	
	//kernel computation code
    int i,j,x,y,start_row,end_row,start_col,end_col;
    double sigma = 5.0, max=0.0;
    double r, s = 2.0 * sigma * sigma;
    
	int max_possible_pattern_area = 15;
    //sum is for normalization
    double sum = 0.0;
    
    double **gKernel = (double **) new double[sizeof(double *) * (ROW+1) ];
    for(i=1;i<=ROW;i++)
    {
         gKernel[i]=(double*)new double[sizeof(double) * (COL+1)];
    }
    
    
    for(i=1;i<=ROW;i++)
    {
        for(j=1;j<=COL;j++)
		{
			gKernel[i][j]=0.0;
		}
    }
   
    if((ROW%2)!=0)
    {
        start_row=(-1) * ((ROW-1)/2);
        start_col=(-1) * ((COL-1)/2);
        end_row=(ROW-1)/2;
        end_col=(COL-1)/2;
   
        // generate 5x5 kernel
        for (x = start_row; x <= end_row; x++)
        {
             for(y = start_col; y <= end_col; y++)
             {
					r = sqrt(x*x + y*y);
					gKernel[x + end_row + 1][y + end_col + 1] = (exp(-(r*r)/s))/(M_PI * s);
					gKernel[x + end_row + 1][y + end_col + 1]=(gKernel[x + end_row + 1][y + end_col + 1])*(s * M_PI);//multiplying each term by 2*pi*sigma^2
					sum += gKernel[x + end_row + 1][y + end_col + 1];
       
             }
        }
    }
    else
    {
         start_row=(-1) * (ROW/2);
         start_col=(-1) * (COL/2);
         end_row=(ROW-1)/2;
         end_col=(COL-1)/2;
   
         // generate 5x5 kernel
         for (x = start_row; x <= end_row; x++)
         {
             for(y = start_col; y <= end_col; y++)
             {
					r = sqrt(x*x + y*y);
					gKernel[x + end_row + 2][y + end_col + 2] = (exp(-(r*r)/s))/(M_PI * s);
					gKernel[x + end_row + 2][y + end_col + 2]=(gKernel[x + end_row + 2][y + end_col + 2])*(s * M_PI);//multiplying each term by 2*pi*sigma^2
					sum += gKernel[x + end_row + 2][y + end_col + 2];
       
             }
         }
    }
    max=0.0,sum = 0.0;   
    for (i=1;i<=ROW;i++)
    {
        for(j=1;j<=COL;j++)
		{
			if(max < gKernel[i][j])
			{
				max=gKernel[i][j];
				sum+=gKernel[i][j];				
			}
		}
    }
    // normalize the Kernel
    for(i = 1; i <= ROW; ++i)
    {
        for(j = 1; j <= COL; ++j)
        {
            gKernel[i][j] /= max; 
		}
	}
    
     
     for(int i=0;i<ROW;i++)
     {
		 for(int j=0;j<COL;j++)
		 {
			 kernel[i][j] = gKernel[i+1][j+1];			 
		 }
	 }
	
	cout << "done! \nStarting convolution for extended input  matrix " << ROW << " * " << COL << " ...";
	
	double output[LAYER][ROW][COL];
	for(int c5=0;c5<LAYER;c5++)
	{
		for(int c1 = 0; c1 < ROW; c1++)
		{
			for(int c2 = 0; c2 < COL; c2++)
			{
				//here we can adress each location of the output array			
				output[c5][c1][c2] = 0;
			
				for(int c3 = 0;c3 < ROW; c3++)
				{
					for(int c4=0; c4 <COL ; c4++)
					{
						output[c5][c1][c2] += temp[c5][c1+c3][c2+c4] * kernel[c3][c4]; 
					}
				}
			}
		}
	} 
	cout << "done!" <<endl;
	
	double min=9999.99, avg=0.0, total_sum=0.0;
	max=0.0;//redefinition, already declared...
	for(int c1=0;c1<LAYER;c1++)
	{
		for(int c2=0;c2<ROW;c2++)
		{
			for(int c3=0;c3<COL;c3++)
			{
				if(min > output[c1][c2][c3]) min = output[c1][c2][c3];
				if(max < output[c1][c2][c3]) max = output[c1][c2][c3];
				total_sum += output[c1][c2][c3];
			}
		}
	}	
	avg = total_sum / (LAYER*ROW*COL);	
	cout << "Min flare: " << min <<endl;
	cout << "Max flare: " << max <<endl;
	cout << "Avg flare: " << avg <<endl;	
	cout << "Total delay: " << overall_delay <<endl;	
}

void computeMinCDconfig()
{	
	Min_centre_distance_config m;	
	m.set_min_cd_config(info);
	
	//now m is a min_delay config based array of nets.
	//we now compute the occupancy of each node
	
	int nodeX, nodeY, nodeZ;
	
	double occupancy[grid_z][grid_y][grid_x],overall_delay=0.0;
	
	cout << "\nCalculating minimum distance flare...\n";	
	
	for(int c1=0;c1<grid_x;c1++)
	{
		for(int c2=0;c2<grid_y;c2++)
		{
			for(int c3=0;c3<grid_z;c3++)
			{
				occupancy[c3][c2][c1]=0;
			}			
		}
	}	
	
	cout << "Calculating occupancy matrix and extending it to proper size...";
	
	int max_occupancy = 0;	
	for(int counter1=0;counter1<m.getMinDelayTotalNets();counter1++)
	{//this loop runs for each net
		
		Tree tree  = m.ot_net[counter1].get_tree();
		
		for(int counter2=0; counter2<tree.getNodeCount(); counter2++)
		{//this loop runs for each node of the one-tree net
			
			Node node = tree.getNodeAtIndex(counter2);
			overall_delay += node.getEdgeDelay();
			
			Dimension dimension = node.getDimension();
			//dimension.printDimension();
			nodeX = dimension.getX();
			nodeY = dimension.getY();
			nodeZ = dimension.getZ();
			
			//increment the corresponding counter in the occupancy matrix.
			occupancy[nodeZ-1][nodeY-1][nodeX-1]++;
			if(occupancy[nodeZ-1][nodeY-1][nodeX-1] > max_occupancy) max_occupancy = occupancy[nodeZ-1][nodeY-1][nodeX-1];
					
		}
	}
	//occupancy calculation is done!
	
	//calculating vacancy matrix and writing the data into a file
	for(int c3=0;c3<grid_z;c3++)
	{
		for(int c2=0;c2<grid_y;c2++)
		{
			for(int c1=0;c1<grid_x;c1++)
			{
				//calculate the occupancy which is allocation/available.
				//We approximate the available value to the max_occupancy as the desired value not available
				occupancy[c3][c2][c1]/=max_occupancy;
				
				//now we calculate the vacancy
				occupancy[c3][c2][c1] = 1 - occupancy[c3][c2][c1];
			}
		}
	}

	//creating extended matrix	
	int ROW,COL,LAYER;
	ROW = grid_x;
	COL = grid_y;
	LAYER = grid_z;
	int doubleRow = 2 * ROW, doubleCol = 2 * COL;
	double temp[LAYER][doubleRow][doubleCol];
	

	
	for(int c3 =0;c3 < LAYER;c3 ++ )
	{
		for(int c2 = 0; c2 < doubleCol; c2++)
		{
			for(int c1 = 0; c1 < doubleRow ; c1++)
			{
				if(c1 < (ROW/2) || c1 >= (ROW + ROW/2)  ||  c2 < (COL/2) || c2 >= (COL + COL/2))
				{
					temp[c3][c2][c1] = 0;
				}
				else
				{
					temp[c3][c2][c1] = occupancy[c3][c2 - (COL/2)][c1 - (ROW/2)];
				}
			}
		}
	}
    //cout << "Extended input matrix completed!!!!" <<endl;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	cout << "done! \nGenerating Kernel for convolution...";

	double kernel[grid_y][grid_x];	
	//kernel computation code
    int i,j,x,y,start_row,end_row,start_col,end_col;
    double sigma = 5.0, max=0.0;
    double r, s = 2.0 * sigma * sigma;
    
	int max_possible_pattern_area = 15;
    //sum is for normalization
    double sum = 0.0;
    double **gKernel = (double **) new double[sizeof(double *) * (ROW+1) ];
    for(i=1;i<=ROW;i++)
    {
         gKernel[i]=(double*)new double[sizeof(double) * (COL+1)];
    }
   
    for(i=1;i<=ROW;i++)
    {
        for(j=1;j<=COL;j++)
		{
			gKernel[i][j]=0.0;
		}
    }
   
    if((ROW%2)!=0)
    {
        start_row=(-1) * ((ROW-1)/2);
        start_col=(-1) * ((COL-1)/2);
        end_row=(ROW-1)/2;
        end_col=(COL-1)/2;
   
        // generate 5x5 kernel
        for (x = start_row; x <= end_row; x++)
        {
             for(y = start_col; y <= end_col; y++)
             {
					r = sqrt(x*x + y*y);
					gKernel[x + end_row + 1][y + end_col + 1] = (exp(-(r*r)/s))/(M_PI * s);
					gKernel[x + end_row + 1][y + end_col + 1]=(gKernel[x + end_row + 1][y + end_col + 1])*(s * M_PI);//multiplying each term by 2*pi*sigma^2
					sum += gKernel[x + end_row + 1][y + end_col + 1];
       
             }
        }
    }
    else
    {
         start_row=(-1) * (ROW/2);
         start_col=(-1) * (COL/2);
         end_row=(ROW-1)/2;
         end_col=(COL-1)/2;
   
         // generate 5x5 kernel
         for (x = start_row; x <= end_row; x++)
         {
             for(y = start_col; y <= end_col; y++)
             {
					r = sqrt(x*x + y*y);
					gKernel[x + end_row + 2][y + end_col + 2] = (exp(-(r*r)/s))/(M_PI * s);
					gKernel[x + end_row + 2][y + end_col + 2]=(gKernel[x + end_row + 2][y + end_col + 2])*(s * M_PI);//multiplying each term by 2*pi*sigma^2
					sum += gKernel[x + end_row + 2][y + end_col + 2];
       
             }
         }
    }
    max=0.0,sum = 0.0;   
    for (i=1;i<=ROW;i++)
    {
        for(j=1;j<=COL;j++)
		{
			if(max < gKernel[i][j])
			{
				max=gKernel[i][j];
				sum+=gKernel[i][j];				
			}
		}
    }
    // normalize the Kernel
    for(i = 1; i <= ROW; ++i)
    {
        for(j = 1; j <= COL; ++j)
        {
            gKernel[i][j] /= max; 
		}
	}
    
     
     for(int i=0;i<ROW;i++)
     {
		 for(int j=0;j<COL;j++)
		 {
			 kernel[i][j] = gKernel[i+1][j+1];			 
		 }
	 }
	
	cout << "done! \nStarting convolution for extended input  matrix " << ROW << " * " << COL << " ...";
	
	double output[LAYER][ROW][COL];
	for(int c5=0;c5<LAYER;c5++)
	{
		for(int c1 = 0; c1 < ROW; c1++)
		{
			for(int c2 = 0; c2 < COL; c2++)
			{
				//here we can adress each location of the output array			
				output[c5][c1][c2] = 0;
			
				for(int c3 = 0;c3 < ROW; c3++)
				{
					for(int c4=0; c4 <COL ; c4++)
					{
						output[c5][c1][c2] += temp[c5][c1+c3][c2+c4] * kernel[c3][c4]; 
					}
				}
			}
		}
	} 
	cout << "done!" <<endl;
	
	double min=9999.99, avg=0.0, total_sum=0.0;
	max=0.0;//redefinition, already declared...
	for(int c1=0;c1<LAYER;c1++)
	{
		for(int c2=0;c2<ROW;c2++)
		{
			for(int c3=0;c3<COL;c3++)
			{
				if(min > output[c1][c2][c3]) min = output[c1][c2][c3];
				if(max < output[c1][c2][c3]) max = output[c1][c2][c3];
				total_sum += output[c1][c2][c3];
			}
		}
	}
	
	avg = total_sum / (LAYER*ROW*COL);
	
	cout << "Min flare: " << min <<endl;
	cout << "Max flare: " << max <<endl;
	cout << "Avg flare: " << avg <<endl;	
	cout << "Total delay: " << overall_delay <<endl;	
}
