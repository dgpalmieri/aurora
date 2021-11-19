/**
 ** C stack implementation by J.R. Manes. (jr@jrmanes.com)
 ** This keeps track of an origin point and direction vector (see stack.h)
 **/

#include "stack.hpp"

void stackinit(stackTop *theStack) {
   theStack->top = NULL; 
   theStack->size = 0;
}

void stackpush(stackTop *theStack, OCTREE *o, stackElement min, stackElement max) {
  stackNode *newnode;
  if ((newnode = (stackNode *) malloc(sizeof(stackNode))) == NULL) {
     fprintf(stderr, "ERROR: Stack full! (number of elements: %d)\n",theStack->size);
     exit(0);
  }
  newnode->min.x = min.x; 
  newnode->min.y = min.y; 
  newnode->min.z = min.z;
  newnode->max.x = max.x; 
  newnode->max.y = max.y; 
  newnode->max.z = max.z;
  newnode->octptr = o;
  newnode->next =  theStack->top;
  theStack->top = newnode;
  theStack->size++;
}

int stackpop(stackTop *theStack, stackData *topData ) {
   if (theStack->size < 1) return 0;
   stackNode *deletePtr = theStack->top;
   topData->min.x = theStack->top->min.x;
   topData->min.y = theStack->top->min.y;
   topData->min.z = theStack->top->min.z;
   topData->max.x = theStack->top->max.x;
   topData->max.y = theStack->top->max.y;
   topData->max.z = theStack->top->max.z;
   topData->octptr = theStack->top->octptr;
   theStack->top = theStack->top->next;
   theStack->size--;
   free(deletePtr);
   return 1;
}

void stackdestroy(stackTop *theStack) {
   stackNode *delPtr;
   while (theStack->top) {
      delPtr = theStack->top;
      theStack->top = theStack->top->next;
      free(delPtr);
   }
}
