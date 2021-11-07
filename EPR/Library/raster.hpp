/*      File: raster.hpp                                                   */
/* do we "need" a class, or classes, here to make this C++? */

/* functions to manipulate images */

#ifndef _RASTER_H

#define _RASTER_H

#include "atomics"

/* changed these to "C++ style struct tag" declarations */

/* what does this do, exactly? */
struct Screen {
  int    xres, yres;
  double xmin, xmax;
  double ymin, ymax;
  double near, far;
}

/* holds the values for a pixel at highest accuracy */
struct Pixel {
  double r,g,b,a;
}

/* functions */
void initialize_image(uByte8 *, int, double, double, double, double);
void clamp_image(Pixel *, int);
int  write_image_as_ppm(Pixel *, char *, int, int);
int  write_image_as_avs(Pixel *, char *, int, int);
/* we include atomics.hpp, but here we use "unsigned long int" instead of uLong from atomics.hpp */
/* so I think I am going to change this to uLong and see if anything bad happens */
/* int  write_shaded_as_ppm(unsigned long int *, char *, int, int); */
int write_shaded_as_ppm( uLong *, char *, int, int);

#endif