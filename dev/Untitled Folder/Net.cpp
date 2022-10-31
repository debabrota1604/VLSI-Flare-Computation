#include "include.h"
#include "Tree.cpp"
extern FILE *filePtr;
extern ofstream out;


typedef char NameStr[MaxNameStrLen];
typedef char InpStr[MaxInpStrLen];


class Net
{
	int id,tree_count;
	float r, c;
		
	int x_max, y_max, z_max, x_min, y_min, z_min;
	int is_bb_equal;//is bounding box of all trees are equal for this net?
	
	public:
	
	Tree *t;
	
	//net default constructor
	Net()
	{
		id = -1;
		tree_count = -1;
		r = -1;
		c = -1;
		//cannot initialize Tree here. Array information not available.
		is_bb_equal = -1;
		
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
		
		
		initialize_trees();
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
		if(tree_count>1 && (x_min == t[0].get_xmin() && x_max == t[0].get_xmax() && y_min == t[0].get_ymin() && y_max == t[0].get_ymax() && z_min == t[0].get_zmin() && z_max == t[0].get_zmax()))
		{
			is_bb_equal = 1;//flag set to true
			out<<"Bounding box of the net is equal to the bounding box of all the trees..."<<endl;
		}
		else if(tree_count > 1)//more than one tree but the values are not equal
		{
			is_bb_equal = 0;
			out << "Bounding box of this net is not equal to each tree. [(xMin,xMax), (yMin,yMax), (zMin,zMax)]: [("<<x_min<<","<<x_max<<"), ("<<y_min<<","<<y_max<<"), ("<<z_min<<","<<z_max<<")]" <<endl;
		}
		else//only one tree
		{
			is_bb_equal = 1;
			out << "Bounding box of this net is equal to the one available tree"<<endl;
		}
	}
	
	//update the net bounding box comparing each tree
	void update_net_bounding_box()
	{
		if(tree_count == 1)//initialize with the first tree
		{
			this->x_min = t[0].get_xmin();
			this->x_max = t[0].get_xmax();
			
			this->y_min = t[0].get_ymin();
			this->y_max = t[0].get_ymax();
			
			this->z_min = t[0].get_zmin();
			this->z_max = t[0].get_zmax();
		}
		else if(tree_count>1)
		{
			this->x_min = t[0].get_xmin();
			this->x_max = t[0].get_xmax();
			
			this->y_min = t[0].get_ymin();
			this->y_max = t[0].get_ymax();
			
			this->z_min = t[0].get_zmin();
			this->z_max = t[0].get_zmax();
			
			for(int i=1;i<tree_count;i++)//update the values from each tree
			{
				if(t[i].get_xmin() < this->x_min) this->x_min = t[i].get_xmin();
				if(t[i].get_xmax() > this->x_max) this->x_max = t[i].get_xmax();
				
				if(t[i].get_ymin() < this->y_min) this->y_min = t[i].get_ymin();
				if(t[i].get_ymax() > this->y_max) this->y_max = t[i].get_ymax();
				
				if(t[i].get_zmin() < this->z_min) this->z_min = t[i].get_zmin();
				if(t[i].get_zmax() > this->z_max) this->z_max = t[i].get_zmax();
			}
		}
	}
	
	//returns the bounding box flag
	int get_bb_flag()
	{
		return is_bb_equal;
	}
	
	int get_tree_count()
	{
		return tree_count;
	}
	
	//takes input for each tree from the input file and stores into the data structure
	void initialize_trees()
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
