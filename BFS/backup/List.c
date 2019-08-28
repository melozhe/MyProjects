//----------------------------------------------------------------
// List.c
// Implementation file for List ADT
//----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "List.h"


// private types and functions
typedef struct NodeObj{
   int data;
   struct NodeObj* prev;
   struct NodeObj* next;
}NodeObj;

typedef NodeObj* Node;

typedef struct ListObj{
   Node front;
   Node back;
   Node cursor;
   int index;
   int numitems;
}ListObj;

// ctor and dtor of NodeObj
// Node ctor
Node newNode(int node_data){
   Node N = malloc(sizeof(NodeObj));
   assert(N!=NULL);
   N->data = node_data;
   N->next = NULL;
   N->prev = NULL;
   return N;
}
// Node dtor
void freeNode(Node* pN){
   if(pN != NULL && *pN != NULL){
      free(*pN);
      *pN = NULL;
   }
}

// public ADT operations
// ctor and dtor of ListObj
List newList(){
   List L = malloc(sizeof(ListObj));
   assert(L!=NULL);
   // Create dummy front and back, and hook them up
   L->front = newNode(UNDEF);
   L->back = newNode(UNDEF);
   L->front->next = L->back;
   L->front->prev = NULL;
   L->back->next = NULL;
   L->back->prev = L->front;
   // Initialize other fields in List
   L->cursor = NULL;
   L->index = UNDEF;
   L->numitems = 0; 
   return L; 
}

void freeList(List* pL){
   if(pL != NULL && *pL != NULL){
      clear(*pL);
      freeNode(&((*pL)->front));
      freeNode(&((*pL)->back));
      free(*pL);
      *pL = NULL;	
   }
}

// Access Functions
int length(List L){
   if(!L){
      fprintf(stderr, "List error: calling length() on NULL List reference\n");
      exit(EXIT_FAILURE);
}

   return L->numitems;
}

int index(List L){
   if(!L){
      fprintf(stderr, "List error: calling index() on NULL List reference\n");
      exit(EXIT_FAILURE);
   }
   
   if(!L->cursor)
      L->index = UNDEF;

   return L->index;
}

int front(List L){
   return L->front->next->data;
}

int back(List L){
   return L->back->prev->data;
}

int get(List L){
   if(L->numitems==0||L->index==UNDEF){
      fprintf(stderr, "List error: calling get() on empty list or undefined cursor\n");
      exit(EXIT_FAILURE);
   }
   return L->cursor->data;
}

int cursor(List L){
   return L->cursor->data;
}

// int equals(List A, List B)
int equals(List A, List B){
   Node M,N;
   int eq = 0;
   eq = (A->numitems==B->numitems);
   M = A->front->next;
   N = B->front->next;
   while(eq && M != A->back){
      eq = (M->data == N->data);
      M= M->next;
      N= N->next;
   }
   return eq;
}
// Manipulation procedures
void clear(List L){
   Node F = L->front;
   Node B = L->back;
   Node N;
   while(F->next!=B){
      N = F->next;
      F->next=F->next->next;
      freeNode(&N);
   }
   B->prev = F;
   L->numitems=0;
   L->cursor = NULL;
   L->index = UNDEF;
}

void moveFront(List L){
   // if L is empty
   if(L->numitems == 0){
      fprintf(stderr, "List error: calling moveFront() on empty List\n ");
      exit(EXIT_FAILURE);
   }
   L->cursor = L->front->next;
   L->index = 0;
}

void moveBack(List L){
   // if L is empty
   if(L->numitems == 0){
      fprintf(stderr, "List error: calling moveBack() on empty List\n ");
      exit(EXIT_FAILURE);
   }
   L->cursor = L->back->prev;
   L->index = L->numitems-1;
}

void movePrev(List L){
   if(!L->cursor)
      return;
   // Now we have a defined cursor
   if(L->cursor != L->front->next){
      L->cursor = L->cursor->prev;
      L->index--;
   }else{
      L->cursor = NULL;
   }
}

void moveNext(List L){
   if(!L->cursor)
      return;
   if(L->cursor != L->back->prev){
      L->cursor = L->cursor->next;
      L->index++;
   }else{
      L->cursor = NULL;
   }
}

void prepend(List L, int data){
   Node F = L->front;
   Node N = newNode(data);
   N->next = F->next;
   F->next = N;
   N->prev = F;
   N->next->prev = N;;
   L->numitems++;
   
   if(L->cursor >=0)
      L->index++;
}

void append(List L, int data){
   Node B = L->back;
   Node N = newNode(data);
   N->next = B;
   B->prev->next = N;
   N->prev = B->prev;
   B->prev = N;
   L->numitems++;
}


void insertBefore(List L, int data){
   Node N = newNode(data);
   if(L->numitems==0 || L->index==UNDEF)
      return;
   // now we have a valid state to insert before
   N->next = L->cursor;
   L->cursor->prev->next = N;
   N->prev = L->cursor->prev;
   L->cursor->prev = N;

   L->numitems++;
   L->index++; 
}

void insertAfter(List L, int data){
   Node N = newNode(data);
   if(L->numitems==0 || L->index==UNDEF)
      return;
   // now we have a valid state to insert before
   N->next = L->cursor->next;
   L->cursor->next = N;
   N->prev = L->cursor;
   N->next->prev = N;
   
   L->numitems++;
}

void deleteFront(List L){
   //Update cursor
   if(L->cursor==L->front->next)
      L->cursor = NULL;
   else if(L->cursor)
      L->index--;
   Node N = L->front->next;
   L->front->next = N->next;
   N->next->prev = L->front;
   freeNode(&N);
   L->numitems--;
}

void deleteBack(List L){
   //Update cursor
   if(L->cursor==L->back->prev)
      L->cursor = NULL;
   Node N = L->back->prev;
   L->back->prev = N->prev;
   N->prev->next = L->back;
   freeNode(&N);
   L->numitems--;
}

void delete(List L){
   if(!L->cursor || L->numitems==0){
      fprintf(stderr,"List error: calling delete() on undefined cursor or emptyList\n");
      exit(EXIT_FAILURE);
   }
   L->cursor->prev->next = L->cursor->next;
   L->cursor->next->prev = L->cursor->prev;
   freeNode(&(L->cursor));
   L->numitems--;
}

// Other operations
void printList(FILE* out, List L){
   Node N;
   for(N=L->front->next;N!=L->back;N=N->next){
      fprintf(out," %d", N->data);
   }
}


List copyList(List L){
   List T = newList();
   Node N;
   for(N=L->front->next;N!=L->back;N=N->next){
      append(T,N->data);
   }
   return T;
}











































