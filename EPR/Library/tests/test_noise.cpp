#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <iostream>

#include "doctest.h"
#include "../noise.hpp"


// Function to scale double values to integer
// used for comparison.
int scale_double(double num){
    double scale = 0.0001;
    return (int)round((int)(num/scale) * scale * 10000);
}

TEST_CASE("Testing noise: Simple test"){
    srand(10);
    initnoise();
    Vector v;

    double n = noise(v);
    REQUIRE(v.x == 0);
    REQUIRE(v.y == 0);
    REQUIRE(v.z == 0);
    REQUIRE(scale_double(n) == 5800);

    SUBCASE("Testing noise(...): Take a vector and return a noise value."){
        Vector v_1 = {10,5,9};
        Vector v_2 = {5,15,0};
        Vector v_3 = {5,10,20};
        auto n_1 = noise(v_1);
        auto n_2 = noise(v_2);
        auto n_3 = noise(v_3);

        CHECK(scale_double(n_1) == 5542);
        CHECK(scale_double(n_2)  == 6241);
        CHECK(scale_double(n_3)  == 4597);
    }
    SUBCASE("Testing Dnoise(...): Take a vector and return a vector with noise value."){
        Vector v_1 = {10,5,9};
        Vector v_2 = {5,15,0};
        Vector v_3 = {5,10,20};

        auto dnv_1 = Dnoise(v_1);
        auto dnv_2 = Dnoise(v_2);
        auto dnv_3 = Dnoise(v_3);

        CHECK(scale_double(dnv_1.x) == -1798);
        CHECK(scale_double(dnv_1.y) == 1720);
        CHECK(scale_double(dnv_1.z) == -1616);

        CHECK(scale_double(dnv_2.x) == 1062);
        CHECK(scale_double(dnv_2.y) == -1306);
        CHECK(scale_double(dnv_2.z) == 4596);

        CHECK(scale_double(dnv_3.x) == 438);
        CHECK(scale_double(dnv_3.y) == -1305);
        CHECK(scale_double(dnv_3.z) == -4937);
    }
    SUBCASE("Testing turbulence(...): Take a four values and return a turbulence value."){
        double x_1 = 10.0, y_1 = 2.0, z_1 = 4.0, size_1 = 5.0;
        double x_2 = 5.0, y_2 = 6.0, z_2 = 14.0, size_2 = 2.0;
        double x_3 = 3.0, y_3 = 3.0, z_3 = 6.0, size_3 = 9.0;

        auto t_1 = turbulence(x_1,y_1,z_1,size_1);
        auto t_2 = turbulence(x_2,y_2,z_2,size_2);
        auto t_3 = turbulence(x_3,y_3,z_3,size_3);

        CHECK(scale_double(t_1) == 4795);
        CHECK(scale_double(t_2) == 3727);
        CHECK(scale_double(t_3) == 5146);
    }
}
