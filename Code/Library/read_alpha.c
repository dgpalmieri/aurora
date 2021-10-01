#include <stdio.h>
#include "read_alpha.h"

unsigned char *read_alpha(char   *filename,
                          int    *x_res,
                          int    *y_res,
                          int    *z_res,
                          double *alpha_min,
                          double *alpha_max,
                          int     be_verbose)
{
  FILE          *fp;
  unsigned char *voxels;
  int            xres, yres, zres;
  float          min, max;

  if ((fp=fopen(filename, "rb")) == NULL) {
    printf("Can't open %s\n", filename);
    return 0;
  }

  if (be_verbose)
    printf("Reading file %s\n", filename);

  if (fscanf(fp, "#ALPHA\n%d %d %d %e %e\n",
              &xres, &yres, &zres, &min, &max) != 5) {
    printf("Error reading header\n");
    return 0;
  }

  if ( xres<0 || yres<0 || zres<1 || min>=max) {
    printf("Invalid values in header\n");
    printf("Read xres=%d yres=%d zres=%d alpha_min=%e alpha_max=%e\n",
           xres, yres, zres, min, max);
    return 0;
  }

  if (be_verbose)
    printf("Resolution= %d x %d x %d\nalpha_min=%e alpha_max=%e\n",
           xres, yres, zres, min, max);

  /* malloc the alpha array */
  voxels = (unsigned char *) malloc(xres*yres*zres*2);
  if (!voxels) {     
    printf("Can't malloc voxels array\n");
    return 0;    
  }

  if (fread(voxels, 1, xres*yres*zres*2, fp) != xres*yres*zres*2) {
    printf("Error reading ALPHA data\n");
    free(voxels); return 0;
  }

  if (be_verbose)
    printf("%d bytes (data) read.\n", xres*yres*zres*2);

  fclose(fp);

  /* values to return */
  *x_res = xres;
  *y_res = yres;
  *z_res = zres;
  *alpha_min = min;
  *alpha_max = max;

  return voxels;
}
