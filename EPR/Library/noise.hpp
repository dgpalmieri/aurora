// File Name: Noise.hpp
// Contain class declaration for Noise.
// Source file: noise.cpp

#ifndef _NOISE_HPP_
#define _NOISE_HPP_

#include<array>

#include "rays.hpp"

extern const int NUMPTS;
extern const  int P1;
extern const int P2;
extern const int P3;
extern const double phi;

class Noise {
public:
    Noise();
    double noise(const Vector & p);
    Vector Dnoise(const Vector & p);
    double turbulence(double x, double y, double z, double size);

private:
    std::array<double,512> pts{};
};

// Alias free functions:
// provide backward compatibility
[[deprecated("Used Noise object.noise() instead.")]]
[[maybe_unused]] double noise(const Vector & p, bool seeding = true, int seed = 10);

[[deprecated("Used Noise object.Dnoise() instead.")]]
[[maybe_unused]] Vector Dnoise(const Vector & p, bool seeding = true, int seed = 10);

[[deprecated("Used Noise object.turbulence() instead.")]]
[[maybe_unused]] double turbulence(double x, double y, double z, double size,
                            bool seeding = true, int seed = 10);

[[deprecated("initnoise() is an empty function that do nothing. "
             "Declare a Noise object instead.")]]
[[maybe_unused]] void initnoise();

#endif //_NOISE_HPP_
