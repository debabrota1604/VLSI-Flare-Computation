#include "include.h"
#include "Tree.cpp"
class One_tree_net
{
	int id,tree_count;
	float r, c;
	Tree *t;
		
	int x_max, y_max, z_max, x_min, y_min, z_min;
	
	public:
	
	//net default constructor
	Net()
	{
		id = -1;
		tree_count = 1;
		r = -1;
		c = -1;
		//cannot initialize Tree here. Array information not available.
		
	}
	
	//set the data related to a particular net
	void setNet(int id,int tree_count,float r,float c)
	{
		this->id = id;
		this->tree_count = tree_count;
		this->r = r;
		this->c = c; 
		try{
			t = new Tree[tree_count];//uses user-defined no parameter constructor to initialize the objects
		}catch(bad_alloc& e)
		{
			cerr << "Error in Net class while creating Trees: Allocation Failed! " << e.what() << endl;
		}
		
		
		initialize_the_tree();
	}
	
	//print the data related to each net
	void printNet()
	{
		char space[10],space2[10];
		strcpy(space,"    ");
		strcpy(space2,"  ");
		out << space;
		out << "####Hierarchy: Printing NET Under Info.class";
		
		
		out << space;
		out << "Net_ID: " << this->id << endl;
		out << space << space2;
		out << "Resistance: " << this->r << endl;
		out << space << space2;
		out << "Capacitance: " << this->c << endl;
		out << space << space2;
		out << "Number_of_Trees: " << this->tree_count << endl;
		update_net_bounding_box();
		out << space << space2;
		print_net_bounding_box();
		
		for(int counterN=0; counterN < tree_count; counterN++)
		{
			t[counterN].printTree();
		}
	}
	
	//print the bounding box of the net
	void print_net_bounding_box()
	{
		out << "Bounding box of this net is equal to the one available tree"<<endl;
		out << "Bounding box of this net is not equal to each tree. [(xMin,xMax), (yMin,yMax), (zMin,zMax)]: [("<<x_min<<","<<x_max<<"), ("<<y_min<<","<<y_max<<"), ("<<z_min<<","<<z_max<<")]" <<endl;
	}
	//takes input for each tree from the input file and stores into the data structure
	void initialize_the_tree()
	{
		InpStr inpStr1, inpStr2, inpStr3, inpStr4;
		NameStr namestr1, namestr2, namestr3, namestr4, namestr5, namestr6, namestr7, namestr8;
		short stmp, ctmp, l;
		int int1, int2, int3, int4, int5, int6, int7;
		float d1,d2;

		for(int counterN=0; counterN < tree_count; counterN++)
		{
			/* Tree ID  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %d", &namestr1, &int1);  /* Tree ID <*Tree= 0>*/
		
			/* NodeCount */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %d", &namestr1, &int2);  /* NodeCount <Number_of_nodes=  3>*/

			t[counterN].setTree(int1,int2);
		}	
	}	
};
