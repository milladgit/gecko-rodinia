#include "geckoRuntime.h"
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
typedef struct Node
{
    int starting;
    int no_of_edges;
} Node;

G_GENERATOR(Node);
#ifndef gecko_bool
G_GENERATOR(bool);
#endif

void BFSGraph(int argc, char** argv);

void Usage(int argc, char**argv) {

    fprintf(stderr,"Usage: %s <input_file>\n", argv[0]);

}
////////////////////////////////////////////////////////////////////////////////
// Main Program
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv)
{
    double totalTime;
    totalTime = omp_get_wtime();

    geckoLoadConfigWithEnv();
    no_of_nodes=0;
    edge_list_size=0;
    BFSGraph( argc, argv);

    totalTime = omp_get_wtime() - totalTime;
    printf("Total time: %.2fus\n", totalTime*1E6);

    return 0;
}



////////////////////////////////////////////////////////////////////////////////
//Apply BFS on a Graph using CUDA
////////////////////////////////////////////////////////////////////////////////
void BFSGraph( int argc, char** argv)
{
    char *input_f;

    // int* h_cost;
    gecko_int h_graph_edges;

    if(argc!=2) {
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

    gecko_Node h_graph_nodes;
    gecko_bool h_graph_mask;
    gecko_bool h_updating_graph_mask;
    gecko_bool h_graph_visited;
    gecko_int  h_cost;

    geckoMemoryInternalTypeDeclare(h_graph_nodes, sizeof(Node), no_of_nodes, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryInternalTypeDeclare(h_graph_mask, sizeof(bool), no_of_nodes, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryInternalTypeDeclare(h_updating_graph_mask, sizeof(bool), no_of_nodes, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryInternalTypeDeclare(h_graph_visited, sizeof(bool), no_of_nodes, exec_loc, GECKO_DISTANCE_NOT_SET);
    geckoMemoryInternalTypeDeclare(h_cost, sizeof(int), no_of_nodes, exec_loc, GECKO_DISTANCE_NOT_SET);

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
    geckoMemoryInternalTypeDeclare(h_graph_edges, sizeof(int), edge_list_size, exec_loc, GECKO_DISTANCE_NOT_SET);
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
        double time = omp_get_wtime();

//	#pragma acc update device(h_graph_nodes[0:no_of_nodes]) async(TRANSFER_GRAPH_NODE)

        {
            int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
            GeckoLocation **dev = NULL;
            int ranges_count = 0;
            float *ranges = NULL;
            int var_count = 0;
            void **var_list = NULL;
            GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, no_of_nodes, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr() async(asyncID) copyin(h_updating_graph_mask[0:1],h_graph_mask[0:1],h_graph_visited[0:1])
                        for(unsigned int i = beginLI; i < endLI; i++)
                        {
                            h_updating_graph_mask[i]=false;
                            h_graph_mask[i]=false;
                            h_graph_visited[i]=false;
                        }
#pragma acc wait(asyncID)
                    } // end of if(dev[devIndex]!=NULL)
                } // end of OpenMP pragma
            } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
            geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
        }


        {
            int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
            GeckoLocation **dev = NULL;
            int ranges_count = 0;
            float *ranges = NULL;
            int var_count = 0;
            void **var_list = NULL;
            GeckoError err = geckoRegion("any", exec_loc, 0, 1, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop num_gangs(1) vector_length(1) deviceptr() async(asyncID) copyin(h_graph_mask[0:1],h_graph_visited[0:1])
                        for(int i = beginLI; i < endLI; i++)
                        {
                            //set the source node as true in the mask
                            h_graph_mask[source]=true;
                            h_graph_visited[source]=true;
                        }
#pragma acc wait(asyncID)
                    } // end of if(dev[devIndex]!=NULL)
                } // end of OpenMP pragma
            } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
            geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
        }

        // allocate mem for the result on host side
        // h_cost = (int*) malloc( sizeof(int)*no_of_nodes);
        {
            int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
            GeckoLocation **dev = NULL;
            int ranges_count = 0;
            float *ranges = NULL;
            int var_count = 0;
            void **var_list = NULL;
            GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, no_of_nodes, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
            jobCount = devCount;
            if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                #pragma omp parallel num_threads(jobCount)
                {
                    int devIndex = omp_get_thread_num();
                    if(dev[devIndex] != NULL) {
                        int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                        int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop deviceptr() async(asyncID) copyin(h_cost[0:1])
                        for(int i = beginLI; i < endLI; i++) {
                            h_cost[i]=-1;
                            if(i == source) h_cost[source]=0;
                        }
#pragma acc wait(asyncID)
                    } // end of if(dev[devIndex]!=NULL)
                } // end of OpenMP pragma
            } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
            geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
        }

        // finish transfer node and edge to target
//	#pragma acc update device(h_graph_edges[0:edge_list_size])
//	#pragma acc wait(TRANSFER_GRAPH_NODE)
        geckoWaitOnLocation(exec_loc);


        printf("Start traversing the tree\n");

        int k=0;

        bool stop;
        do
        {
            //if no thread changes this value then the loop stops
            stop=false;

            {
                int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
                GeckoLocation **dev = NULL;
                int ranges_count = 0;
                float *ranges = NULL;
                int var_count = 0;
                void **var_list = NULL;
                GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, no_of_nodes, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
                jobCount = devCount;
                if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                    #pragma omp parallel num_threads(jobCount)
                    {
                        int devIndex = omp_get_thread_num();
                        if(dev[devIndex] != NULL) {
                            int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                            int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop independent deviceptr() async(asyncID) copyin(h_graph_mask[0:1],h_graph_nodes[0:1],h_graph_edges[0:1],h_graph_visited[0:1],h_cost[0:1],h_updating_graph_mask[0:1])
                            for(int tid = beginLI; tid < endLI; tid++)
                            {
                                if (h_graph_mask[tid] == true) {
                                    h_graph_mask[tid]=false;
#pragma acc loop independent
                                    for(int i=h_graph_nodes[tid].starting; i<(h_graph_nodes[tid].no_of_edges + h_graph_nodes[tid].starting); i++)
                                    {
                                        int id = h_graph_edges[i];
                                        bool visited = h_graph_visited[id];
                                        if(!visited) {
//#pragma acc atomic write
                                            h_cost[id]=h_cost[tid]+1;

                                            h_updating_graph_mask[id]=true;
                                        }
                                    }
                                }
                            }
#pragma acc wait(asyncID)
                        } // end of if(dev[devIndex]!=NULL)
                    } // end of OpenMP pragma
                } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
                geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
            }

            {
                int *beginLoopIndex=NULL, *endLoopIndex=NULL, jobCount, devCount, devIndex;
                GeckoLocation **dev = NULL;
                int ranges_count = 0;
                float *ranges = NULL;
                int var_count = 0;
                void **var_list = NULL;
                GeckoError err = geckoRegion(exec_policy_chosen, exec_loc, 0, no_of_nodes, 1, 0, &devCount, &beginLoopIndex, &endLoopIndex, &dev, ranges_count, ranges, var_count, var_list);
                jobCount = devCount;
                if(err != GECKO_ERR_TOTAL_ITERATIONS_ZERO) {
                    #pragma omp parallel num_threads(jobCount)
                    {
                        int devIndex = omp_get_thread_num();
                        if(dev[devIndex] != NULL) {
                            int beginLI = beginLoopIndex[devIndex], endLI = endLoopIndex[devIndex];
                            int asyncID = dev[devIndex]->getAsyncID();
#pragma acc parallel loop vector reduction(||:stop) independent deviceptr() async(asyncID) copyin(h_graph_mask[0:1],h_graph_visited[0:1],h_cost[0:1],h_updating_graph_mask[0:1])
                            for(int tid = beginLI; tid < endLI; tid++)
                            {
                                if (h_updating_graph_mask[tid] == true) {
                                    h_graph_mask[tid]=true;
                                    h_graph_visited[tid]=true;
                                    stop=true;
                                    h_updating_graph_mask[tid]=false;
                                }
                            }
#pragma acc wait(asyncID)
                        } // end of if(dev[devIndex]!=NULL)
                    } // end of OpenMP pragma
                } // end of checking: err != GECKO_ERR_TOTAL_ITERATIONS_ZERO
                geckoFreeRegionTemp(beginLoopIndex, endLoopIndex, devCount, dev, var_list);
            }

            k++;
        }
        while(stop);

        time = omp_get_wtime() - time;
        printf("Total time for computation: %.2f\n", time*1E6);

    } /* end acc data */

    geckoWaitOnLocation(exec_loc);

    //Store the result into a file
    FILE *fpo = fopen("result.txt","w");
    for(int i=0; i<no_of_nodes; i++)
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

//#pragma gecko memory free(h_graph_nodes)
//#pragma gecko memory free(h_graph_edges)
//#pragma gecko memory free(h_graph_mask)
//#pragma gecko memory free(h_updating_graph_mask)
//#pragma gecko memory free(h_graph_visited)
//#pragma gecko memory free(h_cost)

    h_graph_nodes.freeMem();

}

