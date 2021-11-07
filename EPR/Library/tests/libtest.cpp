#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <iostream>
#include <string>
#include "../err_msg.hpp"

TEST_CASE("error codes output correct strings"){
    std::string file = "thing.txt";
    int error = 1;
    auto fileobj = FileError(file, error);
    CHECK(fileobj.getErrorCode() == error);
    CHECK(fileobj.getFileName() == file);
}