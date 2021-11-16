/*      File: raster.cpp                                                   */

#include <cstdio>
#include <cmath>
/* per Dr. Genetti, there are no asserts in production code, so can leave this out */
/* #include <assert.h> */

#include "raster"

/*--------------------------------------------------------------------*/

void initialize_image(
uByte8 *image,
int     num_pixels,
double  r,
double  g,
double  b,
double  a
)
{
  int i;
/* not sure this declaration and assignment was valid ANSI C? */
  const uByte8 value = (((uByte8) (a * INTEN_NUM + 0.5)) << 48)
                     | (((uByte8) (r * INTEN_NUM + 0.5)) << 32)
                     | (((uByte8) (g * INTEN_NUM + 0.5)) << 16)
                     |  ((uByte8) (b * INTEN_NUM + 0.5));

  for ( i=0 ; i<num_pixels ; i++ ) {
    image[i] = value;
  }

}

/*-------------------------------------------------------------------*/
/* what is the "clamp" here? */
/* this function turns the r, g, b values to 1.0 if they are greater */

void clamp_image(
Pixel *image,
int    num_pixels
)
{
  int i;

  for ( i=0 ; i<num_pixels ; i++,image++ ) {
    if (image->r > 1.0) image->r = 1.0;
    if (image->g > 1.0) image->g = 1.0;
    if (image->b > 1.0) image->b = 1.0;
  }

}

/*-------------------------------------------------------------------*/

int write_image_as_ppm(
Pixel  *image,
char   *filename,
int     xres,
int     yres
)
{
  int   i,j;
  FILE *output;
  int   r,g,b;
  int   num_pixels = xres*yres;
  int   index;

  /* write the image to a file */
  if ((output = fopen(filename,"wb+")) == NULL) {

    fprintf(stderr, "Cannot open file %s.\n",filename);
    fflush(stderr);
    return 0;

  } else {

    fprintf(output, "P6\n");
    fprintf(output, "%d %d\n",xres,yres);
    fprintf(output, "255\n");

    /* those damn ppm files think 0,0 is at the top */
    for ( j=yres-1 ; j>=0 ; j-- ) {
      for ( i=0 ; i<xres ; i++ ) {
        index = i + j*xres;
        r = image[index].r * 255;
        g = image[index].g * 255;
        b = image[index].b * 255;
        fputc(r,output);
        fputc(g,output);
        fputc(b,output);
      }
    }
/* I suppose it would be nice to try catch exceptions here to save the work? */
    fflush(output);
    fclose(output);
    return num_pixels;

  }
}

/*-------------------------------------------------------------------*/
/* since this is a no-op, can we leave it out of the code that we bring forward to C++? */
int write_image_as_avs(
Pixel  *image,
char   *filename,
int     xres,
int     yres
)
{
  return 0;
}

/*-------------------------------------------------------------------*/
/* I don't understand the difference between write_shaded_as_ppm() */
/* and write_image_as_ppm() */
/* is there an advantage here passing image by reference instead? */
/* int write_shaded_as_ppm( unsigned long int &image...) */
int write_shaded_as_ppm(
unsigned long int *image,
char  *filename,
int    xres,
int    yres
)
{
  int   i,j;
  FILE *output;
  int   r,g,b;
  int   num_pixels = xres*yres;
  int   index;

  /* write the image to a file */
  if ((output = fopen(filename,"wb+")) == NULL) {

    fprintf(stderr, "Cannot open file %s.\n",filename);
    fflush(stderr);
    return 0;

  } else {

    fprintf(output, "P6\n");
    fprintf(output, "%d %d\n",xres,yres);
    fprintf(output, "255\n");

    /* those damn ppm files think 0,0 is at the top */
    for ( j=yres-1 ; j>=0 ; j-- ) {
      for ( i=0 ; i<xres ; i++ ) {
        index = i + j*xres;
        r = (image[index] & 0x000000ff);
        g = (image[index] & 0x0000ff00) >> 8;
        b = (image[index] & 0x00ff0000) >> 16;
        fputc(r,output);
        fputc(g,output);
        fputc(b,output);
      }
    }
    fflush(output);
    fclose(output);
    return num_pixels;

  }
}

/*-------------------------------------------------------------------*/

