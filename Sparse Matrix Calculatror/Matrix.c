//------------------------------------------------------------------------- 
// Matrix.c
// Implementation file for Matrix ADT
// Matrix ADT is an array of generic ListADTs
// Entry is an inner struct containing the column and value information-
// which will be the filed of node in ListADT
//------------------------------------------------------------------------- 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "List.h"
#include "Matrix.h"

typedef struct EntryObj{
   int col;
   double value;
}EntryObj;

typedef EntryObj* Entry;

typedef struct MatrixObj{
   List* row;
   int size;
}MatrixObj;

Entry newEntry(int newCol, double newValue){
   Entry E = malloc(sizeof(EntryObj));
   assert(E);
   E->col = newCol;
   E->value = newValue;
   return E;
}

void freeEntry(Entry* pE){
   if(pE != NULL && *pE != NULL){
      free(*pE);
      *pE = NULL;
   }
}

Matrix newMatrix(int n){
   Matrix M = malloc(sizeof(MatrixObj));
   assert(M);
   M->row = calloc(n+1,sizeof(List));
   for(int i=1; i<=n; i++)
      M->row[i] = newList();
   M->size = n;
   return M;
}

void freeMatrix(Matrix* pM){
   if(pM != NULL && *pM != NULL){
      makeZero(*pM);
      for(int i=1; i<=(*pM)->size; i++)
         freeList(&((*pM)->row[i]));
      free((*pM)->row);
      free(*pM);
      *pM = NULL;
   }
}

void makeZero(Matrix M){
   for(int i=1; i<= M->size; i++){
      List row = M->row[i];
      if(length(row)==0) continue;
      moveFront(row);
      while(index(row)>=0){
         Entry E = get(row);
	 freeEntry(&E);
	 moveNext(row);
      }
      clear(row);
   }   
}

int size(Matrix M){
   return M->size;
}

int NNZ(Matrix M){
   int cnt = 0;
   for(int i=1; i<=M->size; i++){
      List row = M->row[i];
      if(length(row)==0) continue;
      moveFront(row);
      while(index(row)>=0){
         cnt++;
         moveNext(row);
      }
   }
   return cnt;
}

int equals(Matrix A, Matrix B){
   int eq = 1;
   if(A->size != B->size) return 0;
   for(int i=1; i< A->size; i++){
      List L = A->row[i];
      List R = B->row[i];
      if(length(L) != length(R)) return 0;
      if(length(L) == 0) continue;
      moveFront(L);
      moveFront(R);
      while(eq && index(L)>=0){
         Entry LE = get(L);
         Entry RE = get(R);
         eq = ((LE->col == RE->col) && (LE->value == RE->value));
         moveNext(L);
         moveNext(R);
      }
      if(!eq) break;
   }
   return eq;
}
void changeEntry(Matrix M, int i, int j, double x){
   if( i<1 || i>size(M) || j<1 || j>size(M)){
      fprintf(stderr, "Matrix error: invalid row or col number\n");
      exit(EXIT_FAILURE);
   }
   if(x == 0) return;
   // now we have a valid entry 
   List row = M->row[i];
   Entry E = newEntry(j,x);

   if(length(row)==0) {
      prepend(row,E);
      return;
   }
   // now we have a normal row containing a couple entries
   moveFront(row);
   Entry tmp ;
   while(index(row)>=0){
      tmp = get(row);
      if(j < tmp->col) break;
      moveNext(row);
   }
   if(index(row) == UNDEF) append(row, E);
   else insertBefore(row, E);
}

Matrix scalarMult(double x, Matrix A){
   Matrix M = newMatrix(A->size);
   for(int i=1; i<=A->size; i++){
      List row = A->row[i]; 
      List new_row = M->row[i];
      if(length(row)==0) continue;
      moveFront(row);
      while(index(row)>=0){
         Entry E = get(row);
         Entry new_E = newEntry(E->col, x * E->value);
         append(new_row, new_E);
         moveNext(row);
      } 
   }
   return M;
}

void vectorCopy(List dest, List src){
   Entry P, Q;
   moveFront(src);
   while(index(src)>=0){
      P = get(src);
      Q = newEntry(P->col, P->value);
      append(dest, Q);
      moveNext(src);
   }
}

Matrix copy(Matrix A){
   int size = A->size;
   Matrix M = newMatrix(size);
   for(int i=1; i<=size; i++){
      List L = M->row[i];
      List R = A->row[i];
      if(length(R)==0) continue;
      vectorCopy(L,R);
   }
   return M;
}

void vectorAdd(List result, List L, List R){
   double sum; int col;
   Entry LE, RE, newE;
   
   if(length(L)==0 && length(R)==0){ 
       return;
   }else if(length(L)==0){
      vectorCopy(result, R); 
   }else if(length(R)==0){
      vectorCopy(result, L); 
   }else{
      moveFront(L);
      moveFront(R);
      // go through two rows at the same time
      while(index(L)>=0 && index(R)>=0){
         LE = get(L);
         RE = get(R);
         if(LE->col == RE->col){
            sum = LE->value + RE->value;
	    col = LE->col;
	    moveNext(L);
	    moveNext(R);
         }else if(LE->col < RE->col){
	    sum = LE->value;
	    col = LE->col;
            moveNext(L);
         }else{
	    sum = RE->value;
	    col = RE->col;
            moveNext(R);
         }
         // now we hava what we need to construct a new entry
	 if(sum != 0){
            newE = newEntry(col, sum);
            append(result, newE);
	 }
      }
      // see if any list has remaining entries   
      if(index(L) < 0){// right list has extra entries
         while(index(R)>=0){
	    RE = get(R);
	    newE = newEntry(RE->col, RE->value);
            append(result, newE);
	    moveNext(R);
	 }
      }else if(index(R) < 0){
         while(index(L)>=0){
	    LE = get(L);
	    newE = newEntry(LE->col, LE->value);
            append(result, newE);
	    moveNext(L);
	 }
      }// both lists are exhausted no action needed
   }
}

Matrix sum(Matrix A, Matrix B){
   if(A->size != B->size){
      fprintf(stderr, "Matrix error: \
         sum called on Matrixes with different size\n");
      exit(EXIT_FAILURE);
   }
   Matrix M;
   if(A==B){
      M = scalarMult(2,A);
      return M;
   }
   
   int size = A->size;
   M = newMatrix(size);
   // decide if it is necessary to call vector sum
   // A = 0, B = 0
   // A = 0
   // B = 0
   for(int i=1; i<=size; i++){
      List L = A->row[i];
      List R = B->row[i];
      List result = M->row[i];
      vectorAdd(result, L, R);
   }
   return M;
}

Matrix negate(Matrix A){
   int size = A->size;
   Matrix M = newMatrix(size);
   for(int i=1; i<=size; i++){
      List L = A->row[i];
      List R = M->row[i];
      if(length(L)==0) continue;
      moveFront(L);
      while(index(L)>=0){
         Entry LE = get(L);
         Entry RE = newEntry(LE->col, -LE->value);
         append(R, RE);
         moveNext(L);
      }
   }
   return M;
}


Matrix diff(Matrix A, Matrix B){
   if(A->size != B->size){
      fprintf(stderr, "Matrix error: \
         diff called on Matrixes with different size\n");
      exit(EXIT_FAILURE);
   }
   Matrix _B, M;
   //if(A == B) return M;
   //if(NNZ(A) == 0 && NNZ(B) == 0) return M;
   _B = negate(B);
   M = sum(A, _B);
   freeMatrix(&_B);
   return M;
}

Matrix transpose(Matrix A){
   Matrix M;
   List LA, LM;
   Entry LAE, LME;
   int row, col, row_t, col_t, size;

   size = A->size;
   M = newMatrix(size);
   for(int i=1; i<=size; i++){
      LA = A->row[i];
      if(length(LA)==0) continue;
      moveFront(LA); 
      while(index(LA)>=0){
         LAE = get(LA);
         row = i; col = LAE->col;
         row_t = col; col_t = row;
         LM = M->row[row_t];
         LME = newEntry(col_t, LAE->value);
         append(LM, LME);
         moveNext(LA);
      } 
   }
   return M;
}

double vectorProd(List A, List B){
   double result = 0;
   Entry EA, EB;
   moveFront(A);
   moveFront(B);
   while(index(A)>=0 && index(B)>=0){
      // vector mul
      EA = get(A); EB = get(B);
      if(EA->col == EB->col){
         result += (EA->value) * (EB->value);
         moveNext(A);
         moveNext(B);
      }else if(EA->col < EB->col){
         moveNext(A);
      }else{
         moveNext(B);
      }
   }

   return result;
}

Matrix product(Matrix A, Matrix B){
   if(A->size != B->size){
      fprintf(stderr, "Matrix error: \
         diff called on Matrixes with different size\n");
      exit(EXIT_FAILURE);
   }   
   Matrix M, BT;
   List LA, LBT, LM;
   Entry LME;
   int size;
   double value;

   BT = transpose(B);
   size = A->size;
   M = newMatrix(size);
   for(int i=1; i<=size; i++){
      LBT = BT->row[i]; 
      if(length(LBT)==0) continue;
      for(int j=1; j<=size; j++){
         LA = A->row[j];
         if(length(LA)==0) continue;
         value = vectorProd(LA, LBT);
         if(value){
            LME = newEntry(i, value);
            LM = M->row[j];
            append(LM, LME);
         } 
      }
   }
   freeMatrix(&BT);
   return M;
}


void printMatrix(FILE* out, Matrix M){
   for(int i=1; i<=M->size; i++){
      List row = M->row[i];
      if(length(row)==0) continue;
      fprintf(out, "%d: ",i);
      moveFront(row);
      while(index(row)>=0){
         Entry E = get(row);
         fprintf(out, "(%d, %.1f) ", E->col, E->value);
         moveNext(row);
      }
      fprintf(out, "\n");
   }

}











