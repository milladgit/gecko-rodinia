#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define TRANSFER_GRAPH_NODE 1

int no_of_nodes;
int edge_list_size;
FILE *fp;

static char *exec_loc = "LocB";
static char *exec_policy_chosen = "static";


//Structure to hold a node information
struct Node
{
	int starting;
	int no_of_edges;
};

void BFSGraph(int argc, char** argv);

void Usage(int argc, char**argv){

fprintf(stderr,"Usage: %s <input_file>\n", argv[0]);

}
////////////////////////////////////////////////////////////////////////////////
// Main Program
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv) 
{
#pragma gecko config env

	no_of_nodes=0;
	edge_list_size=0;
	BFSGraph( argc, argv);
}



////////////////////////////////////////////////////////////////////////////////
//Apply BFS on a Graph using CUDA
////////////////////////////////////////////////////////////////////////////////
void BFSGraph( int argc, char** argv) 
{
    char *input_f;

    // int* h_cost;
    int* h_graph_edges;
	
	if(argc!=2){
	Usage(argc, argv);
	exit(0);
	}
    
	input_f = argv[1];
	
	printf("Reading File\n");
	//Read in Graph from a file
	fp = fopen(input_f,"r");
	if(!fp)
	{
		printf("Error Reading graph file\n");
		return;
	}

	int source = 0;

	fscanf(fp,"%d",&no_of_nodes);
   
	// allocate host memory
//	Node* h_graph_nodes = (Node*) malloc(sizeof(Node)*no_of_nodes);
//	bool *h_graph_mask = (bool*) malloc(sizeof(bool)*no_of_nodes);
//	bool *h_updating_graph_mask = (bool*) malloc(sizeof(bool)*no_of_nodes);
//	bool *h_graph_visited = (bool*) malloc(sizeof(bool)*no_of_nodes);
//	int  *h_cost = (int*) malloc(sizeof(int)*no_of_nodes);

	Node* h_graph_nodes;
	bool *h_graph_mask;
	bool *h_updating_graph_mask;
	bool *h_graph_visited;
	int  *h_cost;

#pragma gecko memory allocate(h_graph_nodes[0:no_of_nodes]) type(Node) location(exec_loc)
#pragma gecko memory allocate(h_graph_mask[0:no_of_nodes]) type(bool) location(exec_loc)
#pragma gecko memory allocate(h_updating_graph_mask[0:no_of_nodes]) type(bool) location(exec_loc)
#pragma gecko memory allocate(h_graph_visited[0:no_of_nodes]) type(bool) location(exec_loc)
#pragma gecko memory allocate(h_cost[0:no_of_nodes]) type(int) location(exec_loc)

	int start, edgeno;   
	// initalize the memory
	for( unsigned int i = 0; i < no_of_nodes; i++) 
	{
		fscanf(fp,"%d %d",&start,&edgeno);
		h_graph_nodes[i].starting = start;
		h_graph_nodes[i].no_of_edges = edgeno;
	}

	//read the source node from the file
	fscanf(fp,"%d",&source);
	source=0;
	
	fscanf(fp,"%d",&edge_list_size);
	
	int id,cost;
//	h_graph_edges = (int*) malloc(sizeof(int)*edge_list_size);
#pragma gecko memory allocate(h_graph_edges[0:edge_list_size]) type(int) location(exec_loc)
	for(int i=0; i < edge_list_size ; i++)
	{
		fscanf(fp,"%d",&id);
		fscanf(fp,"%d",&cost);
		h_graph_edges[i] = id;
	}

	if(fp)
		fclose(fp); 

//#pragma acc data create(h_updating_graph_mask[0:no_of_nodes]) \
//	create(h_graph_mask[0:no_of_nodes],h_graph_visited[0:no_of_nodes]) \
//	create(h_graph_nodes[0:no_of_nodes], h_graph_edges[0:edge_list_size]) \
//	copyout(h_cost[0:no_of_nodes])
{
//	#pragma acc update device(h_graph_nodes[0:no_of_nodes]) async(TRANSFER_GRAPH_NODE)

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen) variable_list(h_updating_graph_mask,h_graph_mask,h_graph_visited)
	#pragma acc parallel loop
	for( unsigned int i = 0; i < no_of_nodes; i++)
	{
		h_updating_graph_mask[i]=false;
		h_graph_mask[i]=false;
		h_graph_visited[i]=false;
	}
#pragma gecko region end


#pragma gecko region at(exec_loc) exec_pol("any") variable_list(h_updating_graph_mask,h_graph_mask,h_graph_visited)
	#pragma acc serial num_gangs(1) vector_length(1) present(h_graph_mask[0:no_of_nodes],h_graph_visited[0:no_of_nodes])
	for(int i=0;i<1;i++)
	{
	    //set the source node as true in the mask
	    h_graph_mask[source]=true;
		h_graph_visited[source]=true;
	}
#pragma gecko region end

	// allocate mem for the result on host side
	// h_cost = (int*) malloc( sizeof(int)*no_of_nodes);
#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen) variable_list(h_cost)
	#pragma acc parallel loop
	for(int i=0;i<no_of_nodes;i++) {
		h_cost[i]=-1;
		if(i == source) h_cost[source]=0;
	}
#pragma gecko region end

	// finish transfer node and edge to target
//	#pragma acc update device(h_graph_edges[0:edge_list_size])
//	#pragma acc wait(TRANSFER_GRAPH_NODE)
#pragma gecko region pause at(exec_loc)


	printf("Start traversing the tree\n");

	int k=0;
    
	bool stop;
	do
	{
		//if no thread changes this value then the loop stops
		stop=false;

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen) variable_list(h_graph_mask,h_cost,h_graph_nodes,h_graph_visited,h_updating_graph_mask,h_graph_edges)
		#pragma acc parallel loop independent
		for(int tid = 0; tid < no_of_nodes; tid++ )
		{
			if (h_graph_mask[tid] == true) {
				h_graph_mask[tid]=false;
#pragma acc loop independent
				for(int i=h_graph_nodes[tid].starting; i<(h_graph_nodes[tid].no_of_edges + h_graph_nodes[tid].starting); i++)
				{
					int id = h_graph_edges[i];
					bool visited = h_graph_visited[id];
					if(!visited) {
#pragma acc atomic write
						h_cost[id]=h_cost[tid]+1;

						h_updating_graph_mask[id]=true;
					}
				}
			}
		}
#pragma gecko region end

#pragma gecko region at(exec_loc) exec_pol(exec_policy_chosen) variable_list(h_graph_mask,h_cost,h_graph_nodes,h_graph_visited,h_updating_graph_mask)
		#pragma acc parallel loop vector reduction(||:stop) independent
  		for(int tid=0; tid< no_of_nodes ; tid++ )
		{
			if (h_updating_graph_mask[tid] == true){
				h_graph_mask[tid]=true;
				h_graph_visited[tid]=true;
				stop=true;
				h_updating_graph_mask[tid]=false;
			}
		}
#pragma gecko region end

		k++;
	}
	while(stop);

} /* end acc data */

#pragma gecko region pause at(exec_loc)

	//Store the result into a file
	FILE *fpo = fopen("result.txt","w");
	for(int i=0;i<no_of_nodes;i++)
		fprintf(fpo,"%d) cost:%d\n",i,h_cost[i]);
	fclose(fpo);
	printf("Result stored in result.txt\n");


	// cleanup memory
//	free( h_graph_nodes);
//	free( h_graph_edges);
//	free( h_graph_mask);
//	free( h_updating_graph_mask);
//	free( h_graph_visited);
//	free( h_cost);

#pragma gecko memory free(h_graph_nodes)
#pragma gecko memory free(h_graph_edges)
#pragma gecko memory free(h_graph_mask)
#pragma gecko memory free(h_updating_graph_mask)
#pragma gecko memory free(h_graph_visited)
#pragma gecko memory free(h_cost)

}

