/* test_stack.cpp
 * author: Dylan Palmieri (@dgpalmieri)
 * 2021-11-17
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../stack.hpp"
//#include "stack.hpp"
#include "doctest.h"
#include <iostream>

// Octree information - needs to be present in order to compile
OCTREE octree;
OCTREE blanknode;
OCTREE *root = &octree;
OCTREE *x_node;
OCTREE *y_node;
OCTREE *xy_node;
OCTREE *z_node;
OCTREE *xz_node;
OCTREE *yz_node;
OCTREE *xyz_node;
int oct_depth;
int original_oct_depth; // allows for non power of 2 supers
unsigned int num_children; // number of nodes created by octree
unsigned int        sub_num_samples; // declared extern in octree.h
unsigned int        num_sub_allocated = 0;


TEST_CASE("Stack Tests"){
    stackTop * my_stack = new stackTop;
    stackinit(my_stack);

    OCTREE * octreeOne = new OCTREE;
    stackElement elemOneMin, elemOneMax;
    elemOneMin = (stackElement){.x = 0.0, .y = 0.0, .z = 0.0};
    elemOneMax = (stackElement){.x = 0.5, .y = 0.5, .z = 0.5};
    octree_init(octreeOne, 0);

    OCTREE * octreeTwo = new OCTREE;
    stackElement elemTwoMin, elemTwoMax;
    elemTwoMin = (stackElement){.x = 1.0, .y = 1.0, .z = 1.0};
    elemTwoMax = (stackElement){.x = 1.5, .y = 1.5, .z = 1.5};
    octree_init(octreeTwo, 0);

    OCTREE * octreeThree = new OCTREE;
    stackElement elemThreeMin, elemThreeMax;
    elemThreeMin = (stackElement){.x = 2.0, .y = 2.0, .z = 2.0};
    elemThreeMax = (stackElement){.x = 2.5, .y = 2.5, .z = 2.5};
    octree_init(octreeThree, 0);

    stackNode * test = new stackNode;
    stackpush(my_stack, octreeOne, elemOneMin, elemOneMax);
    test = my_stack->top;

    REQUIRE(test->octptr == octreeOne);

    REQUIRE(test->min.x == elemOneMin.x);
    REQUIRE(test->min.y == elemOneMin.y);
    REQUIRE(test->min.z == elemOneMin.z);

    REQUIRE(test->max.x == elemOneMax.x);
    REQUIRE(test->max.y == elemOneMax.y);
    REQUIRE(test->max.z == elemOneMax.z);

    stackpush(my_stack, octreeTwo, elemTwoMin, elemTwoMax);
    stackpush(my_stack, octreeThree, elemThreeMin, elemThreeMax);

    stackdestroy(my_stack);
    delete(test);
    delete(my_stack);

}
