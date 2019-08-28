#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"

void printSCC(FILE* out, Graph G, List S);

int main(int argc, char** argv){
   FILE *in, *out;
   int n, vex1, vex2;
   
   if(argc != 3){
      printf("Usage: %s <input file> <output file>\n", argv[0]);
      exit(1);
   }
   
   in = fopen(argv[1], "r");
   out = fopen(argv[2], "w");

   if(in == NULL){
      printf("Unable to open file %s for reading\n", argv[1]);
      exit(1);
   }
   if(out == NULL){
      printf("Unable to open file %s for writinging\n", argv[2]);
      exit(1);
   }
  
   fscanf(in, "%d\n", &n);
   Graph G = newGraph(n);
   fscanf(in, "%d %d\n", &vex1, &vex2);
   while(vex1 != 0 && vex2 != 0){
      addArc (G,vex1,vex2);
      fscanf(in, "%d %d\n", &vex1, &vex2);
   }
   fprintf(out, "Adjacency list representation of G:\n");
   printGraph(out,G);
   fprintf(out, "\n");

   List S = newList();
   for(int i=1; i<=n; i++) append(S, i);
   DFS(G, S); 
   Graph T = transpose(G);
   DFS(T, S);

   int cnt = 0;
   for(int i=1; i<=n; i++){ 
      if(getParent(T, i) == NIL)
         cnt++;
   }
   
   fprintf(out, "G contains %d strongly connected components:\n", cnt); 
   moveBack(S);
   for(int i=1; i<=cnt; i++){
      fprintf(out, "Component %d: ", i);
      printSCC(out, T, S); 
      fprintf(out, "\n");
      movePrev(S);
   }
   
   freeList(&S);
   freeGraph(&G);
   freeGraph(&T);
   fclose(in);
   fclose(out); 
   return 0;
}


void printSCC(FILE* out, Graph G, List S){
   int vex = get(S); 
   if(getParent(G, vex) == NIL) 
      fprintf(out, "%d ", vex);
   else{
      movePrev(S);
      printSCC(out, G, S);
      fprintf(out,"%d ", vex);
   }
}




