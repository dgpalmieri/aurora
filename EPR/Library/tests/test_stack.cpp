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

    // Tests for stackpush

    stackpush(my_stack, octreeOne, elemOneMin, elemOneMax);

    stackNode * stackTop = my_stack->top;
    REQUIRE(stackTop->octptr == octreeOne);
    REQUIRE(stackTop->min.x == elemOneMin.x);
    REQUIRE(stackTop->min.y == elemOneMin.y);
    REQUIRE(stackTop->min.z == elemOneMin.z);
    REQUIRE(stackTop->max.x == elemOneMax.x);
    REQUIRE(stackTop->max.y == elemOneMax.y);
    REQUIRE(stackTop->max.z == elemOneMax.z);

    stackpush(my_stack, octreeTwo, elemTwoMin, elemTwoMax);

    stackTop = my_stack->top;
    REQUIRE(stackTop->octptr == octreeTwo);
    REQUIRE(stackTop->min.x == elemTwoMin.x);
    REQUIRE(stackTop->min.y == elemTwoMin.y);
    REQUIRE(stackTop->min.z == elemTwoMin.z);
    REQUIRE(stackTop->max.x == elemTwoMax.x);
    REQUIRE(stackTop->max.y == elemTwoMax.y);
    REQUIRE(stackTop->max.z == elemTwoMax.z);

    stackpush(my_stack, octreeThree, elemThreeMin, elemThreeMax);

    stackTop = my_stack->top;
    REQUIRE(stackTop->octptr == octreeThree);
    REQUIRE(stackTop->min.x == elemThreeMin.x);
    REQUIRE(stackTop->min.y == elemThreeMin.y);
    REQUIRE(stackTop->min.z == elemThreeMin.z);
    REQUIRE(stackTop->max.x == elemThreeMax.x);
    REQUIRE(stackTop->max.y == elemThreeMax.y);
    REQUIRE(stackTop->max.z == elemThreeMax.z);

    // Tests for stackpop

    stackData * poppedData = new stackData;
    stackpop(my_stack, poppedData);
    stackTop = my_stack->top;

    REQUIRE(poppedData->octptr == octreeThree);
    REQUIRE(poppedData->min.x == elemThreeMin.x);
    REQUIRE(poppedData->min.y == elemThreeMin.y);
    REQUIRE(poppedData->min.z == elemThreeMin.z);
    REQUIRE(poppedData->max.x == elemThreeMax.x);
    REQUIRE(poppedData->max.y == elemThreeMax.y);
    REQUIRE(poppedData->max.z == elemThreeMax.z);
    REQUIRE(stackTop->octptr == octreeTwo);
    REQUIRE(stackTop->min.x == elemTwoMin.x);
    REQUIRE(stackTop->min.y == elemTwoMin.y);
    REQUIRE(stackTop->min.z == elemTwoMin.z);
    REQUIRE(stackTop->max.x == elemTwoMax.x);
    REQUIRE(stackTop->max.y == elemTwoMax.y);
    REQUIRE(stackTop->max.z == elemTwoMax.z);

    stackpop(my_stack, poppedData);
    stackTop = my_stack->top;

    REQUIRE(poppedData->octptr == octreeTwo);
    REQUIRE(poppedData->min.x == elemTwoMin.x);
    REQUIRE(poppedData->min.y == elemTwoMin.y);
    REQUIRE(poppedData->min.z == elemTwoMin.z);
    REQUIRE(poppedData->max.x == elemTwoMax.x);
    REQUIRE(poppedData->max.y == elemTwoMax.y);
    REQUIRE(poppedData->max.z == elemTwoMax.z);
    REQUIRE(stackTop->octptr == octreeOne);
    REQUIRE(stackTop->min.x == elemOneMin.x);
    REQUIRE(stackTop->min.y == elemOneMin.y);
    REQUIRE(stackTop->min.z == elemOneMin.z);
    REQUIRE(stackTop->max.x == elemOneMax.x);
    REQUIRE(stackTop->max.y == elemOneMax.y);
    REQUIRE(stackTop->max.z == elemOneMax.z);

    stackpop(my_stack, poppedData);
    stackTop = my_stack->top;

    REQUIRE(poppedData->octptr == octreeOne);
    REQUIRE(poppedData->min.x == elemOneMin.x);
    REQUIRE(poppedData->min.y == elemOneMin.y);
    REQUIRE(poppedData->min.z == elemOneMin.z);
    REQUIRE(poppedData->max.x == elemOneMax.x);
    REQUIRE(poppedData->max.y == elemOneMax.y);
    REQUIRE(poppedData->max.z == elemOneMax.z);
    REQUIRE(stackTop == nullptr);

    stackdestroy(my_stack);
    delete(my_stack);
    delete(poppedData);
}
