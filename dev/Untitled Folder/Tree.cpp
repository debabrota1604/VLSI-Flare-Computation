#include "include.h"
#include "Node.cpp"
extern FILE *filePtr;
extern ofstream out;


typedef char NameStr[MaxNameStrLen];
typedef char InpStr[MaxInpStrLen];



class Tree
{
	int id, node_count;
	int x_max, y_max, z_max, x_min, y_min, z_min;//for storing the boundary
	
	public:
	
	Node *node;
	Tree()
	{		//no-parameter constructor 
		id = -1;
		node_count = -1;
		
		//settig the max to zero
		x_max = y_max = z_max = 0;
		
		//setting the min to some big value
		x_min = y_min = z_min = MaxNum;
	}
	
	void setTree(int id, int nodeNum)
	{
		this->id = id;
		this->node_count = nodeNum;
		try{
			node = new Node[node_count];
		}catch(bad_alloc& e)
		{
			cerr << "Error in Tree class while creating Nodes: Allocation Failed! " << e.what() << endl;
		};
		
		input_for_nodes();
	}	
	
	
	void printTree()
	{
		char space[10],space2[10];
		strcpy(space,"      ");
		strcpy(space2,"  ");
		out << space;
		out << "####Hierarchy: Printing Tree under Net.class" << space;
		
		out << "Tree_ID: " << id << endl;
		out << space << space2;
		out << "Number_of_Nodes: " << node_count << endl;
		out << space << space2;
		print_tree_bounding_box();
		
		for(int tree_counter= 0; tree_counter < node_count ; tree_counter++)
		{
			node[tree_counter].printNode();
		}
	}
	
	void update_tree_boundary_box(int int5, int int6, int int7)
	{
			if( int5 > x_max)
			{
				x_max = int5;
			}
			if( int5 < x_min)
			{
				x_min = int5;
			}
			
			if( int6 > y_max)
			{
				y_max = int6;
			}
			if( int6 < y_min)
			{
				y_min = int6;
			}
			
			if( int7 > z_max)
			{
				z_max = int7;
			}
			if( int7 < z_min)
			{
				z_min = int7;
			}		
	}
	
	void print_tree_bounding_box()
	{
		out << "Bounding box [(xMin,xMax), (yMin,yMax), (zMin,zMax)]: [("<<x_min<<","<<x_max<<"), ("<<y_min<<","<<y_max<<"), ("<<z_min<<","<<z_max<<")]" <<endl;
	}
	
	int get_xmin()
	{
		return x_min;
	}
	int get_xmax()
	{
		return x_max;
	}
	
	int get_ymin()
	{
		return y_min;
	}
	int get_ymax()
	{
		return y_max;
	}
	
	int get_zmin()
	{
		return z_min;
	}
	int get_zmax()
	{
		return z_max;
	}
	
	int get_node_count()
	{
		return node_count;
	}
	
	
	void input_for_nodes()
	{
		
		InpStr inpStr1, inpStr2;
		NameStr namestr1, namestr2;
		short s1=0,s2=0;
		int int1=0, int2=0, int3=0, int4=0, int5=0, int6=0, int7=0;
		float f1=0,f2=0,f3=0,f4=0;
		
		
		
		for(int tree_counter= 0; tree_counter < node_count ; tree_counter++)
		{
			/* NodeID  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %d", &namestr1, &int1);  /* NodeID <*Node#  1 >*/
			
			/* PINID  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %d", &namestr1, &int2);  /* PINID <*PinId  1>*/
			
			/* GlobalID  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %d", &namestr1, &int3);  /* GlobalID <Global#  3989>*/
			
			/* Global PDSR ID  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %d", &namestr1, &int4);  /* Global PDSR ID <Global#_of_node-predecessor  4053>*/
			
			/* NodeType  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %hu", &namestr1, &s1);  /* NodeType <Node_type(0-root,1-sink,2-intermediate)  1>*/
			
			/* Dimension */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %d %d %d", &namestr1, &int5, &int6, &int7);  /* Dimension <(x,y,z):			  20   62   1>*/
			
			/* NodeCapacitance */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", &namestr1, &f1);  /* NodeCapacitance <Node_capacitance=  0.700000>*/		
		
			/* NodeDirection */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %hu %s %d", &namestr1, &s2, &namestr2, &int4);  /* NodeDirection <direction_to_the_predecessor  2 pred_node 4053>*/		
		
			/* NodeDelay */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", &namestr1, &f2);  /* NodeDelay <Delay_to_the_node  1.830710>*/
		
			/* NodeEdgeDelay */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", &namestr1, &f3);  /* NodeEdgeDelay <Edge_delay_with_end_in_the_node  0.026160>*/
			
			/* NodeSubTreeCap */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", &namestr1, &f4);  /* NodeSubTreeCap <Capacitance_of_subtree_with_root_in_the_node  1.400000>*/
			
			update_tree_boundary_box(int5,int6,int7);
			
			node[tree_counter].setNode(int1,int2,int3,int4,s1,s2,int5,int6,int7,f1,f2,f3,f4);
		}	
		
	}
	
	
	
};
