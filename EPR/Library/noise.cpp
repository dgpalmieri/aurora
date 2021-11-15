// File Name: Noise.cpp
// Contain definitions for Noise in noise.hpp
/*
* Original C language Noise and Dnoise routines
* from: dirks@oak.cis.ohio-state.edu (william r dirks)
* Many thanks to Jon Buller of Colorado State University for these
* routines.
*/

#include <math.h>
#include "noise.hpp"

const int NUMPTS = 512;
const int P1 = 173;
const int P2 = 263;
const int P3 = 337;
const double phi = 0.6180339;

/********************** Definition for Noise *******************/
Noise::Noise() {
    int i;

    for (i = 0; i < NUMPTS; ++i)
        pts[i] = ((float)rand() / (float)RAND_MAX);
}

double Noise::noise(const Vector& p) {
    int xi, yi, zi;
    int xa, xb, xc, ya, yb, yc, za, zb, zc;
    double xf, yf, zf;
    double x2, x1, x0, y2, y1, y0, z2, z1, z0;
    double p000, p100, p200, p010, p110, p210, p020, p120, p220;
    double p001, p101, p201, p011, p111, p211, p021, p121, p221;
    double p002, p102, p202, p012, p112, p212, p022, p122, p222;

    xi = (int)floor(p.x);
    xa = (int)floor(P1 * (xi * phi - floor(xi * phi)));
    xb = (int)floor(P1 * ((xi + 1) * phi - floor((xi + 1) * phi)));
    xc = (int)floor(P1 * ((xi + 2) * phi - floor((xi + 2) * phi)));

    yi = (int)floor(p.y);
    ya = (int)floor(P2 * (yi * phi - floor(yi * phi)));
    yb = (int)floor(P2 * ((yi + 1) * phi - floor((yi + 1) * phi)));
    yc = (int)floor(P2 * ((yi + 2) * phi - floor((yi + 2) * phi)));

    zi = (int)floor(p.z);
    za = (int)floor(P3 * (zi * phi - floor(zi * phi)));
    zb = (int)floor(P3 * ((zi + 1) * phi - floor((zi + 1) * phi)));
    zc = (int)floor(P3 * ((zi + 2) * phi - floor((zi + 2) * phi)));

    p000 = pts[(xa + ya + za) & (NUMPTS - 1)];
    p100 = pts[(xb + ya + za) & (NUMPTS - 1)];
    p200 = pts[(xc + ya + za) & (NUMPTS - 1)];
    p010 = pts[(xa + yb + za) & (NUMPTS - 1)];
    p110 = pts[(xb + yb + za) & (NUMPTS - 1)];
    p210 = pts[(xc + yb + za) & (NUMPTS - 1)];
    p020 = pts[(xa + yc + za) & (NUMPTS - 1)];
    p120 = pts[(xb + yc + za) & (NUMPTS - 1)];
    p220 = pts[(xc + yc + za) & (NUMPTS - 1)];
    p001 = pts[(xa + ya + zb) & (NUMPTS - 1)];
    p101 = pts[(xb + ya + zb) & (NUMPTS - 1)];
    p201 = pts[(xc + ya + zb) & (NUMPTS - 1)];
    p011 = pts[(xa + yb + zb) & (NUMPTS - 1)];
    p111 = pts[(xb + yb + zb) & (NUMPTS - 1)];
    p211 = pts[(xc + yb + zb) & (NUMPTS - 1)];
    p021 = pts[(xa + yc + zb) & (NUMPTS - 1)];
    p121 = pts[(xb + yc + zb) & (NUMPTS - 1)];
    p221 = pts[(xc + yc + zb) & (NUMPTS - 1)];
    p002 = pts[(xa + ya + zc) & (NUMPTS - 1)];
    p102 = pts[(xb + ya + zc) & (NUMPTS - 1)];
    p202 = pts[(xc + ya + zc) & (NUMPTS - 1)];
    p012 = pts[(xa + yb + zc) & (NUMPTS - 1)];
    p112 = pts[(xb + yb + zc) & (NUMPTS - 1)];
    p212 = pts[(xc + yb + zc) & (NUMPTS - 1)];
    p022 = pts[(xa + yc + zc) & (NUMPTS - 1)];
    p122 = pts[(xb + yc + zc) & (NUMPTS - 1)];
    p222 = pts[(xc + yc + zc) & (NUMPTS - 1)];

    xf = p.x - xi;
    x1 = xf * xf;
    x2 = 0.5 * x1;
    x1 = 0.5 + xf - x1;
    x0 = 0.5 - xf + x2;

    yf = p.y - yi;
    y1 = yf * yf;
    y2 = 0.5 * y1;
    y1 = 0.5 + yf - y1;
    y0 = 0.5 - yf + y2;

    zf = p.z - zi;
    z1 = zf * zf;
    z2 = 0.5 * z1;
    z1 = 0.5 + zf - z1;
    z0 = 0.5 - zf + z2;

    return   z0 * (y0 * (x0 * p000 + x1 * p100 + x2 * p200) +
        y1 * (x0 * p010 + x1 * p110 + x2 * p210) +
        y2 * (x0 * p020 + x1 * p120 + x2 * p220)) +
        z1 * (y0 * (x0 * p001 + x1 * p101 + x2 * p201) +
            y1 * (x0 * p011 + x1 * p111 + x2 * p211) +
            y2 * (x0 * p021 + x1 * p121 + x2 * p221)) +
        z2 * (y0 * (x0 * p002 + x1 * p102 + x2 * p202) +
            y1 * (x0 * p012 + x1 * p112 + x2 * p212) +
            y2 * (x0 * p022 + x1 * p122 + x2 * p222));
}

Vector Noise::Dnoise(const Vector& p) {
    Vector v;
    int xi, yi, zi;
    int xa, xb, xc, ya, yb, yc, za, zb, zc;
    double xf, yf, zf;
    double x2, x1, x0, y2, y1, y0, z2, z1, z0;
    double xd2, xd1, xd0, yd2, yd1, yd0, zd2, zd1, zd0;
    double p000, p100, p200, p010, p110, p210, p020, p120, p220;
    double p001, p101, p201, p011, p111, p211, p021, p121, p221;
    double p002, p102, p202, p012, p112, p212, p022, p122, p222;

    xi = (int)floor(p.x);
    xa = (int)floor(P1 * (xi * phi - floor(xi * phi)));
    xb = (int)floor(P1 * ((xi + 1) * phi - floor((xi + 1) * phi)));
    xc = (int)floor(P1 * ((xi + 2) * phi - floor((xi + 2) * phi)));

    yi = (int)floor(p.y);
    ya = (int)floor(P2 * (yi * phi - floor(yi * phi)));
    yb = (int)floor(P2 * ((yi + 1) * phi - floor((yi + 1) * phi)));
    yc = (int)floor(P2 * ((yi + 2) * phi - floor((yi + 2) * phi)));

    zi = (int)floor(p.z);
    za = (int)floor(P3 * (zi * phi - floor(zi * phi)));
    zb = (int)floor(P3 * ((zi + 1) * phi - floor((zi + 1) * phi)));
    zc = (int)floor(P3 * ((zi + 2) * phi - floor((zi + 2) * phi)));

    p000 = pts[(xa + ya + za) & (NUMPTS - 1)];
    p100 = pts[(xb + ya + za) & (NUMPTS - 1)];
    p200 = pts[(xc + ya + za) & (NUMPTS - 1)];
    p010 = pts[(xa + yb + za) & (NUMPTS - 1)];
    p110 = pts[(xb + yb + za) & (NUMPTS - 1)];
    p210 = pts[(xc + yb + za) & (NUMPTS - 1)];
    p020 = pts[(xa + yc + za) & (NUMPTS - 1)];
    p120 = pts[(xb + yc + za) & (NUMPTS - 1)];
    p220 = pts[(xc + yc + za) & (NUMPTS - 1)];
    p001 = pts[(xa + ya + zb) & (NUMPTS - 1)];
    p101 = pts[(xb + ya + zb) & (NUMPTS - 1)];
    p201 = pts[(xc + ya + zb) & (NUMPTS - 1)];
    p011 = pts[(xa + yb + zb) & (NUMPTS - 1)];
    p111 = pts[(xb + yb + zb) & (NUMPTS - 1)];
    p211 = pts[(xc + yb + zb) & (NUMPTS - 1)];
    p021 = pts[(xa + yc + zb) & (NUMPTS - 1)];
    p121 = pts[(xb + yc + zb) & (NUMPTS - 1)];
    p221 = pts[(xc + yc + zb) & (NUMPTS - 1)];
    p002 = pts[(xa + ya + zc) & (NUMPTS - 1)];
    p102 = pts[(xb + ya + zc) & (NUMPTS - 1)];
    p202 = pts[(xc + ya + zc) & (NUMPTS - 1)];
    p012 = pts[(xa + yb + zc) & (NUMPTS - 1)];
    p112 = pts[(xb + yb + zc) & (NUMPTS - 1)];
    p212 = pts[(xc + yb + zc) & (NUMPTS - 1)];
    p022 = pts[(xa + yc + zc) & (NUMPTS - 1)];
    p122 = pts[(xb + yc + zc) & (NUMPTS - 1)];
    p222 = pts[(xc + yc + zc) & (NUMPTS - 1)];

    xf = p.x - xi;
    x1 = xf * xf;
    x2 = 0.5 * x1;
    x1 = 0.5 + xf - x1;
    x0 = 0.5 - xf + x2;
    xd2 = xf;
    xd1 = 1.0 - xf - xf;
    xd0 = xf - 1.0;

    yf = p.y - yi;
    y1 = yf * yf;
    y2 = 0.5 * y1;
    y1 = 0.5 + yf - y1;
    y0 = 0.5 - yf + y2;
    yd2 = yf;
    yd1 = 1.0 - yf - yf;
    yd0 = yf - 1.0;

    zf = p.z - zi;
    z1 = zf * zf;
    z2 = 0.5 * z1;
    z1 = 0.5 + zf - z1;
    z0 = 0.5 - zf + z2;
    zd2 = zf;
    zd1 = 1.0 - zf - zf;
    zd0 = zf - 1.0;

    v.x = z0 * (y0 * (xd0 * p000 + xd1 * p100 + xd2 * p200) +
        y1 * (xd0 * p010 + xd1 * p110 + xd2 * p210) +
        y2 * (xd0 * p020 + xd1 * p120 + xd2 * p220)) +
        z1 * (y0 * (xd0 * p001 + xd1 * p101 + xd2 * p201) +
            y1 * (xd0 * p011 + xd1 * p111 + xd2 * p211) +
            y2 * (xd0 * p021 + xd1 * p121 + xd2 * p221)) +
        z2 * (y0 * (xd0 * p002 + xd1 * p102 + xd2 * p202) +
            y1 * (xd0 * p012 + xd1 * p112 + xd2 * p212) +
            y2 * (xd0 * p022 + xd1 * p122 + xd2 * p222));

    v.y = z0 * (yd0 * (x0 * p000 + x1 * p100 + x2 * p200) +
        yd1 * (x0 * p010 + x1 * p110 + x2 * p210) +
        yd2 * (x0 * p020 + x1 * p120 + x2 * p220)) +
        z1 * (yd0 * (x0 * p001 + x1 * p101 + x2 * p201) +
            yd1 * (x0 * p011 + x1 * p111 + x2 * p211) +
            yd2 * (x0 * p021 + x1 * p121 + x2 * p221)) +
        z2 * (yd0 * (x0 * p002 + x1 * p102 + x2 * p202) +
            yd1 * (x0 * p012 + x1 * p112 + x2 * p212) +
            yd2 * (x0 * p022 + x1 * p122 + x2 * p222));

    v.z = zd0 * (y0 * (x0 * p000 + x1 * p100 + x2 * p200) +
        y1 * (x0 * p010 + x1 * p110 + x2 * p210) +
        y2 * (x0 * p020 + x1 * p120 + x2 * p220)) +
        zd1 * (y0 * (x0 * p001 + x1 * p101 + x2 * p201) +
            y1 * (x0 * p011 + x1 * p111 + x2 * p211) +
            y2 * (x0 * p021 + x1 * p121 + x2 * p221)) +
        zd2 * (y0 * (x0 * p002 + x1 * p102 + x2 * p202) +
            y1 * (x0 * p012 + x1 * p112 + x2 * p212) +
            y2 * (x0 * p022 + x1 * p122 + x2 * p222));
    return v;
}

double Noise::turbulence(double x, double y, double z, double size) {
    double value = 0.0;
    double weight = 1.0 / size;
    double initialsize = 1.0 / (size * 2.0);
    Vector p;

    while (size >= 1.0) {
        p.x = x * weight; p.y = y * weight; p.z = z * weight;
        value += noise(p) * size;
        weight *= 2.0;
        size *= 0.5;
    }

    return value * initialsize;
}
/****************** End of Definition for Noise ******************/


/****************** Alias Function Definition ********************/
double noise(const Vector& p, bool seeding, int seed) {
    if (seeding) srand(seed);
    Noise n;
    return n.noise(p);
}

Vector Dnoise(const Vector& p, bool seeding, int seed) {
    if (seeding) srand(seed);
    Noise n;
    return n.Dnoise(p);
}

double turbulence(double x, double y, double z, double size,
    bool seeding, int seed) {
    if (seeding) srand(seed);
    Noise n;
    return n.turbulence(x, y, z, size);
}

// Empty definition.
// This function is only here to prevent
// undefined error in older code that calls
// on it.
void initnoise() {}
/**************** End of Alias Function Definition ******************/
