#include <stdio.h>
#include "energy_2_alpha.h"

int energy_2_alpha(char   *filename,
                   double *energy,
                   int     xres,
                   int     yres,
                   int     zres,
                   double  alpha_min,
                   double  alpha_max,
                   int     be_verbose)
{
  FILE *fp;
  int x,y,z;
  unsigned char *voxels;
  unsigned char *ptr;
  double *val_ptr;
  int offset;
  double normalize;
  unsigned long alpha;
  const double alpha_range = alpha_max - alpha_min;

  /* malloc the alpha array */
  voxels = (unsigned char *) malloc(xres*yres*zres*2);
  if (!alpha) {     
    printf("Can't malloc voxels array\n");
    return 0;    
  }

  if ((fp = fopen(filename, "wb")) == NULL) {
    printf("Can't open %s\n", filename);
    free(alpha); return 0;
  }

  if (be_verbose)
    printf("Writing file %s\n", filename);

  fprintf(fp, "#ALPHA\n%d %d %d %e %e\n",
          xres, yres, zres, alpha_min, alpha_max);
  fflush(fp);

  /* alpha values are stored as 2 byte integers */
  /* mapped between alpha_min and alpha_max */

  val_ptr = energy;   
  for ( z=0 ; z<zres ; z++ ) {
    for ( y=0 ; y<yres ; y++ ) {
      for ( x=0 ; x<xres ; x++ ) {
        offset = 2*(z+y*zres+x*zres*yres);
        ptr = voxels + offset;
        if (*val_ptr <= alpha_min) {
          *(ptr) = *(ptr+1) = 0x00;
        } else if (*val_ptr >= alpha_max) {
          *(ptr) = *(ptr+1) = 0xff;
        } else {
          /* map between 0.0 and 1.0 */
          normalize = (*val_ptr - alpha_min) / alpha_range;
          alpha = (unsigned long) (65535 * normalize);
          *(ptr) = (unsigned char) ((alpha >> 8) & 0xff);
          *(ptr+1) = (unsigned char) (alpha & 0xff);
        }
        val_ptr ++;
      }
    }
  }


#if 0
        if (*val_ptr > min) {
          alpha = (unsigned long) (4000 * ((*val_ptr-min)/range));
          *(ptr+4) = (unsigned char) ((alpha >> 8) & 0xff);
          *(ptr+5) = (unsigned char) (alpha & 0xff);
        } else {
          *(ptr+4) = *(ptr+5) = 0x00;
        }
#endif


  if (fwrite(voxels, 1, xres*yres*zres*2, fp) != xres*yres*zres*2) {
    printf("Error Writing ALPHA data\n");
    free(voxels); return 0;
  }

  if (be_verbose)
    printf("%d bytes (data) written.\n", xres*yres*zres*2);

  fclose(fp);
  free(voxels);

  return 1;
}
