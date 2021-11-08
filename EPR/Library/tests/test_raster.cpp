/* File: test_raster.cpp */
#include <iostream>
#include <cmath>
#include "raster.hpp"

int main()
{

/* holds raster/window information */
Screen s;
/* set defaults for Screen data structure */
s.xmin = -0.045;
s.xmax =  0.045;
s.ymin = -0.045;
s.ymax =  0.045;
s.near =  0.1;
s.far  =  5.0;
s.xres =  400;
s.yres =  400;

/* dgpalmieri makes the salient point that this should be done with doctest */
std::cout << "Screen is a data structure of two integers and nine doubles." << std::endl;
std::cout << "On this platform, an int is of size " << sizeof(int) << ", and a double is of size " << sizeof(double) << "." << std::endl;
std::cout << "So the Screen data structure consumes " << 2 * sizeof(int) << " plus " << 9 * sizeof(double) \
 << " memory, for a total of " << 2*sizeof(int) + 9*sizeof(double) << "." << std::endl;
std::cout << "The two integers represent the resolution of the screen in the x dimension and the y dimension." << std::endl;
std::cout << "The EPR code initializes the screen to an x resolution of " << s.xres << ", which is a value of size " << sizeof (s.xres) << "." << std::endl;
std::cout << "The EPR code initializes the screen to a y resolution of " << s.yres << ", which is a value of size " << sizeof (s.yres) << "." << std::endl;
std::cout << "Two doubles hold the minimum and the maximum for x, and two more hold the minimum and the maximum for y." << std::endl;
std::cout << "The EPR code initializes the x minimum to " << s.xmin << ", which is a value of size " << sizeof(s.xmin) << "." << std::endl;
std::cout << "The EPR code initializes the x maximum to " << s.xmax << ", which is a value of size " << sizeof(s.xmax) << "." << std::endl;
std::cout << "The EPR code initializes the y minimum to " << s.ymin << ", which is a value of size " << sizeof(s.ymin) << "." << std::endl;
std::cout << "The EPR code initializes the y maximum to " << s.ymax << ", which is a value of size " << sizeof(s.ymax) << "." << std::endl;
std::cout << "The third set of doubles hold the values for near and far." << std::endl;
std::cout << "The EPR code initializes the value of near to " << s.near << ", which is a value of size " << sizeof(s.near) << "." << std::endl;
std::cout << "The EPR code initializes the value of far to " << s.far << ", which is a value of size " << sizeof(s.far) << "." << std::endl;

return 1;
}
