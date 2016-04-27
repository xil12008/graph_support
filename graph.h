// graph.h
#ifndef _GRAPH_H // must be unique name in the project
#define _GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <math.h>       /* pow */
#include <map>
#include <queue>
#include <algorithm>    // std::random_shuffle
#include <limits.h>
#include <string.h>

/*
Define your own marker for the nodes
*/
#define DEFAULT_MARKER -2 


using namespace std;

// A structure to represent an adjacency list node
struct AdjListNode
{
    int dest;
    char type;
    struct AdjListNode* next;
};
 
// A structure to represent an adjacency list
struct AdjList
{
    struct AdjListNode *head;  // pointer to head node of list
};

// for random walk
#define V_m 100 // at most V_m metapathes
class Item
{
public:
    char name[5000];
    double hold[2]; // used for the score in pagerank
    Item(){ hold[0] = hold[1] = 0.0; };
    virtual void myprint() { };
};

class Paper : public Item
{
public:
    char *abstract;
    unsigned int year;
    Paper(const char *name, const char *abstract, int year){
        strcpy(this->name, name);
        this->abstract = new char[strlen(abstract)];  
	strcpy(this->abstract, abstract);
	this->year = year;
    };
    void myprint() { printf("<=================\nPaper:%s, year:%u,\n abstract: %s\n==================>\n", name, year, abstract); }; 
};

class Author : public Item
{
public:
    Author(const char *name)
    {
        strcpy(this->name, name);
    }
    void myprint() { printf("<=====================\nAuthor:%s\n=======================>\n", name); }; 
};

class Venue : public Item
{
public:
    Venue(const char *name)
    {
        strcpy(this->name, name);
    }
    void myprint() { printf("<=====================\nVenue:%s\n=======================>\n", name); }; 
};
 
// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
    int V;
    int cap;
    int* degree;
    int* marker;
    int* parent;
    char* type;
    struct AdjList* array;
    Item** item;
};
 
// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(int dest, char t);

// A utility function that creates a graph of V vertices (memory allocation)
struct Graph* createGraph(int V);
void deleteGraph(Graph* graph);

void addEdge(struct Graph* graph, int src, int dest);
void printGraph(struct Graph* graph);
void printNode(struct Graph* graph, int v);
int newNode(Graph* graph, char t);
bool isConnected(Graph* graph, int a, int b);
int findNode(Graph* graph, int deg, int forbid);
void removeEdge(Graph* graph, int src, int dst);
void removeAllEdges(Graph* graph, int v);
int randomNeighbor(Graph* graph, int id);

int networkSize(Graph *graph, int k);
void resetMarkers(Graph * graph);
int randomNode(Graph* graph, int k);

#endif
