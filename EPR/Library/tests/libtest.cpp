#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <iostream>
#include <string>
#include "../err_msg.hpp"
#include "../octree.hpp"

/* Octree information */
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
int original_oct_depth; //allows for non power of 2 supers
unsigned int num_children; /* number of nodes created by octree */

/* Counters used for volume stats */
unsigned int        sub_num_samples; //declared extern in octree.h
static unsigned int num_inserts = 0; /* number of insertions, ==0, skip render */
unsigned int        num_sub_allocated = 0;
static unsigned int num_overwrites = 0;

TEST_CASE("error codes output correct strings"){
    std::string file = "thing.txt";
    int error = 1;
    auto fileobj = FileError(file, error);
    CHECK(fileobj.getErrorCode() == error);
    CHECK(fileobj.getFileName() == file);
}

TEST_CASE("OCTREE testing"){
    CHECK(num_inserts == 0);
    CHECK(num_overwrites == 0);
}