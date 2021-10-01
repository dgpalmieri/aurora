/*      File: raster.h                                                     */


/* functions to manipulate images */

#ifndef _RASTER_H

#define _RASTER_H

#include "atomics.h"

typedef struct {
  int    xres, yres;
  double xmin, xmax;
  double ymin, ymax;
  double near, far;
} Screen;

/* holds the values for a pixel at highest accuracy */
typedef struct {
  double r,g,b,a;
} Pixel;

/* functions */
void initialize_image(uByte8 *, int, double, double, double, double);
void clamp_image(Pixel *, int);
int  write_image_as_ppm(Pixel *, char *, int, int);
int  write_image_as_avs(Pixel *, char *, int, int);
int  write_shaded_as_ppm(unsigned long int *, char *, int, int);

#endif

