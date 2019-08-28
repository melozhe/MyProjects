//==================================================
// Sparse.c 
// Parsing input files and excute matrix operations
//==================================================
#include <stdio.h>
#include <stdlib.h>
#include "Matrix.h"

int main(int argc, char** argv){
   int n, a, b, row, col;
   double value; 
   FILE *in, *out;
   if(argc != 3){
      fprintf(stderr, "Usage: %s<input file> <output file>\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   
   in = fopen(argv[1], "r");
   out = fopen(argv[2], "w");
   if(in == NULL){
      fprintf(stderr, "Unable to open file %s for reading\n", argv[1]);
      exit(EXIT_FAILURE);
   } 
   if(out == NULL){
      fprintf(stderr, "Unable to open file %s for writing\n", argv[2]);
      exit(EXIT_FAILURE);
   }

   // read and change entries
   fscanf(in, "%d %d %d\n", &n, &a, &b);
   Matrix A = newMatrix(n);
   Matrix B = newMatrix(n);
   Matrix C,D,E,F,G,H,I,J;
   for(int i=1; i<=a; i++){
      fscanf(in, "%d %d %lf\n", &row, &col, &value);
      changeEntry(A, row, col, value);
   }
   for(int j=1; j<=b; j++){
      fscanf(in, "%d %d %lf\n", &row, &col, &value);
      changeEntry(B, row, col, value);
   }

   // perform Matrix operations
   fprintf(out, "A has %d non-zero entries:\n", NNZ(A));
   printMatrix(out, A);
   fprintf(out, "\n");

   fprintf(out, "B has %d non-zero entries:\n", NNZ(B));
   printMatrix(out, B);
   fprintf(out, "\n");

   fprintf(out, "(1.5)*A =\n");
   C = scalarMult(1.5, A);
   printMatrix(out, C);
   fprintf(out, "\n");
   
   fprintf(out, "A+B =\n");
   D = sum(A, B);
   printMatrix(out, D);
   fprintf(out, "\n");
   
   fprintf(out, "A+A =\n");
   E = sum(A, A);
   printMatrix(out, E);
   fprintf(out, "\n");

   fprintf(out, "B-A =\n");
   F = diff(B, A);
   printMatrix(out, F);
   fprintf(out, "\n");

   fprintf(out, "A-A =\n");
   G = diff(A, A);
   printMatrix(out, G);
   fprintf(out, "\n");

   fprintf(out, "Transpose(A) =\n");
   H = transpose(A);
   printMatrix(out, H);
   fprintf(out, "\n");

   fprintf(out, "A*B =\n");
   I = product(A, B);
   printMatrix(out, I);
   fprintf(out, "\n");

   fprintf(out, "B*B =\n");
   J = product(B, B);
   printMatrix(out, J);
   fprintf(out, "\n");

   freeMatrix(&A);
   freeMatrix(&B);
   freeMatrix(&C);
   freeMatrix(&D);
   freeMatrix(&E);
   freeMatrix(&F);
   freeMatrix(&G);
   freeMatrix(&H);
   freeMatrix(&I);
   freeMatrix(&J);

   fclose(in);
   fclose(out);
   return 0;
}
