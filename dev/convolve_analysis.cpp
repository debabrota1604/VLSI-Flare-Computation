#include<iostream>
#include<fstream>
#include<math.h>
using namespace std;

#define readMatrixFile "convMat.txt"
#define readMatrixDimFile "convMatDim.txt"

int main()
{
	ifstream in;
	in.open(readMatrixDimFile);
	
	int row, cols;
	char c;
	in >> row;
	in >> c;
	in >> cols;
	cout << "Row: " <<row << " Col: " << cols <<endl;
	in.close();
	
	
	in.open(readMatrixFile);
	double matrix[row][cols];
	double min=999999999.99, max=0, avg=0, data = 0, total_sum= 0,sum_of_squares = 0,standard_deviation=0;
	for(int i=0;i<row;i++)
	{
		for(int j=0;j<cols;j++)
		{
			in >> data;
			matrix[i][j] = data;
			if(min > data) min = data;
			else if(max < data) max = data;
			total_sum += data;
			
			in >> c;//consuming the comma
		}
	}
	
	
	avg = total_sum / (row*cols);
	for(int i=0;i<row;i++)
	{
		for(int j=0;j<cols;j++)
		{
			sum_of_squares = pow((matrix[i][j] - avg),2);
		}
	}
	standard_deviation = sqrt(sum_of_squares/(row*cols));
	
	
	cout << "Max: " << max << endl;
	cout << "Min: " << min << endl;
	cout << "Avg: " << avg << endl;
	cout << "SD: " << standard_deviation << endl;
	
}
