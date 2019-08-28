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
   int* f;
   int order;
   int size;
}GraphObj;

Graph newGraph (int n) {
   Graph G = malloc (sizeof(GraphObj));
   assert(G);
   G->adjList = calloc (n+1, sizeof(List));
   for (int i=1; i<=n; i++){
      G->adjList[i] = newList();
   }
   G->color = calloc (n+1, sizeof(int));
   G->p = calloc (n+1, sizeof(int));
   G->d = calloc (n+1, sizeof(int));
   G->f = calloc (n+1, sizeof(int));
   for (int i=1; i<=n; i++){
      G->color[i] = white;
      G->p[i] = NIL;
      G->d[i] = UNDEF;
      G->f[i] = UNDEF;
   }
   G->order = n;
   G->size = 0;
   return G;
}

void makeNull (Graph G) {
   for(int i=1; i<= G->order; i++)
      clear(G->adjList[i]);
   G->size = 0;
}

void freeGraph (Graph* pG){
   if(pG!=NULL && *pG!=NULL){
      free((*pG)->f);
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

int getParent (Graph G, int u) { 
   if(u < 1 || u > G->order){
      fprintf(stderr, "Graph error: getParent() called on invalid parameters\n");
      exit(EXIT_FAILURE);
   }
   return G->p[u]; 
}

int getDiscover (Graph G, int u) { 
   if(u < 1 || u > G->order){
      fprintf(stderr, "Graph error: getDiscover() called on invalid parameters\n");
      exit(EXIT_FAILURE);
   }
   return G->d[u];
}
int getFinish (Graph G, int u) {
   if(u < 1 || u > G->order){
      fprintf(stderr, "Graph error: getFinish() called on invalid parameters\n");
      exit(EXIT_FAILURE);
   }
   return G->f[u];
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


void visit(Graph G, List S, int ver, int* time){
   G->d[ver] = ++(*time);
   G->color[ver] = gray;
   List L = G->adjList[ver];

   if(length(L)!=0) {
      for (moveFront(L); index(L)>=0; moveNext(L)){
         if(G->color[get(L)]==white){
            G->p[get(L)] = ver;
            visit(G, S, get(L), time);
         }
      }    
   }

   G->color[ver] = black;
   G->f[ver] = ++(*time);
   prepend(S,ver);
}


void DFS (Graph G, List S){
   for(int i=1; i<=G->order; i++){
      G->color[i] = white;
      G->d[i] = UNDEF;
      G->f[i] = UNDEF;      
      G->p[i] = NIL;
   }
   if(length(S)!=G->order){
      fprintf(stderr, "Graph: DFS(): called on List with size larger than Graph order");
      exit(1);
   }
   int time = 0;
   moveFront(S);
   while(index(S)>=0){
      if(G->color[get(S)]==white){
         visit(G, S, get(S), &time);
      }
      moveNext(S);
   }
   for(int i=0; i<G->order; i++)   
      deleteBack(S);
}


Graph transpose (Graph G){
   Graph T = newGraph(G->order);
   for(int i=1; i<=G->order; i++){
      List L = G->adjList[i];
      if(length(L)==0) continue;
      moveFront(L);
      while(index(L)>=0){
         int vertex = get(L);
         append(T->adjList[vertex], i);
         moveNext(L);
      }
   }
   return T;
}

Graph copyGraph (Graph G){
   Graph C = newGraph(G->order);
   for(int i=1; i<=G->order; i++){
      List L = G->adjList[i];
      if(length(L)==0) continue;
      moveFront(L);
      while(index(L)>=0){
         int vertex = get(L);
         append(C->adjList[i], vertex);
         moveNext(L);
      }
   }
   return C;
}


void printGraph (FILE* out, Graph G){
   for(int i=1; i<=G->order; i++){
      fprintf(out, "%d: ", i);
      printList(out,G->adjList[i]);
      fprintf(out, "\n");
   }
}




