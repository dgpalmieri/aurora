#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "raster.hpp"

TEST_CASE("Testing screen struct from raster.hpp") {

//holds raster/window information
    Screen s;

//set defaults for Screen data structure
    s.xmin = -0.045;
    s.xmax =  0.045;
    s.ymin = -0.045;
    s.ymax =  0.045;
    s.near =  0.1;
    s.far  =  5.0;
    s.xres =  400;
    s.yres =  400;

    REQUIRE(s.xmin == -0.045);
    REQUIRE(s.xmax == 0.045);
    REQUIRE(s.ymin == -0.045);
    REQUIRE(s.ymax == 0.045);
    REQUIRE(s.near == 0.1);
    REQUIRE(s.far == 5.0);
    REQUIRE(s.xres == 400);
    REQUIRE(s.yres == 400);

//implemented via doctest per suggestion of @mattp0 and @dgpalmieri
//and following excellent examples from @ETY-13 and @TailonR
    SUBCASE("Screen is a data structure of two integers and six doubles.") {

        CHECK(sizeof(int) == 4);
        CHECK(sizeof(double) == 8);
        CHECK(sizeof(s) == (2*4)+(6*8));
        CHECK((2*sizeof(int) + 6*sizeof(double)) == sizeof(s));

    }

}

TEST_CASE("Testing pixel struct from raster.hpp") {
    //holds the values for a pixel at highest accuracy
    Pixel p;

    p.r = 33.33;
    p.g = 33.33;
    p.b = 33.33;
    p.a = 33.33;

    REQUIRE(p.r == 33.33);
    REQUIRE(p.g == 33.33);
    REQUIRE(p.b == 33.33);
    REQUIRE(p.a == 33.33);

    SUBCASE("Pixel is a data structure of four doubles.") {

        CHECK(sizeof(p) == (4*8));

    }

}
