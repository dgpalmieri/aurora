#include <stdio.h>
#include <stdlib.h>
#include "read_alpha.h"

main(int argc, char *argv[])
{
  char   buf[80];
  int    xres;
  int    yres;
  int    zres;
  double min;
  double max;
  double range;
  unsigned char *voxels;
  unsigned char *ptr;
  unsigned long alpha;
  double value;
  int    x,y,z;
  int    start, stop, step;
  float *slice;
  float *sptr;

  if (argc != 5) {
    printf("Usage: %s {x|y|z} start stop step\n", argv[0]);
    exit(1);
  }
  start = atoi(argv[2]);
  stop  = atoi(argv[3]);
  step  = atoi(argv[4]);

  sprintf(buf, "/Users/genetti/Documents/Aurora/Otto/aurora.alpha");
  printf("Reading file %s\n", buf);
  voxels = read_alpha(buf, &xres, &yres, &zres, &min, &max, 1);
  if (!voxels) {
    printf("Error reading %s\n", buf);
    exit(1);
  }


  if (argv[1][0]=='z' || argv[1][0]=='Z') {

    slice = (float *) malloc( sizeof(float)*xres*yres );
    if (!slice) {
      printf("Can't malloc XY slice\n");
      exit(1);
    }
    for ( z=start ; z<=stop ; z+=step ) {
      sptr = slice;
      for ( y=0 ; y<yres ; y++ ) {
        for ( x=0 ; x<xres ; x++ ) {
          ptr = voxels + 2*(z + y*zres + x*yres*zres);
          alpha = (((unsigned long) *ptr) << 8) | ((unsigned long) *(ptr+1));
          *sptr = alpha / 65535.0;
          sptr ++;
        }
      }
      sprintf(buf, "XY_%05d.ppm",z);
      printf("Writing %s\n", buf);
      pot_2_ppm_file(buf, xres, yres, slice, 0.0, 1.0);
    }
    free(slice);

  } else if (argv[1][0]=='y' || argv[1][0]=='Y') {

    slice = (float *) malloc( sizeof(float)*xres*zres );
    if (!slice) {
      printf("Can't malloc XZ slice\n");
      exit(1);
    }
    for ( y=start ; y<=stop ; y+=step ) {
      sptr = slice;
      for ( z=0 ; z<zres ; z++ ) {
        for ( x=0 ; x<xres ; x++ ) {
          ptr = voxels + 2*(z + y*zres + x*yres*zres);
          alpha = (((unsigned long) *ptr) << 8) | ((unsigned long) *(ptr+1));
          *sptr = alpha / 65535.0;
          sptr ++;
        }
      }
      sprintf(buf, "XZ_%d.ppm",y);
      printf("Writing %s\n", buf);
      pot_2_ppm_file(buf, xres, zres, slice, 0.0, 1.0);
    }
    free(slice);   

  } else if (argv[1][0]=='x' || argv[1][0]=='X') {

    slice = (float *) malloc( sizeof(float)*yres*zres );
    if (!slice) {
      printf("Can't malloc YZ slice\n");
      exit(1);
    }
    for ( x=start ; x<=stop ; x+=step ) {
      sptr = slice;
      for ( z=0 ; z<zres ; z++ ) {
        for ( y=0 ; y<yres ; y++ ) {
          ptr = voxels + 2*(z + y*zres + x*yres*zres);
          alpha = (((unsigned long) *ptr) << 8) | ((unsigned long) *(ptr+1));
          *sptr = alpha / 65535.0;
          sptr ++;
        }
      }
      sprintf(buf, "YZ_%d.ppm",x);
      printf("Writing %s\n", buf);
      pot_2_ppm_file(buf, yres, zres, slice, 0.0, 1.0);
    }
    free(slice);

  } else {

    printf("Invalid slicing direction %s\n", argv[1]);
    exit(1);

  }

  exit(0);
}

#if 0
    range = max - min;
    for ( z=0 ; z<zres ; z++ ) {
      for ( y=0 ; y<yres ; y++ ) {
        for ( x=0 ; x<xres ; x++ ) {
          ptr = voxels + 2*(z*y*zres+x*zres*yres);
          alpha = (((unsigned long) *ptr) << 8) | ((unsigned long) *(ptr+1));
          value = min + alpha * range;
          printf("[%d,%d,%d] = %u %u = %u = %e\n", x, y, z,
            *ptr, *(ptr+1), alpha, value);
        }
      }
    }
#endif
