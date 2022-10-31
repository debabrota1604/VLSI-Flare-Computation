#include "include.h"
#include "Net.cpp"
extern FILE *filePtr;
extern ofstream out;

typedef char NameStr[MaxNameStrLen];
typedef char InpStr[MaxInpStrLen];

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
	
	void printInfo()
	{
		int bb_flag = 1;
		char space[10],space2[10];
		strcpy(space,"  ");
		strcpy(space2,"  ");
		out << space;
		out << "####Hierarchy: Info Class. Input File ";	
		d.printDimension();
		
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
			net[info_count].printNet();
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
			sscanf (inpStr1, "%s %d", &namestr1, &int1);  /* NET ID <*Net_Id= 0>*/
		
			/* Drive Resistance */
			fgets (inpStr2, MaxInpStrLen, filePtr);
			sscanf (inpStr2, "%s  %f", &namestr1, &d1);  /* Resistance <drive(r_0)=  0.965000>*/
		
			/* Drive Capacitance */
			fgets (inpStr3, MaxInpStrLen, filePtr);
			sscanf (inpStr3, "%s  %f", &namestr1, &d2);  /* Capacitance <C_0=  1.870000>*/
		
			/* Tree Count */
			fgets (inpStr4, MaxInpStrLen, filePtr);
			sscanf (inpStr4, "%s  %d", &namestr1, &int2);  /* Tree Count <Number_of_trees  1>*/
			
		
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
			int max2 = n->t[count1].get_node_count();
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
