#include<iostream>
#include<cmath>
using namespace std;

int main()
{


	int size;
	cout << "Set kernel size: ";
	cin >> size;
	int W = size;
	double sigma = 0.25 * size;
	double kernel[W][W];
	double mean = W/2;
	double sum = 0.0; // For accumulating the kernel values
	for (int x = 0; x < W; ++x) 
	{
		for (int y = 0; y < W; ++y) {
			kernel[x][y] =  (1 / sqrt(2 * M_PI * pow(sigma,2.0)) ) * exp(- (pow((x-mean)/sigma, 2.0) + pow((y-mean)/sigma,2.0)) / (2 * pow(sigma,2.0) ));

			// Accumulate the kernel values
			sum += kernel[x][y];
		}
	}
	
	cout << "The output array is: " << endl;
	
	for(int c1=0; c1<W; c1++)
	{
		for(int c2=0; c2<W; c2++)
		{
			cout << kernel[c1][c2] << "\t";
		}
		cout << endl;
	}
	
	
	// Normalize the kernel
	for (int x = 0; x < W; ++x) 
	{
		for (int y = 0; y < W; ++y)
		{
			kernel[x][y] /= sum;
		}
	}

	

	cout << "\n\nThe normalised output array is: " << endl;
	
	for(int c1=0; c1<W; c1++)
	{
		for(int c2=0; c2<W; c2++)
		{
			cout << (kernel[c1][c2]) << "\t";
		}
		cout << endl;
	}

	return 0;
}
