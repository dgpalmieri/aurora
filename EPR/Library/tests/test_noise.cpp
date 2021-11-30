#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../noise.hpp"

TEST_CASE("Testing noise: Simple test") {
    initnoise();
    Vector v;
    REQUIRE(v.x == 0);
    REQUIRE(v.y == 0);
    REQUIRE(v.z == 0);

    SUBCASE("Testing noise(...): Take a vector and return a noise value.") {
        v = { 10,5,9 };
        auto n = noise(v);

        CHECK(n < 1);
    }
    SUBCASE("Testing Dnoise(...): Take a vector and return a vector with noise value.") {
        auto dnv = Dnoise(v);

        CHECK(dnv.x < 1);
        CHECK(dnv.y < 1);
        CHECK(dnv.z < 1);
    }
    SUBCASE("Testing turbulence(...): Take four values and return a turbulence value.") {
        double x = 10.0, y = 2.0, z = 4.0, size = 5.0;
        auto t = turbulence(x, y, z, size);

        CHECK(t < 1);
    }
    SUBCASE("All functions retain the same value for all calls.") {
        // The randomness in Noise is specific to each Noise object; not to each call of
        // Noise functions.
        double x = 10.0, y = 2.0, z = 4.0, size = 5.0;

        auto dnv = Dnoise(v);
        auto dnv2 = Dnoise(v);

        CHECK(noise(v) == noise(v));
        CHECK(turbulence(x, y, z, size) == turbulence(x, y, z, size));
        CHECK(dnv.x == dnv2.x);
        CHECK(dnv.y == dnv2.y);
        CHECK(dnv.z == dnv2.z);
    }
    // This test case is only if using C++17 implementation of Noise.
    // Comments this part out if testing c implementation of Noise.
    SUBCASE("C++17 version of Noise: All functions retain the same value for all calls.") {
        Noise n;
        double x = 10.0, y = 2.0, z = 4.0, size = 5.0;

        auto dnv = n.Dnoise(v);
        auto dnv2 = n.Dnoise(v);

        CHECK(n.noise(v) == n.noise(v));
        CHECK(n.turbulence(x, y, z, size) == n.turbulence(x, y, z, size));
        CHECK(dnv.x == dnv2.x);
        CHECK(dnv.y == dnv2.y);
        CHECK(dnv.z == dnv2.z);
    }
}
