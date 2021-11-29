// File Name: Noise.hpp
// Contain class declaration for Noise.
// Source file: noise.cpp

#ifndef _NOISE_HPP_
#define _NOISE_HPP_

#include <array>
#include "rays.hpp"

extern const int NUMPTS;
extern const  int P1;
extern const int P2;
extern const int P3;
extern const double phi;

class Noise {
public:
    Noise();
    Noise(int seeding);
    double noise(const Vector& p);
    Vector Dnoise(const Vector& p);
    double turbulence(double x, double y, double z, double size);

private:
    std::array<double, 512> pts{};
};

// Alias free functions:
// provide backward compatibility
[[deprecated("Use Noise object.noise() member function instead.")]]
[[maybe_unused]] double noise(const Vector& p);

[[deprecated("Use Noise object.Dnoise() member function instead.")]]
[[maybe_unused]] Vector Dnoise(const Vector& p);

[[deprecated("Use Noise object.turbulence() member function instead.")]]
[[maybe_unused]] double turbulence(double x, double y, double z, double size);

[[deprecated("Instantiate a Noise object instead of using initnoise(). It is an empty function that does nothing.")]]
[[maybe_unused]] void initnoise();

#endif //_NOISE_HPP_
