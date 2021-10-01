#include <stdio.h>
#include "energy_2_volc.h"

int energy_2_volc(char   *filename,
                  double *energy,
                  int     xres,
                  int     yres,
                  int     zres,
                  double  min,
                  double  range,
                  int     be_verbose)
{
  unsigned char *voxels;
  FILE *volc;
  char magic[7];
  unsigned char volc_header[13];
  unsigned long red, green, blue, alpha;
  unsigned char fixed[8];
  int x,y,z;
  unsigned char *ptr;
  double *val_ptr;
  int offset;

  /* malloc the voxel array */
  voxels = (unsigned char *) malloc(xres*yres*zres*8);
  if (!voxels) {     
    printf("Can't malloc voxel array\n");
    return 0;    
  }

  if ((volc = fopen(filename, "wb")) == NULL) {
    printf("Can't open %s\n", filename);
    free(voxels); return 0;
  }

  if (be_verbose)
    printf("Writing file %s\n", filename);

  magic[0] = '#';
  magic[1] = 'V';
  magic[2] = 'O';
  magic[3] = 'L';
  magic[4] = 'C';
  magic[5] = '\n';
  if (fwrite(magic, 1, 6, volc) != 6) {
    printf("Error writing VOLC magic number\n");
    free(voxels); return 0;
  }

  volc_header[3]  = (unsigned char) (xres & 0x000000ff);
  volc_header[2]  = (unsigned char) ((xres & 0x0000ff00) >> 8);
  volc_header[1]  = (unsigned char) ((xres & 0x00ff0000) >> 16);
  volc_header[0]  = 0x00;

  volc_header[7]  = (unsigned char) (yres & 0x000000ff);
  volc_header[6]  = (unsigned char) ((yres & 0x0000ff00) >> 8);
  volc_header[5]  = (unsigned char) ((yres & 0x00ff0000) >> 16);
  volc_header[4]  = 0x00;

  volc_header[11] = (unsigned char) (zres & 0x000000ff);
  volc_header[10] = (unsigned char) ((zres & 0x0000ff00) >> 8);
  volc_header[9]  = (unsigned char) ((zres & 0x00ff0000) >> 16);
  volc_header[8]  = 0x00;

  if (fwrite(volc_header, 1, 12, volc) != 12) {
    printf("Error writing VOLC header\n");
    free(voxels); return 0;
  }

  /* 8 byte format for <R,G,B,alpha,beta> */
  /* +--------+--------+--------+--------+--------+--------+--------+-------+ */
  /* |RRRRRRRR|rrGGGGGG|GGggggBB|BBBBBBbb|AAAAAAAA|aaaaaaaa|BBBBBBBB|bbbbbbb| */

  /* these values are constant for the volume R=0.1, G=0.8, B=0.4 Beta=0.0 */
  red   = (unsigned long) (0.1 * 1023); /* 10 bits */
  green = (unsigned long) (0.8 * 4095); /* 12 bits */
  blue  = (unsigned long) (0.4 * 1023); /* 10 bits */
  fixed[0] = (unsigned char) (red >> 2);            /* upper 8 bits of R */
  fixed[1] = (unsigned char) (((red & 0x03) << 6)   /* lower 2 bits of R */
           | (green >> 6));                         /* upper 6 bits of G */
  fixed[2] = (unsigned char) (((green & 0x3f) << 2) /* lower 6 bits of G */
           | (blue >> 8));                          /* upper 2 bits of B */
  fixed[3] = (unsigned char) (blue & 0xff);         /* lower 8 bits of B */
  fixed[6] = 0x00; /* fully transparent */
  fixed[7] = 0x00; /* fully transparent */

  val_ptr = energy;   
  for ( z=0 ; z<zres ; z++ ) {
    for ( y=0 ; y<yres ; y++ ) {
      for ( x=0 ; x<xres ; x++ ) {
        
        offset = 8*(z+y*zres+x*zres*yres);
        ptr = voxels + offset;
        *ptr = fixed[0];
        *(ptr+1) = fixed[1];
        *(ptr+2) = fixed[2];
        *(ptr+3) = fixed[3];
        *(ptr+6) = fixed[6];
        *(ptr+7) = fixed[7];
          
        if (*val_ptr > min) {
/*        alpha = (unsigned long) (8192 * ((*val_ptr-min)/range));*/
/*        alpha = (unsigned long) (4095 * ((*val_ptr-min)/range));*/
          alpha = (unsigned long) (4000 * ((*val_ptr-min)/range));
          *(ptr+4) = (unsigned char) ((alpha >> 8) & 0xff);
          *(ptr+5) = (unsigned char) (alpha & 0xff);
        } else {
          *(ptr+4) = *(ptr+5) = 0x00;
        }
        val_ptr ++;

      }
    }
  }

  if (fwrite(voxels, 1, xres*yres*zres*8, volc) != xres*yres*zres*8) {
    printf("Error Writing VOLC data\n");
    free(voxels); return 0;
  }

  if (be_verbose)
    printf("%d bytes written.\n", xres*yres*zres*8);

  fclose(volc);
  free(voxels);

  return 1;
}
