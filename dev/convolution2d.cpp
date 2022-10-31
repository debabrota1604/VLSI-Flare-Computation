#include<iostream>
#include<fstream>
using namespace std;


int main()
{
	int x,y;
	cout << "Enter rows: ";
	cin >> x;
	cout << "Enter cols: ";
	cin >> y;
	
	int input[x][y];
	
	for(int c1 = 0; c1 < x; c1++)
	{
		for(int c2 = 0; c2 < y; c2++)
		{
			cout << "input[" << c1 << "][ " << c2 << " ]: ";
			cin >> input[c1][c2];
			//cout << input[c1][c2] <<endl;
		}
	}
	
	cout << "The input array is: " << endl;
	
	for(int c1=0; c1<x; c1++)
	{
		for(int c2=0; c2<y; c2++)
		{
			cout << input[c1][c2] << "\t";
		}
		cout << endl;
	}
	
	
	int kernel[5][5] = { { 1, 4, 6, 4, 1 },
						{ 4, 16, 24, 16, 4 },
						{ 6, 24, 36, 24, 6 },
						{ 4, 16, 24, 16, 4 },
						{ 1, 4, 6, 4, 1 } };
						
					
	
	cout << "The kernel is: " << endl;
						
	for(int c1=0;c1<5;c1++)
	{
		for(int c2=0;c2<5;c2++)
		{
			cout << kernel[c1][c2] << "\t";
		}
		cout << endl;
	}
	
	int sum = 256;
	int temp[x + 4][y + 4];
	float output[x][y];
	
	//initialization of input matrix with padding
	for(int c1 = 0; c1 < x + 4; c1++)
	{
		for(int c2 = 0; c2 < y + 4; c2++)
		{
			if(c1 < 2 || c1 >= x + 2)
			{
				temp[c1][c2] = 0;
			}
			else if(c2 < 2 || c2 >= y + 2)
			{
				temp[c1][c2] = 0;
			}
			else
			{
				temp[c1][c2] = input[c1 - 2][c2 - 2];
			}
		}
	}	
	
	cout << "The temporary array is: " << endl;
	
	for(int c1=0;c1<x + 4;c1++)
	{
		for(int c2=0;c2<y + 4;c2++)
		{
			cout << temp[c1][c2] << "\t";
		}
		cout << endl;
	}
	
	//applying convolution operation
	for(int c1 = 0; c1 < x; c1++)
	{
		for(int c2 = 0; c2 < y; c2++)
		{
			//here we can adress each location of the output array
			
			output[c1][c2] = 0;
			
			for(int c3 = 0;c3 < 5; c3++)
			{
				for(int c4=0; c4 <5 ; c4++)
				{
					//output[c1][c2] += temp[c1 - c3][c2 - c4] * kernel [c3][c4];
					output[c1][c2] += temp[c1+c3][c2+c4] * kernel[c3][c4];
					//cout << "output["<<c1<<"]["<<c2<<"] = temp["<<c1<<"+"<<c3<<"]["<<c2<<"+"<<c4<<"] * kernel["<<c3<<"]["<<c4<<"]\t";  
					cout << "output["<<c1<<"]["<<c2<<"] = temp["<<c1 + c3<<"]["<<c2 + c4<<"] * kernel["<<c3<<"]["<<c4<<"] = " << temp[c1+c3][c2+c4] * kernel[c3][c4] <<endl;  
				}
			}
			
			cout << "Final value: output["<<c1<<"]["<<c2<<"] = " << output[c1][c2] << endl;
			cout << endl;
		}
	}
	
	
	cout << "The output array is: " << endl;
	
	for(int c1=0; c1<x; c1++)
	{
		for(int c2=0; c2<y; c2++)
		{
			cout << output[c1][c2] << "\t";
		}
		cout << endl;
	}
	
	cout << "The normalised output array is: " << endl;
	
	for(int c1=0; c1<x; c1++)
	{
		for(int c2=0; c2<y; c2++)
		{
			cout << (output[c1][c2])/sum << "\t";
		}
		cout << endl;
	}
	ofstream out;
	out.open(convol.txt);
	for(int c1=0; c1<x; c1++)
	{
		for(int c2=0; c2<y; c2++)
		{
			cout << (output[c1][c2])/sum << "\t";
			out << (output[c1][c2])/sum << "   ";
		}
		cout << endl;
		out << endl;
	}
	out.close();
}
