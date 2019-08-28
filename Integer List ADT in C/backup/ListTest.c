#include <stdio.h>
#include "List.h"

int main(void){
	List A = newList();

prepend(A, 76);
        prepend(A, 4);
        deleteFront(A);
        prepend(A, 3);
        prepend(A, 1);
        moveFront(A);
        insertBefore(A, 115);
        deleteFront(A);
printf("%d\n",length(A));
        if (length(A) != 3) return 1;


















      
        freeList(&A);
        return 0;
}

