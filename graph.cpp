/*-----graph utilities----*/
// A C Program to demonstrate adjacency list representation of graphs
#include "graph.h"

#define TRIAL_FIND_DEGREE 20
#define UNDIRECTED_GRAPH

// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest, char t)
{
    struct AdjListNode* newNode =
            (struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->type = t;
    newNode->next = NULL;
    return newNode;
};

void deleteAdjListNode(struct AdjListNode* node)
{
    while (node != NULL)
    {
        struct AdjListNode* tmp = node->next;
        free(node);
        node = tmp;
    }
};

// A utility function that creates a graph of V vertices
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
    graph->cap = 0;
 
    // Create an array of adjacency lists.  Size of array will be V
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
    graph->degree = (int*) malloc(V * sizeof(int));
    graph->marker = (int*) malloc(V * sizeof(int));
    graph->parent = (int*) malloc(V * sizeof(int));
    graph->type = (char*) malloc(V * sizeof(char));
    graph->item = (Item **) malloc(V * sizeof(Item *));

     // Initialize each adjacency list as empty by making head as NULL
    int i;
    for (i = 0; i < V; ++i)
    {
        graph->array[i].head = NULL;
        graph->degree[i] = 0;
        graph->marker[i] = DEFAULT_MARKER;
        graph->parent[i] = -1;
        graph->item[i] = NULL;
    }
    return graph;
};

// A utility function that deletes a graph of V vertices
void deleteGraph(Graph* graph)
{
    free(graph->array);
    free(graph->degree);
    free(graph->marker);
    free(graph);
    graph = NULL;
    printf("Network Deleted.\n");
};

// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, int src, int dest)
{
    if (src < 0 || dest < 0 || src >= graph->cap || dest >= graph->cap) {
        fprintf(stderr, "Sorry, wrong node id src=%d dest=%d.\n", src, dest);
        return;
    }
    // Add an edge from src to dest.  A new node is added to the adjacency
    // list of src.  The node is added at the begining
    struct AdjListNode* newNode = newAdjListNode(dest, 'o');
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
    ++graph->degree[src]; //In an undirected graph, the degree is the out-degree.
    //for metapath traceback 
    graph->parent[dest] = src;
 
#ifdef UNDIRECTED_GRAPH
    // If the graph is undirected, add an edge from dest to src also
    newNode = newAdjListNode(src, 'i');
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
    ++graph->degree[dest];
#endif
}
 
// A utility function to print the adjacenncy list representation of graph
void printGraph(struct Graph* graph)
{
    int v;
    printf("  cap=%d", graph->cap);
    for (v = 0; v < graph->cap; ++v)
    {
        printNode(graph, v);
    }
    printf("=================\n");
}

//print one node
void printNode(struct Graph* graph, int v)
{
    struct AdjListNode* pCrawl = graph->array[v].head;
    printf("\n Adjacency list of vertex %d (type %c, marker %d, degree %d)\n head ", v, graph->type[v], graph->marker[v], graph->degree[v]);
    while (pCrawl)
    {
        printf("-> %d(Marker%d)", pCrawl->dest, graph->marker[pCrawl->dest]);
        pCrawl = pCrawl->next;
    }
    printf("\n");
    if (graph->item[v] != NULL) graph->item[v]->myprint();
    printf("\n");
}

// return the id of newly joining node
int newNode(Graph* graph, char t)
{
    if ( graph->cap >= graph->V) {
        fprintf(stderr, "Sorry, insufficient memory space when adding node.\n");
        return -1;
    }
    graph->type[graph->cap] = t;
    return graph->cap++; //the assigned node id increases by 1
}

// One edge from a --> b  ??
// only in single direction graph, you could replace a and b
bool isConnected(Graph* graph, int a, int b)
{
    if (a == -1 || b == -1) return false;
    if (a == b) return true; // NOTE: treat as connected even if same node
    struct AdjListNode* pCrawl = graph->array[a].head;
    while (pCrawl)
    {
        if (pCrawl->dest == b)
        {
            //printf("conflict:%d->%d\n", a, b);
            return true;  
        } 
        pCrawl = pCrawl->next;
    }
    return false;
}

int randomNeighbor(Graph* graph, int id)
{
    struct AdjListNode* pCrawl = graph->array[id].head;
    int njump = rand() % graph->degree[id];
    for (int i = 0;i < njump - 1; ++i)
    {
        pCrawl = pCrawl->next;    
    }
    if (graph->degree[id] == 0) 
    {
        printf("------------------------\nWARNING: randomNeighbor returns %d", pCrawl->dest);   
    }
    return pCrawl->dest;
}

// find a random node ID with desired degree 
// @TODO complete randomness
int findNode(Graph* graph, int deg, int forbid = -1) 
{
    int rnum;
    for (int i = 0; i < TRIAL_FIND_DEGREE; ++i)
    {
        rnum = rand() % graph->cap;
        // not connected with forbid, in case return the same node twice.
        if (graph->degree[rnum] == deg && ! isConnected(graph, rnum, forbid) )
        {
            return rnum;
        } 
    }
    //if it did not find the node with desired deg, then
    vector<int> vec; 
    for (int i = 0; i < graph->cap; ++i)
    {
        if (graph->degree[i] == deg)
        {
            vec.push_back(i);
        }
    }
    if (vec.size() > 0)
    {
        int tmp_count = 30 * vec.size();
        while (--tmp_count > 0) 
        {
            int ret = rand() % vec.size();
            if( ! isConnected(graph, vec[ret], forbid) && ! graph->degree[vec[ret]] == 0)
                return vec[ret];
        }
    }
    //printf("\rWARNING: No node has degree %d (current graph size = %d)\n", deg, graph->cap);
    return -1;
}

// NOTE: it's just one direction
void removeEdge(Graph* graph, int src, int dst)
{
    struct AdjListNode* pCrawl = graph->array[src].head;
    struct AdjListNode* prev = graph->array[src].head;
    while (pCrawl)
    {
        //printf("%d dst%d\n", pCrawl->dest, dst);
        if (pCrawl->dest == dst)
        {
            if (pCrawl == graph->array[src].head)
            {
                graph->array[src].head = pCrawl->next;
            }
            else
            {
                prev->next = pCrawl->next;
                free(pCrawl);
                pCrawl = NULL;
            }
            graph->degree[src]--;
            return;
        }
        prev = pCrawl;
        pCrawl = pCrawl->next;
    }
    fprintf(stderr, "ERROR: remove edge failed src=%d dst=%d\n", src, dst);
};

void removeAllEdges(Graph* graph, int v)
{
#ifdef UNDIRECTED_GRAPH
    struct AdjListNode* pCrawl = graph->array[v].head;
    while (pCrawl)
    {
        removeEdge(graph, pCrawl->dest, v);
        pCrawl = pCrawl->next;
    }
#endif
    deleteAdjListNode(graph->array[v].head);
    graph->array[v].head = NULL;
    graph->degree[v] = 0;
}

void resetMarkers(Graph * graph)
{
    for (int i = 0; i < graph->cap; ++i)
    {
        graph->marker[i] = DEFAULT_MARKER;
	graph->parent[i] = -1;
    }
}

int networkSize(Graph *graph, int k)
{
    int sum = 0, errorsum = 0;
    for( int i = 0;i < graph->cap; ++i)
    {
        if ( graph->degree[i] >= k)
	{
	    ++sum;
	}
	else if ( graph->degree[i] < k && graph->degree[i] > 0)
	{
	    ++errorsum;
	}
    }
    fprintf(stderr, "Total %d number of nodes are less than minimum degree %d", errorsum, k);
    return sum;
}

int randomNode(Graph* graph, int k)
{
    int rid = -1;
    while (1)
    {
        rid = rand() % graph->cap;
        if (graph->degree[rid] >= k) break; //ignore the range of degree
    }
    return rid;
}

