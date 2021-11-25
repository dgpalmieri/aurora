#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../view.hpp"
//#include "view.hpp"
#include "doctest.h"

#include <iostream>

TEST_CASE("View Tests"){

    // set_view_matrix tests

    double testMatrix[16] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0,
                             10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};

    set_view_matrix(testMatrix);
    // Is there a way to access the xform and invrse matricies in the view file?
    // Ideally, this would be some kind of read-only global variable

    // Now the xform matrix is a 4x4 matrix, and the inverse matrix is
    // still the identity matrix

    // camera_xform tests
    // Because this function is composed entirely of two prints two MatrixPrints (which
    // are tested in test_rays.cpp), testing this function seems repeetitive and non-DRY

    SUBCASE("camera_xform tests"){
        Point testPoint = {1, 1, 1};
        Point retPoint = camera_xform(testPoint);

        CHECK(retPoint.x == 28);
        CHECK(retPoint.y == 32);
        CHECK(retPoint.z == 36);

        Point testPoint2 = {2, 2, 2};
        Point retPoint2 = camera_xform(testPoint2);

        CHECK(retPoint2.x == 43);
        CHECK(retPoint2.y == 50);
        CHECK(retPoint2.z == 57);
    }

    SUBCASE("inv_camera_xform"){
        Point testPoint = {1, 1, 1};
        Point retPoint = camera_xform(testPoint);

        CHECK(retPoint.x == 28);
        CHECK(retPoint.y == 32);
        CHECK(retPoint.z == 36);

        Point testPoint2 = {2, 2, 2};
        Point retPoint2 = camera_xform(testPoint2);

        CHECK(retPoint2.x == 43);
        CHECK(retPoint2.y == 50);
        CHECK(retPoint2.z == 57);
    }

    SUBCASE("vec_camera_xform tests"){
        Point vecTestPoint = {1, 1, 1};
        Point vecRetPoint = camera_xform(vecTestPoint);

        CHECK(vecRetPoint.x == 28);
        CHECK(vecRetPoint.y == 32);
        CHECK(vecRetPoint.z == 36);

        Point vecTestPoint2 = {2, 2, 2};
        Point vecRetPoint2 = camera_xform(vecTestPoint2);

        CHECK(vecRetPoint2.x == 43);
        CHECK(vecRetPoint2.y == 50);
        CHECK(vecRetPoint2.z == 57);
    }

    SUBCASE("vec_inv_camera_xform"){}

    SUBCASE("camera_lookup tests"){}

}
