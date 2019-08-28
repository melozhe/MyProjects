#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"

int main(int argc, char** argv){
   FILE *in, *out;
   int n, vex1, vex2;

   if(argc != 3){
      printf("Usage: %s <input file> <outout file>\n", argv[0]);
      exit(1);
   }

   in = fopen (argv[1], "r");
   out = fopen (argv[2], "w");
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
      addEdge (G,vex1,vex2);
      fscanf(in, "%d %d\n", &vex1, &vex2);
   }
   printGraph(out,G);
   
   fscanf (in, "%d %d\n", &vex1, &vex2);
   List L = newList();
   while (vex1 != 0 && vex2 != 0){
      fprintf(out, "\n");
      BFS(G,vex1);
      int dist = getDist(G, vex2);
      if(dist == INF){
         fprintf(out, "The distance from %d to %d is infinity\n", vex1, vex2);
         fprintf(out, "No %d-%d path exists", vex1, vex2);
      }else{
         fprintf(out, "The distance from %d to %d is %d\n", vex1, vex2, dist);
         fprintf(out, "A shortest %d-%d path is:", vex1, vex2);
         getPath(L,G,vex2);
         for(moveFront(L); index(L)>=0; moveNext(L))
            fprintf(out, " %d", get(L));
      }
      fprintf(out, "\n");
      clear(L);
      fscanf(in, "%d %d\n", &vex1, &vex2);
   }

   freeList(&L);
   freeGraph(&G);
   fclose(in);
   fclose(out);
   return 0;
}
