#include <stdio.h>
#include <stdlib.h>

typedef unsigned short Loc;

typedef struct _OCTREE {
   Loc xLoc;
   Loc yLoc;
   Loc zLoc;
   Loc depth;
   struct _OCTREE *parent;
   struct _OCTREE *child;
   short *data;
} OCTREE;

extern int oct_depth;    /* how deep the octree will be */
extern int original_oct_depth; /* for non-power of 2 supers */
extern unsigned int num_children; /* number of nodes created by octree */
extern unsigned int num_sub_allocated;
extern unsigned int sub_num_samples;
extern OCTREE *root;
extern OCTREE blanknode;
void octree_init(OCTREE *o, int deep);
void binary(Loc byte);
OCTREE * octree_insert(OCTREE *node, int nextLevel, Loc xLoc, Loc yLoc, Loc zLoc);
OCTREE * octree_traverse(OCTREE *node, int nextLevel, Loc xLoc, Loc yLoc, Loc zLoc);
void octree_destroy(OCTREE *o);
