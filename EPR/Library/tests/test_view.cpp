#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../view.hpp"
//#include "view.hpp"
#include "doctest.h"

#include <iostream>

TEST_CASE("View Tests"){

    // set_view_matrix tests

    double zeroMatrix[16] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    set_view_matrix(zeroMatrix);
    // Is there a way to access the xform and invrse matricies in the view file?
    // Ideally, this would be some kind of read-only global variable

    // Now the xform matrix is a 4x4 matrix of zeros, and the inverse matrix is
    // still the identity matrix

    // camera_xform tests
    // Because this function is composed entirely of two prints two MatrixPrints (which
    // are tested in test_rays.cpp), testing this function seems repeetitive and non-DRY

    // camera_xform tests

    Point testPoint = {1, 1, 1};
    Point retPoint = camera_xform(testPoint);

    REQUIRE(retPoint.x == 0);
    REQUIRE(retPoint.y == 0);
    REQUIRE(retPoint.z == 0);

    Point testPoint2 = {2, 2, 2};
    Point retPoint2 = camera_xform(testPoint2);

    REQUIRE(retPoint2.x == 0);
    REQUIRE(retPoint2.y == 0);
    REQUIRE(retPoint2.z == 0);

    // inv_camera_xform

    Point invTestPoint = {1, 1, 1};
    Point invRetPoint = camera_xform(testPoint);

    REQUIRE(invRetPoint.x == 1);
    REQUIRE(invRetPoint.y == 1);
    REQUIRE(invRetPoint.z == 1);

    Point invTestPoint2 = {2, 2, 2};
    Point invRetPoint2 = camera_xform(testPoint2);

    REQUIRE(invRetPoint2.x == 2);
    REQUIRE(invRetPoint2.y == 2);
    REQUIRE(invRetPoint2.z == 2);

    // vec_camera_xform tests

    // vec_inv_camera_xform

    // camera_lookup tests

}
