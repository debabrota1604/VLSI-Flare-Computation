#include "include.h"
#include "Dimension.cpp"
extern FILE *filePtr;
extern ofstream out;

class Node
{
	int node_id,pin_id,global_id,global_parent_id;
	short node_type,parent_direction;
	float capacitance,delay,edge_delay,subtree_capacitance;
	
	
	public: 
	
	Dimension d;
	
	Node()
	{
		node_id = pin_id = global_id = global_parent_id = -1;
		
		node_type = parent_direction = -1;
		
		capacitance = delay = edge_delay = subtree_capacitance = -1;
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
	
	
	void printNode()
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
		d.printDimension();
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
	
};
