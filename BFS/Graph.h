//------------------------------------------------
// Graph.h
// Header file for Graph ADT
//------------------------------------------------

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "List.h"

#define INF -101
#define NIL -102

typedef struct GraphObj* Graph;

/*Ctor and Dtor*/ 
Graph newGraph (int n);
void freeGraph (Graph* pG);

/*Access functions*/
int getOrder (Graph G);
int getSize (Graph G);
int getSource (Graph G);
int getParent (Graph G, int u);
int getDist (Graph G, int u);
void getPath (List L, Graph G, int u);

/*Manipulation procedures*/
void makeNull (Graph G);
void addEdge (Graph G, int u, int v);
void addArc (Graph G, int u, int v);
void BFS (Graph G, int s);

/*Other operations*/
void printGraph (FILE* out, Graph G);

#endif
