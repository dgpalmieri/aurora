/*
 * File: render.hpp
 *
 *
 */

#ifndef _RENDER_H

#define _RENDER_H

struct Vertex {
  double x;  /* x value (in voxel coordinates) */
  double y;  /* y value (in voxel coordinates) */
  double z;  /* z value (in voxel coordinates) */
  double d;  /* weight [0.0, 1.0] brightness of curtain*visibility */
  double e;  /* elevation (in world coordinates) */
  double u;  /* u value for texture map (width of curtain) */
  double v;  /* v value for texture map (along length of curtain) */
};

/* holds information about texture maps applied to the curtains */
struct TurbMap {
  int Ures;      /* resolution in U (width of curtain) */
  int Vres;      /* resolution in V (length of curtain) */
  double area;   /* area that a pixel sample represents */
  short *values; /* array of texture samples */
};

/* fixed max sizes for curtains - change if needed */
const int MAX_FRAMES = 802;
const int MAX_POINTS = 104;
const int MAX_CURTAINS = 9;

/* change both (if needed) */
const int NUM_LATS = 46;
const int LOWER_LAT = 45;
const double LOWER_LAT_RANGE = 45.0;

const int GREEN_CHANNEL = 0;
const int MAGENTA_CHANNEL = 1;
const int RED_CHANNEL = 2;

/* this is also defined in rays with the same name and the same value */
/* #define INF 10e38 */
/* copying what @ETY-13 did in rays.c for now */
const double INF = 1.0e+38;

void render_image(char *, int, int);

#endif
