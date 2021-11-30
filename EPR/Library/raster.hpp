/*      File: raster.hpp                                                   */

/* functions to manipulate images */

#ifndef _RASTER_H

#define _RASTER_H

#include "atomics.hpp"

/* changed these to "C++ style struct tag" declarations */

struct Screen {
  int    xres, yres;
  double xmin, xmax;
  double ymin, ymax;
  double near, far;
};

/* holds the values for a pixel at highest accuracy */
/* seems to be used only in raster.cpp, but not 100% certain */
struct Pixel {
  double r,g,b,a;
};

/* functions */
void initialize_image(uByte8 *, int, double, double, double, double);
void clamp_image(Pixel *, int);
int  write_image_as_ppm(Pixel *, char *, int, int);
int  write_image_as_avs(Pixel *, char *, int, int);
/* we include atomics.hpp, but here the code used "unsigned long int" instead of uLong from atomics.hpp */
/* so I changed this to uLong */
/* it doesn't appear to be part of aurora workflow */
/* int write_shaded_as_ppm(unsigned long int *, char *, int, int); */
int write_shaded_as_ppm( uLong *, char *, int, int);

#endif
