// test_view.cpp
// Author: Dylan Palmieri @dgpalmieri

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "view.hpp"
#include "doctest.h"

#include <iostream>
#include <vector>

TEST_CASE("View Tests"){

    // set_view_matrix tests

    std::vector<double> testMatrix = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0,
                                   10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0};

    setViewMatrix(testMatrix);
    // Is there a way to access the viewMatrix and inverseViewMatrix variables in the view file?
    // Ideally, this would be some kind of read-only global variable

    // Now the view matrix is a 4x4 matrix, and the inverse matrix is
    // still the identity matrix

    // outputCamera tests
    // Because this function is composed entirely of two prints and two MatrixPrints (which
    // are tested in test_rays.cpp), testing this function seems repetitive and non-DRY

    SUBCASE("cameraTransform tests"){
        Point testPoint = {1, 1, 1};
        Point retPoint = cameraTransform(testPoint);

        REQUIRE(retPoint.x == 28);
        REQUIRE(retPoint.y == 32);
        REQUIRE(retPoint.z == 36);

        Point testPoint2 = {2, 2, 2};
        Point retPoint2 = cameraTransform(testPoint2);

        REQUIRE(retPoint2.x == 43);
        REQUIRE(retPoint2.y == 50);
        REQUIRE(retPoint2.z == 57);
    }

    SUBCASE("invCameraTransform"){
        Point testPoint = {1, 1, 1};
        Point retPoint = invCameraTransform(testPoint);

        REQUIRE(retPoint.x == 1.0);
        REQUIRE(retPoint.y == 1.0);
        REQUIRE(retPoint.z == 1.0);

        Point testPoint2 = {2, 2, 2};
        Point retPoint2 = invCameraTransform(testPoint2);

        REQUIRE(retPoint2.x == 2.0);
        REQUIRE(retPoint2.y == 2.0);
        REQUIRE(retPoint2.z == 2.0);
    }

    SUBCASE("vecCameraTransform tests"){
        Point vecTestPoint = {1, 1, 1};
        Point vecRetPoint = vecCameraTransform(vecTestPoint);

        REQUIRE(vecRetPoint.x == 15.0);
        REQUIRE(vecRetPoint.y == 18.0);
        REQUIRE(vecRetPoint.z == 21.0);

        Point vecTestPoint2 = {2, 2, 2};
        Point vecRetPoint2 = vecCameraTransform(vecTestPoint2);

        REQUIRE(vecRetPoint2.x == 30.0);
        REQUIRE(vecRetPoint2.y == 36.0);
        REQUIRE(vecRetPoint2.z == 42.0);
    }

    SUBCASE("vec_inv_camera_xform"){
        Point vecTestPoint = {1, 1, 1};
        Point vecRetPoint = vecInvCameraTransform(vecTestPoint);

        REQUIRE(vecRetPoint.x == 1.0);
        REQUIRE(vecRetPoint.y == 1.0);
        REQUIRE(vecRetPoint.z == 1.0);

        Point vecTestPoint2 = {2, 2, 2};
        Point vecRetPoint2 = vecInvCameraTransform(vecTestPoint2);

        REQUIRE(vecRetPoint2.x == 2.0);
        REQUIRE(vecRetPoint2.y == 2.0);
        REQUIRE(vecRetPoint2.z == 2.0);
    }

    SUBCASE("camera_lookup tests"){
        REQUIRE(cameraLookup(0,0) == 1.0);
        REQUIRE(cameraLookup(0,1) == 2.0);
        REQUIRE(cameraLookup(0,2) == 3.0);
        REQUIRE(cameraLookup(0,3) == 4.0);
        REQUIRE(cameraLookup(1,0) == 5.0);
        REQUIRE(cameraLookup(1,1) == 6.0);
    }
}
