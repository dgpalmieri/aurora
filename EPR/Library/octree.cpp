#include "octree.hpp"

void octree_init(OCTREE *o, int deep) {
    o->xLoc = 0x00000000;
    o->yLoc = 0x00000000;
    o->zLoc = 0x00000000;
    o->depth = 0;
    o->parent = NULL;
    o->child = NULL;
    o->data = NULL;
    if (deep==0) return; // setting a blank node
    int pow2test = 1;
    int testconfirm = 0;
    int orgsize = deep;
    while ((deep=(deep >> 1)) > 0) { oct_depth += 1; pow2test *= 2; }
    original_oct_depth = oct_depth;
    if ((orgsize-pow2test)>0) { 
        while ((pow2test=(pow2test >> 1)) > 0) { testconfirm += 1; } 
        oct_depth = testconfirm+1;
        std::cerr << " Supergrid not a power of 2.  Adjust oct_depth to:" << oct_depth << std::endl;
    }
    num_children = 0;
}

void binary( Loc byte ) {
    int count=oct_depth+1;
    int MASK = 1<<(count-1);
    while(count--) {
        std::cout << (( byte & MASK ) ? 1 : 0) << std::endl;
        byte <<= 1;
    }
} 

OCTREE *octree_insert(OCTREE *node, int nextLevel, Loc xLoc, Loc yLoc, Loc zLoc){
    int i;   
    int childLevel = nextLevel - 1;
    int testbit = 1 << childLevel;
    int datachild = (xLoc & testbit) + ((yLoc & testbit)<<1) + ((zLoc & testbit)<<2) >> childLevel;   
    if (node->child == NULL && nextLevel != 0) {
        if ((node->child = (OCTREE *) malloc(8*sizeof(OCTREE))) == NULL) { 
            double subblocksize = sizeof(short) * sub_num_samples;
            double octree_size = (double)(((num_children+1)*sizeof(OCTREE))/1048576.0);
            double subblocks_size = (double)(num_sub_allocated*subblocksize/1048576.0); 
            std::cerr << "ERROR: Can't malloc child: " << num_children << "at " << node->xLoc << node->yLoc << node->zLoc << ", level: " << node->depth << std::endl;
            std::cerr << "   Octree size: " << octree_size << " MB, Subblocks size: " << subblocks_size << " MB, Total: " << subblocks_size + octree_size << "MB" << std::endl;
            std::cerr << "deallocating octree and subblocks..." << std::endl;
            octree_destroy(root);
            exit(0);
        }
        for (i = 0; i < 8; i++) {
            num_children++;
            node->child[i].child  = NULL;
            node->child[i].data   = NULL;
            node->child[i].parent = node;
            node->child[i].depth  = node->depth + 1;
        }
        node->child[0].xLoc = node->xLoc;
        node->child[0].yLoc = node->yLoc;
        node->child[0].zLoc = node->zLoc;
        node->child[1].xLoc = node->xLoc | (1 << childLevel);
        node->child[1].yLoc = node->yLoc;
        node->child[1].zLoc = node->zLoc;
        node->child[2].xLoc = node->xLoc;
        node->child[2].yLoc = node->yLoc | (1 << childLevel);
        node->child[2].zLoc = node->zLoc;
        node->child[3].xLoc = node->xLoc | (1 << childLevel);
        node->child[3].yLoc = node->yLoc | (1 << childLevel);
        node->child[3].zLoc = node->zLoc;
        node->child[4].xLoc = node->xLoc;
        node->child[4].yLoc = node->yLoc;
        node->child[4].zLoc = node->zLoc | (1 << childLevel);
        node->child[5].xLoc = node->xLoc | (1 << childLevel);
        node->child[5].yLoc = node->yLoc;
        node->child[5].zLoc = node->zLoc | (1 << childLevel);
        node->child[6].xLoc = node->xLoc;
        node->child[6].yLoc = node->yLoc | (1 << childLevel);
        node->child[6].zLoc = node->zLoc | (1 << childLevel);
        node->child[7].xLoc = node->xLoc | (1 << childLevel);
        node->child[7].yLoc = node->yLoc | (1 << childLevel);
        node->child[7].zLoc = node->zLoc | (1 << childLevel);
    }

    if (node->child[datachild].xLoc == xLoc && node->child[datachild].yLoc == yLoc && node->child[datachild].zLoc == zLoc && node->child[datachild].depth == oct_depth) {
        /* We found the child we want, so set child to node and return node */
        return &node->child[datachild];
    } 
    else {
        /* Follow to correct child */
        if(node->child[datachild].depth <= oct_depth){
            octree_insert(&node->child[datachild], childLevel, xLoc, yLoc, zLoc);
        }
        else { 
            std::cerr << "ERROR: " << xLoc << yLoc << zLoc << " traversed too deep on insert. DEPTH: " << node->child[datachild].depth << std::endl;
            exit(1); 
        }
    }
}

OCTREE *octree_traverse(OCTREE *node, int nextLevel, Loc xLoc, Loc yLoc, Loc zLoc){
    int childLevel = nextLevel - 1;
    int testbit = 1 << childLevel;
    int datachild = (xLoc & testbit) + ((yLoc & testbit)<<1) + ((zLoc & testbit)<<2) >> childLevel;
    if (!node->child) { return &blanknode; } 
    if (node->child[datachild].xLoc == xLoc && node->child[datachild].yLoc == yLoc && node->child[datachild].zLoc == zLoc && node->child[datachild].depth == oct_depth) {
        return &node->child[datachild];
    } else {
        if(node->child[datachild].depth <= oct_depth)
            octree_traverse(&node->child[datachild], childLevel, xLoc, yLoc, zLoc);
        else { 
            std::cerr << "ERROR: traversed too deep on traverse." << std::endl;
            exit(1);  
        }
    }
}

void octree_destroy(OCTREE *o) {
   int i;
   for (i=0; i<8; i++) {
      if (o->child) octree_destroy(&o->child[i]);
   }
   if (o->depth == oct_depth) free(o->data);
   if (o->child != NULL) free(o->child);
   return;
}
