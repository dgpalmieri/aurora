#include "rays.h"
#include "octree.h"
#include <stdio.h>
#include <stdlib.h>

typedef Point stackElement;

typedef struct _stackNode {
   stackElement min;
   stackElement max;
   OCTREE *octptr;
   struct _stackNode *next;
} stackNode;

typedef struct {
   int size;
   stackNode *top;
} stackTop;

typedef struct {
   stackElement min;
   stackElement max;
   OCTREE *octptr;
} stackData;

void stackpush(stackTop *theStack, OCTREE *o, stackElement min, stackElement max);
int  stackpop (stackTop *theStack, stackData *topData);
void stackinit(stackTop *theStack);
void stackdestroy(stackTop *theStack);
