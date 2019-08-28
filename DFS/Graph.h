//------------------------------------------------
// Graph.h
// Header file for Graph ADT
//------------------------------------------------

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "List.h"

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
int getDiscover (Graph G, int u);
int getFinish (Graph G, int u);

/*Manipulation procedures*/
void addEdge (Graph G, int u, int v);
void addArc (Graph G, int u, int v);
void DFS (Graph G, List S);

/*Other operations*/
Graph transpose(Graph G);
Graph copyGraph(Graph G);
void printGraph (FILE* out, Graph G);

#endif
