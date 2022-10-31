#include "include.h"
#include<stdlib.h>
#include<math.h>
extern FILE *filePtr;
extern ofstream out;
extern int grid_x,grid_y,grid_z;

typedef char NameStr[MaxNameStrLen];
typedef char InpStr[MaxInpStrLen];

class Dimension
{
	int x,y,z;
	
	public:
	
	Dimension()
	{
		this->x = this->y = this->z = -1;
	}
	
	Dimension(const Dimension &temp)
	{
		this->x = temp.x;
		this->y = temp.y;
		this->z = temp.z;
	}
	
	void setDimension(int i,int j,int k)
	{
		this->x = i;
		this->y = j;
		this->z = k;
	}
	
	void printDimensionToFile()
	{
		out<<"Dimension: (x,y,z) : " << "( "<< x << " , " << y << " , " << z << " ) " << endl;
	}
	
	void printDimension()
	{
		cout<<"Dimension: (x,y,z) : " << "( "<< x << " , " << y << " , " << z << " ) " << endl;
	}
	
	int getX(){return x;}
	int getY(){return y;}
	int getZ(){return z;}
};

class Node
{
	int node_id,pin_id,global_id,global_parent_id;
	short node_type,parent_direction;
	float capacitance,delay,edge_delay,subtree_capacitance;
	
	
	public: 
	
	Dimension d;
	
	Node()//unparameterised constructor
	{
		node_id = pin_id = global_id = global_parent_id = -1;
		
		node_type = parent_direction = -1;
		
		capacitance = delay = edge_delay = subtree_capacitance = -1;
	}
	
	Node(const Node &temp)//copy constructor
	{
		this->node_id = temp.node_id;
		this->pin_id = temp.pin_id;
		this->global_id = temp.global_id;
		this->global_parent_id = temp.global_parent_id;
		this->node_type = temp.node_type;
		this->parent_direction = temp.parent_direction;
		this->capacitance = temp.capacitance;
		this->delay = temp.delay;
		this->edge_delay = temp.edge_delay;
		this->subtree_capacitance = temp.subtree_capacitance;
		this->d = temp.d;
	}
	
	
	void setNode(int id_node, int id_pin, int id_global, int id_parent, 
	short type_node, short direction_parent, int x, int y, int z, 
	double cap, double delay, double delay_edge, double subtree_cap)
	{
		node_id = id_node;
		pin_id = id_pin;
		global_id = id_global;
		global_parent_id = id_parent;
		node_type = type_node;
		parent_direction = direction_parent;

		d.setDimension(x,y,z);
		
		capacitance = cap;
		this->delay = delay;
		edge_delay = delay_edge;
		subtree_capacitance = subtree_cap;
	}
	
	
	void printNodeToFile()
	{
		char space[10],space2[10];
		strcpy(space,"        ");
		strcpy(space2,"  ");
		
		
		out << space;
		out << "####Hierarchy: Printing Node Under Tree.class" <<endl;
		
		out << space << space2;
		out << "Node_ID: " << node_id;
		out << space << space2;
		out << "PIN_ID: " << pin_id << endl;
		out << space << space2;
		out << "Global_ID: " << global_id << endl;
		out << space << space2;
		out << "Global_Parent_ID: " << global_parent_id << endl;
		
		out << space << space2;
		out << "Node_type: "<< node_type;
		switch(node_type)
		{
			case 0:		out << " ( Root ) " << endl;
				break;
			case 1:		out << " ( Sink ) " << endl;
				break;
			case 2:		out << " ( Intermediate ) " << endl;
				break;
			default:	out << " ( Undeclared ) " << endl;
				break;
		}
		out << space << space2;
		d.printDimensionToFile();
		out << space << space2;
		out << "Node_Capacitance: " << capacitance << endl;
		
		out << space << space2;
		out << "Direction_to_predecessor: " << parent_direction;
		switch(parent_direction)
		{
			case 1:		out << " ( WEST ) " << endl;
				break;
			case 2:		out << " ( NORTH ) " << endl;
				break;
			case 3:		out << " ( EAST ) " << endl;
				break;
			case 4:		out << " ( SOUTH ) " << endl;
				break;
			case 5:		out << " ( UP ) " << endl;
				break;
			case 6:		out << " ( DOWN ) " << endl;
				break;
			default:	out << " ( Undeclared ) " << endl;
				break;
		}
		
		out << space << space2;
		out << "Delay_to_the_Node: " << delay << endl;
		out << space << space2;
		out << "Edge_delay_with_end_in_the_node: " << edge_delay << endl;
		out << space << space2;
		out << "Capacitance_of_subtree_with_root_in_the_node: " << subtree_capacitance <<endl;
	}
	
	Dimension getDimension()
	{
		return this->d;
	}
	
	int getGlobalId()
	{
		return global_id;
	}
	
	int getGlobalParentId()
	{
		return global_parent_id;
	}
	
	float getEdgeDelay()
	{
		return edge_delay;
	}
	
};


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
	
	Tree(const Tree &temp) //copy constructor used in one tree net
	{
		
		//cout << "under Tree copy constructor\n";
		this->id = temp.id;
		this->node_count = temp.node_count;
		this->x_max = temp.x_max;
		this->y_max = temp.y_max;
		this->z_max = temp.z_max;
		this->x_min = temp.x_min;
		this->y_min = temp.y_min;
		this->z_min = temp.z_min;
		
		try{
			node = new Node[this->node_count];
		}catch(bad_alloc& e)
		{
			//cerr << "[Tree CC]received node_count: " << temp.node_count;
			cerr << "nodeCount: " << node_count << " , Error in Tree class copy-constructor: Node Allocation Failed! " << e.what() << endl;
		};
		
		for(int counter1 = 0; counter1 < this->node_count ; counter1++)
		{
			node[counter1] = temp.node[counter1];
		}
	}
	
	void setTree(int id, int nodeNum)
	{
		this->id = id;
		this->node_count = nodeNum;
		try{
			node = new Node[node_count];
		}catch(bad_alloc& e)
		{
			cerr << "Error in Tree class setTree method: Node Allocation Failed! " << e.what() << endl;
		};
		
		input_for_nodes();
	}	
	
	
	void printTreeToFile()
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
			node[tree_counter].printNodeToFile();
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
	
	int getNodeCount()
	{
		return node_count;
	}
	
	Node getNodeAtIndex(int index)
	{
		return node[index];
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
			sscanf (inpStr1, "%s %d", namestr1, &int1);  /* NodeID <*Node#  1 >*/
			
			/* PINID  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %d", namestr1, &int2);  /* PINID <*PinId  1>*/
			
			/* GlobalID  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %d", namestr1, &int3);  /* GlobalID <Global#  3989>*/
			
			/* Global PDSR ID  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %d", namestr1, &int4);  /* Global PDSR ID <Global#_of_node-predecessor  4053>*/
			
			/* NodeType  */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %hu", namestr1, &s1);  /* NodeType <Node_type(0-root,1-sink,2-intermediate)  1>*/
			
			/* Dimension */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %d %d %d", namestr1, &int5, &int6, &int7);  /* Dimension <(x,y,z):			  20   62   1>*/
			
			/* NodeCapacitance */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", namestr1, &f1);  /* NodeCapacitance <Node_capacitance=  0.700000>*/		
		
			/* NodeDirection */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %hu %s %d", namestr1, &s2, namestr2, &int4);  /* NodeDirection <direction_to_the_predecessor  2 pred_node 4053>*/		
		
			/* NodeDelay */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", namestr1, &f2);  /* NodeDelay <Delay_to_the_node  1.830710>*/
		
			/* NodeEdgeDelay */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", namestr1, &f3);  /* NodeEdgeDelay <Edge_delay_with_end_in_the_node  0.026160>*/
			
			/* NodeSubTreeCap */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", namestr1, &f4);  /* NodeSubTreeCap <Capacitance_of_subtree_with_root_in_the_node  1.400000>*/
			
			update_tree_boundary_box(int5,int6,int7);
			
			node[tree_counter].setNode(int1,int2,int3,int4,s1,s2,int5,int6,int7,f1,f2,f3,f4);
		}	
		
	}	
};


class One_tree_net
{//class equivalent to Net class. Only difference is that each net has only one tree
	
	public:
	int id, tree_count;
	float r, c;
	Tree t;
		
	One_tree_net()//default constructor called from min_delay_config class
	{
		tree_count=1;
		
	}	
	
	void setOTnet(int param_id, float param_r, float param_c, Tree param_t)
	{
		id = param_id;
		r = param_r;
		c = param_c;
		t = param_t;//uses copy constructor
	}
	
	Tree get_tree()
	{
		
		return t;
	}
};



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
	
	Net(const Net &temp)//copy constructor
	{
		this->id = temp.id;
		this->tree_count = temp.tree_count;
		this->r = temp.r;
		this->c = temp.c; 
		
		try{
			t = new Tree[tree_count];//uses user-defined no parameter constructor to initialize the objects
		}catch(bad_alloc& e)
		{
			cerr << "Error in Net class CopyConstructor while creating Trees: Allocation Failed! " << e.what() << endl;
		}
		
		//very crucial step!!!!!!!!
		for(int i=0;i < tree_count;i++)
		{
			this->t[i] = temp.t[i];
		}
		
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
			cerr << "Error in Net class Constructor while creating Trees: Allocation Failed! " << e.what() << endl;
		}
		
		
		initialize_trees();
	}
	
	//print the data related to each net
	void printNetToFile()
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
			t[counterN].printTreeToFile();
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
			sscanf (inpStr1, "%s %d", namestr1, &int1);  /* Tree ID <*Tree= 0>*/
		
			/* NodeCount */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %d", namestr1, &int2);  /* NodeCount <Number_of_nodes=  3>*/

			t[counterN].setTree(int1,int2);
		}	
	}	
	int get_id()
	{
		return id;
	}
	
	int get_r()
	{
		return r;
	}
	
	int get_c()
	{
		return c;
	}
	
	Tree getTreeByIndex(int index)//risky function
	{
		Tree tree;
		tree = t[index];
		return tree;
	}
};


class Delete_net
{
	Net *n;
	
	public:
	Delete_net()
	{
	}
	
	void set_net(Net *net)
	{
		n = net;
	}
	
	void delete_net()
	{
		//for each tree of the net
		int max1 = n->get_tree_count();
		for(int count1 = 0; count1 < max1; count1++)
		{//count1 is pointing to each tree
			
			//for each node of the tree
			int max2 = n->t[count1].getNodeCount();
			for(int count2 = 0; count2 < max2; count2++)
			{//count2 is pointing to each node
				
				delete &(n->t[count1].node[count2].d);
				delete &(n->t[count1].node[count2]);
			}
			//delete the tree after deleting its member dimension and all the member nodes
			delete &(n->t[count1]);
		}
		//delete the net itself after deleting its members
		delete &n;
		
	}
	
};


class Info
{
	Dimension d;
	int total_nets,max_trees,max_iter,max_qq,one_tree_nets;	
	
	public:
	Net *net;
	
	Info()
	{
		this->total_nets = 0;
		this->max_trees = 0;
		this->max_iter = 0;
		this->max_qq = 0;
		this->one_tree_nets = 0;
	}
	
	int get_total_nets()
	{
		return total_nets;
	}
	
	Dimension getDimension()
	{//return the dimension object
		
		return this->d;
	}
	
	void setInfo(int x,int y,int z,int netNum, int tree_max,int max_iteration,int max_qq)
	{
		d.setDimension(x,y,z);
		this->total_nets = netNum;
		this->max_trees = tree_max;
		this->max_iter = max_iteration;
		this->max_qq = max_qq;
		this->one_tree_nets = 0;
		
		try{			
			net  = new Net[total_nets];
		}catch(bad_alloc& e)
		{
			cerr << "Error in Info class while creating Nets: Allocation Failed! " << e.what() << endl;
		}
		
		initialize_nets();
	}
	
	void printInfoToFile()
	{
		int bb_flag = 1;
		char space[10],space2[10];
		strcpy(space,"  ");
		strcpy(space2,"  ");
		out << space;
		out << "####Hierarchy: Info Class. Input File ";	
		d.printDimensionToFile();
		
		out << space << space2;
		out << "Total number of nets: " << total_nets << endl;
		out << space << space2;
		out << "Maximum trees per net: " << max_trees << endl;
		out << space << space2;
		out << "Maximum iterations: " << max_iter << endl;
		out << space << space2;
		out << "Max qq: " << max_qq << endl;
		
		for(int info_count=0; info_count<total_nets; info_count++)
		{
			net[info_count].printNetToFile();
			bb_flag *= net[info_count].get_bb_flag();
		}
		
		
		cout << "# of one-tree-Nets: "<< one_tree_nets << " out of " << total_nets << " nets."<<endl;
		if(bb_flag ==1)
		{
			cout<<"All the trees under a multi-tree net have the same bounding box."<<endl;
		}
		else
		{
			cout<<"Bounding boxes are different for each tree in a multi-tree net." <<endl;
		}
	}
	
	void printInfo()
	{
		int bb_flag = 1;
		cout << "Total number of nets: " << total_nets << endl;
		cout << "Maximum trees per net: " << max_trees << endl;
		cout << "Maximum iterations: " << max_iter << endl;
		cout << "Max qq: " << max_qq << endl;
		d.printDimension();
		
		
		cout << "# of one-tree-Nets: "<< one_tree_nets << " out of " << total_nets << " nets."<<endl;
		if(bb_flag ==1)
		{
			cout<<"All the trees under a multi-tree net have the same bounding box."<<endl;
		}
		else
		{
			cout<<"Bounding boxes are different for each tree in a multi-tree net." <<endl;
		}
	}
	
	void initialize_nets()
	{
		
		InpStr inpStr1, inpStr2, inpStr3, inpStr4;
		NameStr namestr1, namestr2, namestr3, namestr4, namestr5, namestr6, namestr7, namestr8;
		short stmp, ctmp, l;
		int int1, int2, int3, int4, int5, int6, int7;
		float d1,d2;
		
		for(int info_count=0; info_count<total_nets; info_count++)
		{		
			/* NET ID */
			fgets (inpStr1, MaxInpStrLen, filePtr);
			sscanf (inpStr1, "%s %d", namestr1, &int1);  /* NET ID <*Net_Id= 0>*/
		
			/* Drive Resistance */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", namestr1, &d1);  /* Resistance <drive(r_0)=  0.965000>*/
		
			/* Drive Capacitance */
			fgets (inpStr3, MaxInpStrLen, filePtr);
			sscanf (inpStr3, "%s  %f", namestr1, &d2);  /* Capacitance <C_0=  1.870000>*/
		
			/* Tree Count */
			fgets (inpStr4, MaxInpStrLen, filePtr);
			sscanf (inpStr4, "%s  %d", namestr1, &int2);  /* Tree Count <Number_of_trees  1>*/
			
		
			if(int2 > 1) //major correction if more than one tree is present for a NET.
			{
				int2++;
			}
			else
			{
				one_tree_nets++;
			}
		
			net[info_count].setNet(int1,int2,d1,d2);
			
		}
		
	}
	
	Net get_netByIndex(int index)
	{
		return net[index];
	}
	
};


class Min_delay_config
{//class equivalent to Info class. Only difference is that each net has only one tree
	
	int total_nets;//stores the number of nets in this configuration
	Dimension d;
	
	public:
	One_tree_net *ot_net;//stores the address of each one tree nets
	
	Min_delay_config(){total_nets=0;}
	
	void set_min_delay_config(Info object){
		//select the 0th tree from all the nets which represents minimum delay nets.
		
		total_nets = object.get_total_nets();
		this->d = object.getDimension();
		
		//allocate memory for each One_tree_net
		try{
			ot_net  = new One_tree_net[total_nets];
		}
		catch(bad_alloc& e)
		{
			cerr << "Error in Min_delay_config class while creating ot_Nets: Allocation Failed! " << e.what() << endl;
		}
		
		//take a tree of index zero and initialize the tree
		for(int i=0;i<total_nets;i++)
		{			
			ot_net[i].setOTnet(object.net[i].get_id(), object.net[i].get_r(), object.net[i].get_c(), object.net[i].getTreeByIndex(0));
		}
	}
	
	Dimension getDimension()
	{
		return this->d;
	}
	
	int getMinDelayTotalNets()
	{
		return total_nets;
	}
	int get_nodes_ofTree_atIndex(int index)
	{//give the number of nodes present in the min_delay tree of net <index>.
		//Remember,here net contains only one tree.
		
		return ot_net[index].t.getNodeCount();
	}
};

class Min_centre_distance_config
{//class equivalent to Info class. Only difference is that each net has only one tree
	
	int total_nets;//stores the number of nets in this configuration
	Dimension d;
	
	public:
	One_tree_net *ot_net;//stores the address of each one tree nets
	
	Min_centre_distance_config(){total_nets=0;}
	
	void set_min_cd_config(Info object)
	{
		//select the 0th tree from all the nets which represents minimum delay nets.
		
		total_nets = object.get_total_nets();
		this->d = object.getDimension();
		
		//allocate memory for each One_tree_net
		try{
			ot_net  = new One_tree_net[total_nets];
		}
		catch(bad_alloc& e)
		{
			cerr << "Error in Min_delay_config class while creating ot_Nets: Allocation Failed! " << e.what() << endl;
		}
		int centre_x = grid_x/2,centre_y = grid_y/2;
		
		
		//take a tree of minimum edge distance from centre of region and initialize the tree
		for(int i=0;i<total_nets;i++)//for each net
		{
			double distance = 9999.99, dist = 0.0;
			int min_distance_index = 0,id,treeInNet;
			float r,c;
			Tree t,tempTree;
			Net net = object.get_netByIndex(i);
			//initialize the net parameters
			id = net.get_id(); r = net.get_r(); c = net.get_c();
						
			//select the desired tree with minimum edge distance from centre
			//as all the trees under the same net seems to have the same bounding box,
			//we calculate the sum of edge-distance for each tree of the selected net
			//then we choose the tree with minimum centre distance
			for(int c1=0;c1<net.get_tree_count();c1++)//for each tree
			{			
				//cout << "\tTree " << c1 +1 << " with " << net.getTreeByIndex(c1).getNodeCount() << " nodes..." <<endl;				
				tempTree = net.getTreeByIndex(c1);
				dist = 0.0;
				for(int c2 = 0; c2 < tempTree.getNodeCount(); c2++)//for each node of the tree
				{//calculate the total distance
					Node node = tempTree.getNodeAtIndex(c2);
					int id,id2,pid;
					id = node.getGlobalId();
					pid = node.getGlobalParentId();
					//cout << "node "<< id << " has parent id " <<pid <<endl;
					if(id == pid)
					{
						//this is the root node
						//no distance can be computed for this node
					}
					else
					{//search the rest of the nodes for finding its parent
						for(int c3 = 0; c3 <tempTree.getNodeCount(); c3++)//for each neighbour of the node c2
						{
							if(c3 == c2) continue;
							Node node2 = tempTree.getNodeAtIndex(c3);
							id2 = node2.getGlobalId();
							if(pid == id2)
							{
								//this is the parent node
								//calculate the edge distance and then
								//calculate the distance from the centre.								
								Dimension d = node.getDimension();
								Dimension d2 = node2.getDimension();
								if(d.getZ() == d2.getZ())
								{//both nodes are in the same layer. So we can calculate the distance
									
									//formula for calculating distance from a point (x0,y0) to a straight line connecting two points P1 & P2.
									//distance(p1,p2,(x0,y0)) = abs((y2-y1)x0 - (x2-x1)y0 + x2y1 - y2x1) / sqrt(pow((y2-y1),2) + pow((x2-x1),2))
									
									int x0 = grid_x/2 ,x1 = d.getX() ,x2 = d2.getX() ,y0 = grid_y/2, y1 = d.getY(), y2 = d2.getY();
									
									double length= fabs((y2 - y1)*x0 - (x2 - x1)*y0 + (x2 * y1) - (y2 * x1)) / sqrt(pow(y2 - y1 , 2) + pow(x2 - x1 , 2));
									dist+=length;
								}
								//else, the edge is a via, so does not contribute to flare.								
								break;
							}
							//else check the rest of the nodes
						}
					}
				}
				//if the computed distance is less than the last distance, then update the lowest distance index
				
				//cout << "Comparing " << dist << " with " << distance << endl;
				if(dist < distance)
				{
					distance = dist;
					min_distance_index = c1;
					//cout << "Modified index: " << c1 <<endl;
				}
			}
			//cout << "Min_distance index for net " << i << " is " << min_distance_index << endl;
			
			t = net.getTreeByIndex(min_distance_index);
			//call the set function of ot_net
			ot_net[i].setOTnet(id,r,c,t);
			//ot_net[i].setOTnet(object.net[i].get_id(), object.net[i].get_r(), object.net[i].get_c(), object.net[i].getTreeByIndex(0));
		}
	}
	
	Dimension getDimension()
	{
		return this->d;
	}
	
	int getMinDelayTotalNets()
	{
		return total_nets;
	}
	int get_nodes_ofTree_atIndex(int index)
	{//give the number of nodes present in the min_distance tree of net <index>.
		//Remember,here net contains only one tree.
		
		return ot_net[index].t.getNodeCount();
	}
};

