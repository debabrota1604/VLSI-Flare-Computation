#include "include.h"
extern ofstream out;

class Dimension
{
	int x,y,z;
	
	public:
	
	Dimension()
	{
		this->x = this->y = this->z = -1;
	}
	
	void setDimension(int i,int j,int k)
	{
		this->x = i;
		this->y = j;
		this->z = k;
	}
	
	void printDimension()
	{
		out<<"Dimension: (x,y,z) : " << "( "<< x << " , " << y << " , " << z << " ) " << endl;
	}
};
