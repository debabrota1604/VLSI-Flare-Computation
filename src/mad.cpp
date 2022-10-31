#include "routing.h"
#include "extern.h"

int *Tree_pred;			//Tree_pred[j] is the predecessor node for j

// this procedure constructs one timing-driven congestion-aware Steiner tree
void
MAD(int s, TNET * CurNet, float r_0, int N, int N_All, TNODE * Node_All,
    int *PMinResource)
{
  int u, v, v1, v2, i, j, k, l, Sinks_in_Tree, dist;
  int Min_res, L, lev;
  short lay, layer_u, layer_v;
  short Pred_dir;
  float Min_arc, cap, res, delay, C_0;
  int MinResource, number, Num_nodes;
  short *In_Tree;		//If In_Tree[i]==1, node i is in current tree
  short *In_Tree_tmp;		//If In_Tree[i]==1, node i is temporary in current tree
  float *t;			//t[i] = delay in node i
  float *R;			//R[i] = \sum\limits_{e\in P_i} r_e
  float *C;			//C[i] = capacitance of subtree T_i
  float *del;			//del[j]=d_{pred[j],j}
  int *Pred;			//Pred[j] = node - predecessor of j
  int *level;			//level[i] = number of edges in path from root to i
  short *Up_Down;		//Up_Down[i]=up if i\in upper level; Up_Down[i]=down if i\in lower level
  int *degree;			//degree[i]=number of children of node i

  // Allocate all the require memory, we can have a local structure to 
  // handle this
  In_Tree = (short *) calloc(N_All + 1, sizeof(short));
  In_Tree_tmp = (short *) calloc(N_All + 1, sizeof(short));
  t = (float *) calloc(N_All + 1, sizeof(float));
  R = (float *) calloc(N_All + 1, sizeof(float));
  C = (float *) calloc(N_All + 1, sizeof(float));
  del = (float *) calloc(N_All + 1, sizeof(float));
  Pred = (int *) calloc(N_All + 1, sizeof(int));
  level = (int *) calloc(N_All + 1, sizeof(int));
  Up_Down = (short *) calloc(N_All + 1, sizeof(short));
  degree = (int *) calloc(N_All + 1, sizeof(int));

  //----------------------------------------------------------------------
  //use only global edges with current resource >= MinResource:
  MinResource = *PMinResource;
  MinResource++;

  do {
    MinResource--;
#ifndef NDEBUG
    printf ("****** Processing net %d @ %d resource *******\n", s, MinResource);
#endif
    L = 0;
    Sinks_in_Tree = 0;		//total number of sinks in current tree
    for (i = 1; i <= N_All; i++) {
      t[i] = 0;			//initial delay in node i
      R[i] = 0;			//initial sum of resistances in path to i
      In_Tree[i] = 0;
      In_Tree_tmp[i] = 0;
      Pred[i] = -1;
      level[i] = -1;
      degree[i] = 0;
    }

    for (i = 1; i <= N_All; i++) {
      if (Node_All[i].type == 0) {
	In_Tree[i] = 1;		//include a root node
	In_Tree_tmp[i] = 1;	//in the current tree
	Pred[i] = i;
	level[i] = 0;

	//if there are other sinks in the root:
	for (k = 1; k <= N; k++) {
	  if (Node_All[i].x == CurNet->Sink[k].x
	      && Node_All[i].y == CurNet->Sink[k].y
	      && Node_All[i].z == CurNet->Sink[k].z)
	    Sinks_in_Tree++;
	}
      }
    }

    //Tree construction for fixed MinResource:
    do {
      Min_arc = MaxFloat;
      i = -1;
      j = -1;
      for (u = 1; u <= N_All; u++) {
	if (In_Tree_tmp[u] == 1)	//for all nodes already included in T
	{
          // if west resource is free (not in T)
	  if (Node_All[u].w > 0 && In_Tree_tmp[Node_All[u].w] == 0)
	  {
	    v = Node_All[u].w;	//neighbor in the west
	    Min_res = MaxInt;
	    v1 = Node_All[u].No;
	    v2 = Node_All[v].No;

	    for (k = v2; k < v1; k++) {
              // printf ("Global %d - w cap %d\n", k, GlobalNode[k].EastResource);
	      if (GlobalNode[k].EastResource < Min_res)
		Min_res = GlobalNode[k].EastResource;
            }
               

	    if (Min_res >= MinResource) {
              /* printf ("WE %d - %d Min Target %d, found %d\n", u, 
                         Node_All[u].w, MinResource, Min_res);
              */
	      lay = Node_All[u].z;
	      dist = Node_All[u].x - Node_All[v].x;
	      cap = (Layer[lay].cap) * dist;
	      res = (Layer[lay].res) * dist;
	      delay = res * (cap / 2 + Node_All[v].cap);

	      if (t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay <
		  Min_arc) {
		Min_arc =
		    t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay;
		i = u;
		j = v;
	      }
	    }
	  }			//end west

	  //------------------------- north --------------------------------
	  if (Node_All[u].n > 0 && In_Tree_tmp[Node_All[u].n] == 0) {
	    v = Node_All[u].n;
	    Min_res = MaxInt;
	    v1 = Node_All[u].No;
	    v2 = Node_All[v].No;
	    k = v1;

	    do {
	      if (GlobalNode[k].NorthResource < Min_res)
		Min_res = GlobalNode[k].NorthResource;
	      k = k + Size_X + 1;
	    } while (k < v2);

	    if (Min_res >= MinResource) {
	      lay = Node_All[u].z;
	      dist = Node_All[v].y - Node_All[u].y;
	      cap = Layer[lay].cap * dist;
	      res = Layer[lay].res * dist;
	      delay = res * (cap / 2 + Node_All[v].cap);
	      if (t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay <
		  Min_arc) {
		Min_arc =
		    t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay;
		i = u;
		j = v;
	      }
	    }
	  }			//end north

	  //--------------------- east -------------------------------------
	  if (Node_All[u].e > 0 && In_Tree_tmp[Node_All[u].e] == 0) {
	    v = Node_All[u].e;
	    Min_res = MaxInt;
	    v1 = Node_All[u].No;
	    v2 = Node_All[v].No;

	    for (k = v1; k < v2; k++)
	      if (GlobalNode[k].EastResource < Min_res)
		Min_res = GlobalNode[k].EastResource;

	    if (Min_res >= MinResource) {
              /* printf ("WE %d - %d Min Target %d, found %d\n", u, 
                         Node_All[u].e, MinResource, Min_res);
               */
	      lay = Node_All[u].z;
	      dist = Node_All[v].x - Node_All[u].x;
	      cap = Layer[lay].cap * dist;
	      res = Layer[lay].res * dist;
	      delay = res * (cap / 2 + Node_All[v].cap);
	      if (t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay <
		  Min_arc) {
		Min_arc =
		    t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay;
		i = u;
		j = v;
	      }
	    }
	  }

	  //------------------- South ---------------------------------------
	  if (Node_All[u].s > 0 && In_Tree_tmp[Node_All[u].s] == 0) {
	    v = Node_All[u].s;
	    Min_res = MaxInt;
	    v1 = Node_All[u].No;
	    v2 = Node_All[v].No;
	    k = v2;

	    do {
	      if (GlobalNode[k].NorthResource < Min_res)
		Min_res = GlobalNode[k].NorthResource;
	      k = k + Size_X + 1;
	    } while (k < v1);

	    if (Min_res >= MinResource) {
	      lay = Node_All[u].z;
	      dist = Node_All[u].y - Node_All[v].y;
	      cap = Layer[lay].cap * dist;
	      res = Layer[lay].res * dist;
	      delay = res * (cap / 2 + Node_All[v].cap);
	      if (t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay <
		  Min_arc) {
		Min_arc =
		    t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay;
		i = u;
		j = v;
	      }
	    }
	  }			//end south

	  //-------------------  up  --------------------------------------
	  if (Node_All[u].u > 0 && In_Tree_tmp[Node_All[u].u] == 0) {
	    v = Node_All[u].u;
	    Min_res = MaxInt;
	    v1 = Node_All[u].No;
	    v2 = Node_All[v].No;
	    k = v1;

	    do {
	      if (GlobalNode[k].UpResource < Min_res)
		Min_res = GlobalNode[k].UpResource;
	      k = k + (Size_X + 1) * (Size_Y + 1);
	    } while (k < v2);

	    if (Min_res >= MinResource) {
	      layer_u = Node_All[u].z;
	      layer_v = Node_All[v].z;
	      cap = 0.0;
	      res = 0.0;
	      for (lay = layer_u; lay < layer_v; lay++) {
		cap = cap + Layer[lay].up_cap;
		res = res + Layer[lay].up_res;
	      }
	      delay = res * (cap / 2 + Node_All[v].cap);
	      if (t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay <
		  Min_arc) {
		Min_arc =
		    t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay;
		i = u;
		j = v;
	      }
	    }
	  }			//end up

	  //------------------- down ---------------------------------------
	  if (Node_All[u].d > 0 && In_Tree_tmp[Node_All[u].d] == 0) {
	    v = Node_All[u].d;
	    Min_res = MaxInt;
	    v1 = Node_All[u].No;
	    v2 = Node_All[v].No;
	    k = v2;

	    do {
	      if (GlobalNode[k].UpResource < Min_res)
		Min_res = GlobalNode[k].UpResource;
	      k = k + (Size_X + 1) * (Size_Y + 1);
	    } while (k < v1);

	    if (Min_res >= MinResource) {
	      layer_u = Node_All[u].z;
	      layer_v = Node_All[v].z;
	      cap = 0.0;
	      res = 0.0;

	      for (lay = layer_v; lay < layer_u; lay++) {
		cap = cap + Layer[lay].up_cap;
		res = res + Layer[lay].up_res;
	      }

	      delay = res * (cap / 2 + Node_All[v].cap);
	      if (t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay <
		  Min_arc) {
		Min_arc =
		    t[u] + (r_0 + R[u]) * (cap + Node_All[v].cap) + delay;
		i = u;
		j = v;
	      }
	    }
	  }			//end down
	}			//end for u
      }


      //=====================================================================
      if (i > 0)		//arc (i,j), to be included in T, is found
      {
	Pred[j] = i;
	level[j] = level[i] + 1;
	if (Node_All[i].x < Node_All[j].x)
	  Pred_dir = west;
	if (Node_All[i].x > Node_All[j].x)
	  Pred_dir = east;
	if (Node_All[i].y > Node_All[j].y)
	  Pred_dir = north;
	if (Node_All[i].y < Node_All[j].y)
	  Pred_dir = south;
	if (Node_All[i].z > Node_All[j].z)
	  Pred_dir = up;
	if (Node_All[i].z < Node_All[j].z)
	  Pred_dir = down;
	Node_All[j].pred_dir = Pred_dir;

        //recompute R[j]
	switch (Pred_dir) {
	case west:
	  R[j] =
	      R[i] + Layer[Node_All[i].z].res * (Node_All[j].x -
						 Node_All[i].x);
	  cap = Layer[Node_All[i].z].cap * (Node_All[j].x - Node_All[i].x);
	  break;
	case north:
	  R[j] =
	      R[i] + Layer[Node_All[i].z].res * (Node_All[i].y -
						 Node_All[j].y);
	  cap = Layer[Node_All[i].z].cap * (Node_All[i].y - Node_All[j].y);
	  break;
	case east:
	  R[j] =
	      R[i] + Layer[Node_All[i].z].res * (Node_All[i].x -
						 Node_All[j].x);
	  cap = Layer[Node_All[i].z].cap * (Node_All[i].x - Node_All[j].x);
	  break;
	case south:
	  R[j] =
	      R[i] + Layer[Node_All[i].z].res * (Node_All[j].y -
						 Node_All[i].y);
	  cap = Layer[Node_All[i].z].cap * (Node_All[j].y - Node_All[i].y);
	  break;
	case up:
	  R[j] = R[i];
	  cap = 0;
	  for (lay = Node_All[j].z; lay < Node_All[i].z; lay++) {
	    R[j] = R[j] + Layer[lay].up_res;
	    cap = cap + Layer[lay].up_cap;
	  }
	  break;
	case down:
	  R[j] = R[i];
	  cap = 0;
	  for (lay = Node_All[i].z; lay < Node_All[j].z; lay++) {
	    R[j] = R[j] + Layer[lay].up_res;
	    cap = cap + Layer[lay].up_cap;
	  }
	  break;
	}			//switch

        /*
        printf ("New edge %d to %d, R %f, C %f, C[v] %f\n",
                Node_All[i].No, Node_All[j].No, res, cap, Node_All[j].cap);
        */
               
	//---------------------------------------------------------------------
	//recompute all t[v1], v1 in T (taking into account that there are leaf paths)
	t[j] = Min_arc;
	if (Node_All[j].type == 1)	//sink is added
	{			//if there are other sinks in the node:
	  for (k = 1; k <= N; k++)
	    if (Node_All[j].x == CurNet->Sink[k].x
		&& Node_All[j].y == CurNet->Sink[k].y
		&& Node_All[j].z == CurNet->Sink[k].z)
	      Sinks_in_Tree++;
	  In_Tree[j] = 1;
	  v2 = j;

	  do {
	    v1 = Pred[v2];
	    In_Tree[v1] = 1;
	    v2 = v1;
	  } while (In_Tree[Pred[v2]] == 0);	//all nodes in path to j are in tree
	  L = 0;
	  for (v = 1; v <= N_All; v++) {
	    if (In_Tree[v] == 1)
	      In_Tree_tmp[v] = 1;

	    else {
	      In_Tree_tmp[v] = 0;
	      level[v] = -1;
	    }
	    if (L < level[v])
	      L = level[v];	//L - number of levels in tree
	  }			//end for v
	  for (v = 1; v <= N_All; v++) {
	    if (In_Tree[v] != 1) {
	      Pred[v] = -1;
	      C[v] = 0;
	    }
	    else
              // C[v] - total capacitance of subtree T_v of subtree T_v 
	      C[v] = Node_All[v].cap;
          }

          //calculate C_j and d_{ij}. From leafs to root
	  for (lev = L; lev > 0; lev--)
	    for (v = 1; v <= N_All; v++)
	      if (level[v] == lev) {
		u = Pred[v];
		C[u] = C[u] + C[v];
		Pred_dir = Node_All[v].pred_dir;
		switch (Pred_dir) {
		case west:
		  res = Layer[Node_All[u].z].res * 
                            (Node_All[v].x - Node_All[u].x);
		  cap = Layer[Node_All[u].z].cap * 
                            (Node_All[v].x - Node_All[u].x);
		  break;
		case north:
		  res = Layer[Node_All[u].z].res * 
                            (Node_All[u].y - Node_All[v].y);
		  cap = Layer[Node_All[u].z].cap * 
                            (Node_All[u].y - Node_All[v].y);
		  break;
		case east:
		  res = Layer[Node_All[u].z].res * 
                            (Node_All[u].x - Node_All[v].x);
		  cap = Layer[Node_All[u].z].cap * 
                            (Node_All[u].x - Node_All[v].x);
		  break;
		case south:
		  res = Layer[Node_All[u].z].res * 
                            (Node_All[v].y - Node_All[u].y);
		  cap = Layer[Node_All[u].z].cap * 
                            (Node_All[v].y - Node_All[u].y);
		  break;
		case up:
		  res = 0;
		  cap = 0;
		  for (lay = Node_All[v].z; lay < Node_All[u].z; lay++) {
		    res = res + Layer[lay].up_res;
		    cap = cap + Layer[lay].up_cap;
		  }
		  break;
		case down:
		  res = 0;
		  cap = 0;
		  for (lay = Node_All[u].z; lay < Node_All[v].z; lay++) {
		    res = res + Layer[lay].up_res;
		    cap = cap + Layer[lay].up_cap;
		  }
		  break;
		}		//end switch

		C[u] = C[u] + cap;
               	//del[v]=d_{uv}, u=pred[v]
		del[v] = res * (cap / 2 + C[v]);
		C_0 = C[u];
	      }			//end from leafs to root

          //from root to leafs. Calculate t_k
	  for (v = 1; v <= N_All; v++) {
	    if (level[v] > 0)
	      t[v] = 0;
	    else if (level[v] == 0)
	      t[v] = C_0 * r_0;
	  }
	  for (lev = 1; lev <= L; lev++) {
	    for (v = 1; v <= N_All; v++) {
	      if (level[v] == lev) {
		u = Pred[v];
		t[v] = t[u] + del[v];
	      }
	    }
	  }
	}			//end if sink is added
	else
	  In_Tree_tmp[j] = 1;	//not sink is added
      }				//end if(i>0)
    } while (Sinks_in_Tree < N && i > 0);	//do2
  } while (i < 0);		//do1

  // printf ("Completed MAD, found %d sinks out of %d sinks\n", Sinks_in_Tree, N);

  for (k = 1; k <= N_All; k++) {
    Node_All[k].delay = t[k];
    Node_All[k].SubtreeCap = C[k];
    Node_All[k].EdgeDelay = del[k];
  }

  //------------------------------------------------------------------------
  //prune the leaf paths and get the final Tree_pred[i]
  for (i = 1; i <= N_All; i++)
    Tree_pred[i] = -1;

  for (i = 1; i <= N_All; i++) {
    if (Node_All[i].type == 1)	//node i is sink
    {
      v = i;

      do {
	u = Pred[v];
	Tree_pred[v] = u;
	Node_All[v].pred = u;
	v = u;
      } while (u != Pred[u]);
    } else {
      if (Node_All[i].type == 0)
	Tree_pred[i] = i;
    }				//root
  }

  //find degrees:
  for (u = 1; u <= N_All; u++) {
    if (Tree_pred[u] > 0)	//node u in tree
      for (v = 1; v <= N_All; v++)
	if (Tree_pred[v] == u)
	  degree[u]++;
  }				//end find degrees

  //delete intermediate nodes in simple paths
  for (i = 1; i <= N_All; i++)
    if (Node_All[i].type == 1)	//node i is sink
    {
      v = i;

      do {
	u = Tree_pred[v];
	Pred_dir = Node_All[v].pred_dir;
	while (degree[u] < 2 && Node_All[u].pred_dir == Pred_dir && Node_All[u].type < 0)
	{
          //u is intermediate and degree[u]=1
	  j = Tree_pred[u];
	  Tree_pred[u] = -1;
	  Tree_pred[v] = j;
	  u = j;
	}
	v = u;
      } while (u != Pred[u]);
    }

  //-------------------------------------------------------------------------
  //*************************************************************************
  //levels:
  L = 0;			//number of levels in tree
  number = 0;			//number of global nodes with assigned levels
  Num_nodes = 0;		//total number of global nodes in tree
  for (i = 1; i <= N_All; i++) {
    if (Tree_pred[i] > 0)
      Num_nodes++;
    level[i] = -1;
    Up_Down[i] = -1;
    if (Node_All[i].type == 0)	//i is a root node
    {
      level[i] = L;		//on level 0
      number++;
      Up_Down[i] = up;
    }
  }				//end for level 0
  while (number < Num_nodes)	//other levels, i.e. >0
  {
    L++;
    for (i = 1; i <= N_All; i++)	//upper level
      for (j = 1; j <= N_All; j++)	//lover level
      {
	if (Up_Down[i] == up && Tree_pred[j] == i && level[j] < 0) {
	  level[j] = L;
	  number++;
	  Up_Down[j] = down;
	}			//if
      }				//for
    for (i = 1; i <= N_All; i++) {
      if (Up_Down[i] == up)
	Up_Down[i] = -1;
      if (Up_Down[i] == down)
	Up_Down[i] = up;
    }				//for
  }				//while
  //endlevels

  //calculate C_j and d_{ij}. From leafs to root
  for (lev = L; lev > 0; lev--)
    for (v = 1; v <= N_All; v++)
      if (level[v] == lev) {
	u = Tree_pred[v];

        //  C[u]=C[u]+C[v];
	Pred_dir = Node_All[v].pred_dir;
	switch (Pred_dir) {
	case west:
	  res = Layer[Node_All[u].z].res * (Node_All[v].x - Node_All[u].x);
	  cap = Layer[Node_All[u].z].cap * (Node_All[v].x - Node_All[u].x);
	  break;
	case north:
	  res = Layer[Node_All[u].z].res * (Node_All[u].y - Node_All[v].y);
	  cap = Layer[Node_All[u].z].cap * (Node_All[u].y - Node_All[v].y);
	  break;
	case east:
	  res = Layer[Node_All[u].z].res * (Node_All[u].x - Node_All[v].x);
	  cap = Layer[Node_All[u].z].cap * (Node_All[u].x - Node_All[v].x);
	  break;
	case south:
	  res = Layer[Node_All[u].z].res * (Node_All[v].y - Node_All[u].y);
	  cap = Layer[Node_All[u].z].cap * (Node_All[v].y - Node_All[u].y);
	  break;
	case up:
	  res = 0;
	  cap = 0;
	  for (lay = Node_All[v].z; lay < Node_All[u].z; lay++) {
	    res = res + Layer[lay].up_res;
	    cap = cap + Layer[lay].up_cap;
	  }
	  break;
	case down:
	  res = 0;
	  cap = 0;
	  for (lay = Node_All[u].z; lay < Node_All[v].z; lay++) {
	    res = res + Layer[lay].up_res;
	    cap = cap + Layer[lay].up_cap;
	  }
	  break;
	}			//end switch
        //     C[u]=C[u]+cap;

	del[v] = res * (cap / 2 + C[v]);	//del[v]=d_{uv}, u=pred[v]
        //     C_0=C[u];
        /*
        printf ("[%d] - Edge %d to %d, res %f, cap %f cap[v] %f, del[v] %f\n",
                lev, Node_All[u].No, Node_All[v].No, res, cap, C[v], del[v]);
        */
      }				//end from leafs to root

  //from root to leafs. Calculate t_k
  for (v = 1; v <= N_All; v++)
    if (level[v] > 0)
      t[v] = 0;

    else if (level[v] == 0)
      t[v] = C_0 * r_0;
  for (lev = 1; lev <= L; lev++)
    for (v = 1; v <= N_All; v++)
      if (level[v] == lev) {
	u = Tree_pred[v];
	t[v] = t[u] + del[v];
      }
  for (k = 1; k <= N_All; k++) {
    Node_All[k].delay = t[k];
    Node_All[k].SubtreeCap = C[k];
    Node_All[k].EdgeDelay = del[k];
  }
#ifndef NDEBUG
  printf("***** Tree found for net %d min resource %d ******\n", s, MinResource);
#endif
  // PRINT_TNODE(Node_All, N_All, k)

  //**************************************************************************
  *PMinResource = MinResource;
  free(In_Tree);
  free(In_Tree_tmp);
  free(t);
  free(R);
  free(C);
  free(del);
  free(Pred);
  free(level);
  free(degree);
  // EndFunc(MAD);
}				//End MAD
