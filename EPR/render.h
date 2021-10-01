/*
 * File: render.h
 *
 *
 */

#ifndef _RENDER_H

#define _RENDER_H

typedef struct {
  double x;  /* x value (in voxel coordinates) */
  double y;  /* y value (in voxel coordinates) */
  double z;  /* z value (in voxel coordinates) */
  double d;  /* weight [0.0, 1.0] brightness of curtain*visibility */
  double e;  /* elevation (in world coordinates) */
  double u;  /* u value for texture map (width of curtain) */
  double v;  /* v value for texture map (along length of curtain) */
} Vertex;

/* holds information about texture maps applied to the curtains */
typedef struct {
  int Ures;      /* resolution in U (width of curtain) */
  int Vres;      /* resolution in V (length of curtain) */
  double area;   /* area that a pixel sample represents */
  short *values; /* array of texture samples */
} TurbMap;

/* fixed max sizes for curtains - change if needed */
#define MAX_FRAMES 802
#define MAX_POINTS 104
#define MAX_CURTAINS 9

/* change both (if needed) */
#define NUM_LATS 46
#define LOWER_LAT 45
#define LOWER_LAT_RANGE 45.0

#define GREEN_CHANNEL   0
#define MAGENTA_CHANNEL 1
#define RED_CHANNEL     2

#define INF 10e38

void render_image(char *, int, int);

#endif

