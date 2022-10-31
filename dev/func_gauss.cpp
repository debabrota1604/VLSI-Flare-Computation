#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<iostream>

using namespace std;

double **gKernel,**gKernel_area;
int max_possible_pattern_area = 15;

void estimate_density_guass(int ROW,int COL)
{
    FILE *gfile;
    int i,j,x,y,start_row,end_row,start_col,end_col;
    double sigma = 5.0, max=0.0;
    double r, s = 2.0 * sigma * sigma;
 
    //sum is for normalization
    double sum = 0.0;
    
   
    gKernel=(double**)malloc(sizeof(double*) * (ROW+1));
    for(i=1;i<=ROW;i++)
    {
         gKernel[i]=(double*)malloc(sizeof(double) * (COL+1));
    }
    for(i=1;i<=ROW;i++)
    {
        for(j=1;j<=COL;j++)
    {
        gKernel[i][j]=0.0;
    }
    }
   
    gKernel_area=(double**)malloc(sizeof(double*) * (ROW+1));
    for(i=1;i<=ROW;i++)
    {
        gKernel_area[i]=(double*)malloc(sizeof(double) * (COL+1));
    }
    for(i=1;i<=ROW;i++)
    {
        for(j=1;j<=COL;j++)
    {
        gKernel_area[i][j]=0.0;
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
   
    max=0.0;
   
    for (i=1;i<=ROW;i++)
    {
        for(j=1;j<=COL;j++)
    {
        if(max < gKernel[i][j])
        {
           max=gKernel[i][j];
        }
    }
    }
   
    // normalize the Kernel
    for(i = 1; i <= ROW; ++i)
        for(j = 1; j <= COL; ++j)
            gKernel[i][j] /= max;
   
   
    //making gKernel_area which gives estimated pattern area
     for(i=1;i<=ROW;i++)
     {
         for(j=1;j<=COL;j++)
     {
         gKernel_area[i][j]=gKernel[i][j]*max_possible_pattern_area;
     }
     }
     
     for(int i=0;i<ROW;i++)
     {
		 for(int j=0;j<COL;j++)
		 {
			 kernel[ROW][COL] = gKernel_area[i][j];
		 }
	 }
     
     
    //printing in the file
     gfile=fopen("output/gaussfile.log","w");
     if(gfile==NULL)
     {
         printf("\ngauss file can not be opened\n");
     exit(0);
     }
     for(i=1;i<=ROW;i++)
     {
         for(j=1;j<=COL;j++)
     {
          fprintf(gfile,"%lf ",gKernel[i][j]);
     }
     fprintf(gfile,"\n");
     }
     fclose(gfile);
    
     gfile=fopen("output/gauss_areafile.log","w");
     if(gfile==NULL)
     {
         printf("\ngauss area file can not be opened\n");
     exit(0);
     }
     for(i=1;i<=ROW;i++)
     {
         for(j=1;j<=COL;j++)
     {
          fprintf(gfile,"%lf ",gKernel_area[i][j]);
     }
     fprintf(gfile,"\n");
     }
     fclose(gfile);
   
 
}

int main()
{
	int row,col;
	cout << "Row: ";
	cin >>row;
	cout << "Col: ";
	cin >>col;
	
	estimate_density_guass(row,col);
	
	
	
	return 0;
}
