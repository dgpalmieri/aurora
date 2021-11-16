#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "../rays.hpp"

#include<iostream>
using std::ostream;

#define TESTING_CPP_VERSION true // Comments this if testing c version.

TEST_CASE("Testing rays.h: Simple function calls") {

    Vector simple_v;
    simple_v.x = 1;
    simple_v.y = 1;
    simple_v.z = 1;

    REQUIRE(simple_v.x == 1);
    REQUIRE(simple_v.y == 1);
    REQUIRE(simple_v.z == 1);

    SUBCASE("negate should change the sign of the vector.") {
        simple_v = negate(simple_v);

        CHECK(simple_v.x == -1);
        CHECK(simple_v.y == -1);
        CHECK(simple_v.z == -1);

        simple_v = negate(simple_v);

        CHECK(simple_v.x == 1);
        CHECK(simple_v.y == 1);
        CHECK(simple_v.z == 1);
    }

    SUBCASE("add combine two Vector together by adding each x, y, z axis."){
        simple_v = add(simple_v,simple_v);

        CHECK(simple_v.x == 2);
        CHECK(simple_v.y == 2);
        CHECK(simple_v.z == 2);

        simple_v = add(negate(simple_v),negate(simple_v));

        CHECK(simple_v.x == -4);
        CHECK(simple_v.y == -4);
        CHECK(simple_v.z == -4);

    }

    SUBCASE("subtract(a,b) subtracts vector a from vector b along x, y, z axis"){
        simple_v = subtract(simple_v,simple_v);

        CHECK(simple_v.x == 0);
        CHECK(simple_v.y == 0);
        CHECK(simple_v.z == 0);

        Vector to_sub_v;
        to_sub_v.x = 2;
        to_sub_v.y = 2;
        to_sub_v.z = 2;

        simple_v = subtract(simple_v,to_sub_v);

        CHECK(simple_v.x == -2);
        CHECK(simple_v.y == -2);
        CHECK(simple_v.z == -2);

    }
    SUBCASE("multiply(c,v) multiple the vector v's x, y, z axis by constant c"){
        simple_v = multiply(2.0,simple_v);

        CHECK(simple_v.x == 2);
        CHECK(simple_v.y == 2);
        CHECK(simple_v.z == 2);

        simple_v = multiply(0.5,simple_v);

        CHECK(simple_v.x == 1);
        CHECK(simple_v.y == 1);
        CHECK(simple_v.z == 1);

    }

    SUBCASE("normalize normalizes the vector"){
        simple_v = normalize(simple_v);
        double scale = 0.001;

        CHECK(int(round((int)(simple_v.x / scale)*scale *1000)) == 577);
        CHECK(int(round((int)(simple_v.y / scale)*scale *1000)) == 577);
        CHECK(int(round((int)(simple_v.z / scale)*scale *1000)) == 577);

        simple_v.x = 4.0;
        simple_v.y = 10.0;
        simple_v.z = 3.0;

        simple_v = normalize(simple_v);

        scale = 0.00001;

        CHECK(int(round((int)(simple_v.x / scale)*scale *100000)) == 35777);
        CHECK(int(round((int)(simple_v.y / scale)*scale *100000)) == 89442);
        CHECK(int(round((int)(simple_v.z / scale)*scale *100000)) == 26832);
    }
    SUBCASE("cross(a,b) compute cross product of vector a and b") {

        simple_v = cross(simple_v, simple_v);

        CHECK(simple_v.x == 0);
        CHECK(simple_v.y == 0);
        CHECK(simple_v.z == 0);

        simple_v.x = 3.0;
        simple_v.y = 2.0;
        simple_v.z = 4.0;

        Vector cross_v;
        cross_v.x = 3.0;
        cross_v.y = 6.0;
        cross_v.z = 2.0;

        simple_v = cross(simple_v, cross_v);

        CHECK(simple_v.x == -20.0);
        CHECK(simple_v.y == 6.0);
        CHECK(simple_v.z == 12.0);
    }
    SUBCASE("dot_product(a,b) compute dot product of vector a and b") {

        double dot_result = dot_product(simple_v, simple_v);

        CHECK(dot_result == 3.0);

        Vector dot_v;
        dot_v.x = 3.0;
        dot_v.y = 6.0;
        dot_v.z = 2.0;

        dot_result = dot_product(simple_v, dot_v);

        CHECK(dot_result == 11.0);
    }
    SUBCASE("dist2(a,b) compute the squared distance of vector a and b") {

        double dist_result = dist2(simple_v, simple_v);

        CHECK(dist_result == 0.0);

        Vector dot_v;
        dot_v.x = 3.0;
        dot_v.y = 6.0;
        dot_v.z = 2.0;

        dist_result = dist2(simple_v, dot_v);

        CHECK(dist_result == 30.0);
    }
    SUBCASE("distance(a,b) compute the distance of vector a and b") {

        double dist_result = distance(simple_v, simple_v);

        CHECK(dist_result == 0.0);

        Vector dot_v;
        dot_v.x = 3.0;
        dot_v.y = 6.0;
        dot_v.z = 2.0;

        dist_result = distance(simple_v, dot_v);

        CHECK(round(dist_result) == 5.0);
    }
    SUBCASE("mag compute the magnitude of a vector ") {

        double mag_result = mag(simple_v);

        CHECK(round(mag_result) == 2.0);

        Vector dot_v;
        dot_v.x = 3.0;
        dot_v.y = 6.0;
        dot_v.z = 2.0;

        mag_result = mag(dot_v);

        CHECK(round(mag_result) == 7.0);
    }
    SUBCASE("create_vector creates a vector base on one vector to another") {

        Vector to_v;
        to_v.x = 3.0;
        to_v.y = 6.0;
        to_v.z = 2.0;

        simple_v = create_vector(simple_v,to_v);

        double scale = 0.00001;

        CHECK(int(round((int)(simple_v.x / scale)*scale *100000)) == 36514);
        CHECK(int(round((int)(simple_v.y / scale)*scale *100000)) == 91287);
        CHECK(int(round((int)(simple_v.z / scale)*scale *100000)) == 18257);
    }
    SUBCASE("make_ray creates a ray") {

        double x1 = 1.0, y1 = 2.0, z1 = 3.0,  x2 = 4.0, y2 = 5.0, z2 = 6.0;

        Ray simple_ray = make_ray(x1,y1,z1,x2,y2,z2);

        CHECK(simple_ray.org.x == 1.0);
        CHECK(simple_ray.org.y == 2.0);
        CHECK(simple_ray.org.z == 3.0);
        double scale = 0.000001;
        CHECK(int(round((int)(simple_ray.dir.x / scale)*scale *100000)) == 57735);
        CHECK(int(round((int)(simple_ray.dir.y / scale)*scale *100000)) == 57735);
        CHECK(int(round((int)(simple_ray.dir.z / scale)*scale *100000)) == 57735);
    }

    SUBCASE("testing function fcmp") {

        double a = 1.0, b = 2.0, c = 0.001, d = 0.001;
        int res = fcmp(a,b);
        CHECK(res == 0);

        res = fcmp(b,c);
        CHECK(res == 0);

        res = fcmp(c,d);
        CHECK(res == 1);
    }
    SUBCASE("MatrixIdentity computes the indentity matrix") {
        MATRIX simple_mat = {{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}};

        MatrixIdentity(simple_mat);
        CHECK(simple_mat[0][0] == 1);
        CHECK(simple_mat[1][1] == 1);
        CHECK(simple_mat[2][2] == 1);
        CHECK(simple_mat[3][3] == 1);
    }
    SUBCASE("inverse computes the inverse of two matrices") {
        MATRIX simple_mat = {{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}};
        int inv = inverse(simple_mat,simple_mat);

        CHECK(inv == 1);
        CHECK(simple_mat[0][0] == 0);
        CHECK(simple_mat[1][1] == 0);
        CHECK(simple_mat[2][2] == 0);
        CHECK(simple_mat[3][3] == 0);

        MATRIX inv_mat = {{3,1,3,4},{3,2,3,4},{8,2,6,4},{4,5,3,4}};
        inv = inverse(simple_mat,inv_mat);

        CHECK(inv == 1);
        CHECK(inv_mat[0][3] == 0);
        CHECK(inv_mat[1][2] == 0);
        CHECK(inv_mat[2][0] == 0);
        CHECK(inv_mat[3][1] == 0);

    }
    SUBCASE("test case for det2x2") {
        double a = 2.0, b = 3.0, c = 3.0, d = 4.0;
        CHECK(det2x2(a,b,c,d) == -1.0);

        a = 6.0, b = 8.0, c = 1.0, d = 14.0;
        CHECK(det2x2(a,b,c,d) == 76.0);
    }
    SUBCASE("test case for det3x3") {
        double a1 = 1.0, a2 = 2.0, a3 = 3.0;
        double b1 = 1.0, b2 = 2.0, b3 = 3.0;
        double c1 = 1.0, c2 = 2.0, c3 = 3.0;

        CHECK(det3x3(a1,a2,a3,b1,b2,b3,c1,c2,c3) == 0.0);

        a1 = 3.0, a2 = 1.0, a3 = 3.0;
        b1 = 5.0, b2 = 7.0, b3 = 3.0;
        c1 = 6.0, c2 = 12.0, c3 = 3.0;

        CHECK(det3x3(a1,a2,a3,b1,b2,b3,c1,c2,c3) == 12.0);
    }
    SUBCASE("test case for det4x4") {
        MATRIX simple_mat = {{1.0,2.0,3.0,4.0},{1.0,2.0,3.0,4.0},{1.0,2.0,3.0,4.0},{1.0,2.0,3.0,4.0}};

        CHECK(det4x4(simple_mat) == 0.0);

        MATRIX v_mat = {{5,1,3,2},{1,1,1,3},{2,2,3,4},{1,2,3,4}};
        CHECK(det4x4(v_mat) == 3.0);
    }
    SUBCASE("test case for adjoint") {
        MATRIX in_mat = {{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}};

        MATRIX out_mat = {{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}};
        adjoint(in_mat,out_mat);

        for (auto i = 0; i <4; i++){
            for(auto j = 0; j<4;j++){
                CHECK(out_mat[i][j] ==0.0);
            }
        }

        MATRIX in_mat_2 = {{1,1,3,2},{1,1,1,3},{2,2,3,4},{1,2,3,4}};
        adjoint(in_mat_2,out_mat);

        CHECK(out_mat[0][0] ==0.0);
        CHECK(out_mat[0][1] ==-0.0);
        CHECK(out_mat[0][2] ==3.0);
        CHECK(out_mat[0][3] ==-3.0);

        CHECK(out_mat[1][0] ==-5.0);
        CHECK(out_mat[1][1] ==-6.0);
        CHECK(out_mat[1][2] ==4.0);
        CHECK(out_mat[1][3] ==3.0);

        CHECK(out_mat[2][0] ==2.0);
        CHECK(out_mat[2][1] ==-0.0);
        CHECK(out_mat[2][2] ==-1.0);
        CHECK(out_mat[2][3] ==-0.0);

        CHECK(out_mat[3][0] ==1.0);
        CHECK(out_mat[3][1] ==3.0);
        CHECK(out_mat[3][2] ==-2.0);
        CHECK(out_mat[3][3] ==0.0);
    }
    SUBCASE("test case for point_times_4x4") {
        MATRIX simple_mat = {{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}};

        Point simple_point;
        simple_point.x = 2.0;
        simple_point.y = 2.0;
        simple_point.z = 2.0;

        simple_point = point_times_4x4(simple_point, simple_mat);

        CHECK(simple_point.x ==7.0);
        CHECK(simple_point.y ==14.0);
        CHECK(simple_point.z ==21.0);
    }
    SUBCASE("test case for vector_times_3x3of4x4") {
        MATRIX simple_mat = {{1,2,3,4},{1,2,3,4},{1,2,3,4},{1,2,3,4}};

        Point simple_point;
        simple_point.x = 2.0;
        simple_point.y = 2.0;
        simple_point.z = 2.0;

        simple_point = vector_times_3x3of4x4(simple_point, simple_mat);

        CHECK(simple_point.x ==7.0);
        CHECK(simple_point.y ==14.0);
        CHECK(simple_point.z ==21.0);

        simple_v.x = 3.0;
        simple_v.y = 2.0;
        simple_v.z = 10.0;

        simple_v = vector_times_3x3of4x4(simple_v, simple_mat);

        CHECK(simple_v.x ==16.0);
        CHECK(simple_v.y ==32.0);
        CHECK(simple_v.z ==48.0);
    }

    SUBCASE("test case for VolumeBoxIntersect") {

        double rorgx = 1.0;
        double rorgy = 1.0;
        double rorgz = 1.0;
        double rdirx = 1.0;
        double rdiry = 1.0;
        double rdirz = 1.0;

        Point b1;
        b1.x = 2.0;
        b1.y = 2.0;
        b1.z = 2.0;

        Point b2;
        b2.x = 1.0;
        b2.y = 0.0;
        b2.z = 0.0;

        double *tnear =&rorgx;
        double *tfar = &rorgy;

        CHECK(VolumeBoxIntersect(rorgx,rorgy,rorgz,rdirx,rdiry,rdirz,b1,b2,tnear,tfar) == 1.0);
    }

}

#ifdef TESTING_CPP_VERSION
TEST_CASE("Testing rays.hpp: Test Functionality Added By C++ 17 Conversion.") {

    // Only test constructor and overloaded operator. The member functions
    // are tested by calling the their alias according as tested above.

        SUBCASE("Testing Vector Functionality:") {
            Vector simple_v{1, 1, 1};
            REQUIRE(simple_v.x == 1);
            REQUIRE(simple_v.y == 1);
            REQUIRE(simple_v.z == 1);

            Point simple_p{2, 2, 2};
            REQUIRE(simple_p.x == 2);
            REQUIRE(simple_p.y == 2);
            REQUIRE(simple_p.z == 2);

            simple_v +=simple_p;
            CHECK(simple_v.x == 3);
            CHECK(simple_v.y == 3);
            CHECK(simple_v.z == 3);

            CHECK(simple_p.x == 2);
            CHECK(simple_p.y == 2);
            CHECK(simple_p.z == 2);

            simple_v -=simple_p;
            CHECK(simple_v.x == 1);
            CHECK(simple_v.y == 1);
            CHECK(simple_v.z == 1);

            simple_v = -simple_v;
            CHECK(simple_v.x == -1);
            CHECK(simple_v.y == -1);
            CHECK(simple_v.z == -1);

            simple_v *= 2;
            CHECK(simple_v.x == -2);
            CHECK(simple_v.y == -2);
            CHECK(simple_v.z == -2);
        }

        SUBCASE("Testing Ray Functionality:") {
            double x1 = 1.0, y1 = 2.0, z1 = 3.0, x2 = 4.0, y2 = 5.0, z2 = 6.0;

            Ray simple_ray{x1, y1, z1, x2, y2, z2};

            REQUIRE(simple_ray.org.x == 1.0);
            REQUIRE(simple_ray.org.y == 2.0);
            REQUIRE(simple_ray.org.z == 3.0);
            double scale = 0.000001;
            REQUIRE(int(round((int) (simple_ray.dir.x / scale) * scale * 100000)) == 57735);
            REQUIRE(int(round((int) (simple_ray.dir.y / scale) * scale * 100000)) == 57735);
            REQUIRE(int(round((int) (simple_ray.dir.z / scale) * scale * 100000)) == 57735);
        }
        SUBCASE("Testing MATRIX Functionality:") {
            MATRIX m;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    REQUIRE(m[i][j] == 0);
                }
            }

            MATRIX m_2{{1, 1, 1, 1},
                       {2, 2, 2, 2}};
            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 4; j++) {
                    if (i == 0) {
                        REQUIRE(m_2[i][j] == 1);
                    } else {
                        REQUIRE(m_2[i][j] == 2);
                    }
                }
            }
        }


}
#endif
