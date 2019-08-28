#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "List.h"

#define MAX_LEN 160

int main(int argc, char** argv){
   char line[MAX_LEN];
   int n = 0;
   FILE *in, *out;
   if(argc != 3){
      fprintf(stderr,"Usage: %s <input file> <output file>\n", argv[0]);
      exit(1);
   }   
   // open files for reading an writing
   in = fopen(argv[1], "r");
   out = fopen(argv[2], "w");
   if(!in){
      fprintf(stderr,"Unable to open file %s for reading\n", argv[1]);
      exit(1);
   }
   if(!out){
      fprintf(stderr,"Unable to open file %s for writing\n", argv[2]);
      exit(1);
   }
   
   // count the numbers of lines n in the input file   
   while(fgets(line, MAX_LEN, in) != NULL){
      n++;
   }
   fseek(in,0,SEEK_SET);
   
   // Read in lines of the file as strings and place them in the array
   char* strs[n];
   int i=0;
   while(fgets(line, MAX_LEN, in) != NULL){
      strs[i] = malloc(strlen(line)+1);
      strcpy(strs[i++],line);
   }
   
   List L = newList();
   prepend(L,0);
   // outer loop, place the index of each strs[] element in the proper place
   for(int i=1;i<n;i++){
      moveFront(L);
      // if the string belongs to the right
      while(index(L)>=0 && strcmp(strs[i],strs[get(L)])>0){
         moveNext(L);
      }
      if(index(L)==UNDEF)
         append(L,i);
      else
         insertBefore(L,i);
   }
   // print the strings in alphabetical order to outfile
   moveFront(L);
   while(index(L)>=0){
      fprintf(out, strs[get(L)]);
      moveNext(L);

   }
   
   // free up the memory of strs[]   
   for(int i=0;i<n;i++){
      free(strs[i]);
      strs[i] = NULL;
   }
   
   // free list
   freeList(&L);
   // close files
   fclose(in);
   fclose(out);
   return 0;
}
