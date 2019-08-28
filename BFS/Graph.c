//---------------------------------------------------
// Graph.c
// Implementaion of Graph ADT
// Using an array of lists to represent graph
//---------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Graph.h"


enum colors{white, gray, black};

typedef struct GraphObj{
   List* adjList;
   int* color;
   int* p;
   int* d;
   int order;
   int size;
   int source;
}GraphObj;

Graph newGraph (int n){
   Graph G = malloc(sizeof(GraphObj));
   assert(G!=NULL);
   G->adjList = calloc(n+1, sizeof(List));
   assert(G->adjList!=NULL);
   for(int i=1; i<=n; i++)
      G->adjList[i] = newList();
   G->color = calloc(n+1, sizeof(int));
   G->p = calloc(n+1, sizeof(int));
   G->d = calloc(n+1, sizeof(int));
   G->order = n;
   G->size = 0;
   G->source = NIL; 
   for(int i=1; i<=n; i++){
      G->color[i] = white;
      G->p[i] = NIL;
      G->d[i] = INF;
   }
   return G;
}

void freeGraph (Graph* pG){
   if(pG!=NULL && *pG!=NULL){
      free((*pG)->d);
      free((*pG)->p);
      free((*pG)->color);
      makeNull(*pG);
      for(int i=0; i<=(*pG)->order; i++)
         freeList(&((*pG)->adjList[i]));
      free((*pG)->adjList);
      free(*pG);
      *pG = NULL;
   }
}

int getOrder (Graph G) { 
   return G->order; 
}

int getSize (Graph G) { 
   return G->size; 
}

int getSource (Graph G) { 
   return G->source; 
}

int getParent (Graph G, int u) { 
   if(u < 1 || u > G->order){
      fprintf(stderr, "Graph error: getDist() called on invalid parameters\n");
      exit(EXIT_FAILURE);
   }
   return G->p[u]; 
}

void getPath (List L, Graph G, int u) {
   if(G->source == NIL){
      fprintf (stderr, "Graph error: getPath() called on NIL source\n");
      exit (EXIT_FAILURE);
   }
   if(u < 1 || u > G->order){
      fprintf(stderr, "Graph error: getPath() called on invalid parameters\n");
      exit(EXIT_FAILURE);
   }
  
   if(u==G->source){
      append(L,G->source);
   }else if(G->p[u]==NIL){
      append (L,NIL);
   }else{
      getPath (L,G,G->p[u]);
      append (L,u);
   }
}

int getDist (Graph G, int u) {
   if(u < 1 || u > G->order){
      fprintf(stderr, "Graph error: getDist() called on invalid parameters\n");
      exit(EXIT_FAILURE);
   }
   return G->d[u];
}

void makeNull (Graph G) {
   for(int i=1; i<= G->order; i++)
      clear(G->adjList[i]);
   G->size = 0;
   G->source = NIL;
}
// precondition
void addEdge (Graph G, int u, int v){
   if(u < 1 || u > G->order || v < 1 || v > G->order){
      fprintf(stderr, "Graph error: addEdge() called on invalid parameters\n");
      exit(EXIT_FAILURE);
   }
   addArc (G,u,v);
   addArc (G,v,u);  
   G->size--;
}
// precondition u and v [1, getOrder(G)]
void addArc (Graph G, int u, int v){
   if(u < 1 || u > G->order || v < 1 || v > G->order){
      fprintf(stderr, "Graph error: addArc() called on invalid parameters\n");
      exit(EXIT_FAILURE);
   }
   List L = G->adjList[u];
   if(length(L)==0) { append(L,v); }
   else{
      moveFront(L);
      while(index(L)>=0 && v >= get(L)){
         moveNext(L);
      }
      if(index(L)<0) append(L,v);
      else insertBefore(L,v);
   }
   G->size++;
}

void BFS (Graph G, int s){
   for(int i=1; i <= G->order; i++){
      G->color[i] = white;
      G->p[i] = NIL;
      G->d[i] = INF;
   } 
   if(s < 1 || s > G->order){
      fprintf(stderr, "Graph error: BFS() called on invalid source vetex\n");
      exit(EXIT_FAILURE);
   }
   G->source = s;
   G->color[s] = gray;
   G->d[s] = 0;
   List queue = newList();
   append(queue, s);
   while(length(queue)>0){
      int x = front(queue);
      deleteFront(queue);
      List adjL = G->adjList[x];
      if(length(adjL)!=0){
         moveFront(adjL);
         while(index(adjL)>=0){
            int y = get(adjL);
            if(G->color[y] == white){
               G->p[y] = x;
               G->d[y] = G->d[x] + 1;
               G->color[y] = gray;
               append(queue, y);
            }
            moveNext(adjL);
         }
      }
      G->color[x] = black;
   }
   freeList(&queue);
}

void printGraph (FILE* out, Graph G){
   for(int i=1; i<=G->order; i++){
      fprintf(out, "%d:", i);
      printList(out,G->adjList[i]);
      fprintf(out, "\n");
   }
}




