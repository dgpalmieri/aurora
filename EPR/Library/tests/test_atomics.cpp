/*      File: test_atomics.cpp */
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include <limits>
#include "atomics.hpp"

TEST_CASE("Testing atomics.hpp: Types") {

  SUBCASE("Confirm numeric constants") {

    CHECK(INTEN_NUM == 32768);
    CHECK(INTEN_BIT == 15);

  }

  SUBCASE("Confirm type definition for unsigned long int") {

    REQUIRE(sizeof(unsigned long int) == sizeof(uLong));

  }

#ifdef MTA 
  SUBCASE("Confirm type definitions for platform") {

    REQUIRE(sizeof(unsigned char) == sizeof(uByte1));
    REQUIRE(sizeof(unsigned short) == sizeof(uByte4));
    REQUIRE(sizeof(unsigned int) == sizeof(uByte8)); 

  }
#endif

#ifdef SGI64
  SUBCASE("Confirm type definitions for platform") {

    REQUIRE(sizeof(unsigned char) == sizeof(uByte1));
    REQUIRE(sizeof(unsigned int) == sizeof(uByte4));
    REQUIRE(sizeof(unsigned long) == sizeof(uByte8)); 

  }
#endif

//Here's the copy pasta because these platforms 
//are identically specified in atomics.hpp
#ifdef SGI32
  SUBCASE("Confirm type definitions for platform") {

    REQUIRE(sizeof(unsigned char) == sizeof(uByte1));
    REQUIRE(sizeof(unsigned int) == sizeof(uByte4));
    REQUIRE(sizeof(unsigned long long) == sizeof(uByte8)); 

  }
#endif

#ifdef SUN 
  SUBCASE("Confirm type definitions for platform") {

    REQUIRE(sizeof(unsigned char) == sizeof(uByte1));
    REQUIRE(sizeof(unsigned int) == sizeof(uByte4));
    REQUIRE(sizeof(unsigned long long) == sizeof(uByte8)); 

  }
#endif

#ifdef SP2 
  SUBCASE("Confirm type definitions for platform") {

    REQUIRE(sizeof(unsigned char) == sizeof(uByte1));
    REQUIRE(sizeof(unsigned int) == sizeof(uByte4));
    REQUIRE(sizeof(unsigned long long) == sizeof(uByte8)); 

  }
#endif

#ifdef OSX 
  SUBCASE("Confirm type definitions for platform") {

    REQUIRE(sizeof(unsigned char) == sizeof(uByte1));
    REQUIRE(sizeof(unsigned int) == sizeof(uByte4));
    REQUIRE(sizeof(unsigned long long) == sizeof(uByte8)); 

  }
#endif

}
