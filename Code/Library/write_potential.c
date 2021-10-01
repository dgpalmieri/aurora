#include <stdio.h>
#include "write_potential.h"

static char rec1[5] = {0, 0, 0, 12, 0};
static char rec2[5] = {0, 0, 37, -116, 0};
static char rec3[5] = {0, 88, 9, -124, 0};

int write_potential(char *filename,
                    int   xmax,
                    int   ymax,
                    float time_step,
                    float *xunits,
                    float *yunits,
                    float zpo,
                    float *pot,
                    int   be_verbose)
{
  FILE *fp;

  if ((fp = fopen(filename, "wb")) == 0) {
    printf("Can't open %s\n", filename);
    return 0;
  }

  /* write the first record marker */
  fwrite(rec1, 1, 4, fp);

  /* write the dimensions of the grid */
  fwrite(&xmax, 1, 4, fp);
  fwrite(&ymax, 1, 4, fp);
  fwrite(&time_step, 1, 4, fp);

  /* write the first record marker */
  fwrite(rec1, 1, 4, fp);

  /* write the second record marker */
  fwrite(rec2, 1, 4, fp);

  /* write the grid coordinates */
  if (fwrite(xunits, sizeof(float), xmax, fp) != xmax)
    { printf("Error writing X grid coordinates\n"); return 0; }
  if (fwrite(yunits, sizeof(float), ymax, fp) != ymax)
    { printf("Error writing Y grid coordinates\n"); return 0; }
  if (fwrite(&zpo, 1, 4, fp) != 4)
    { printf("Error writing Z potential\n"); return 0; }

  /* write the second record marker */
  fwrite(rec2, 1, 4, fp);

  /* write the third record marker */
  fwrite(rec3, 1, 4, fp);

  if (fwrite(pot, sizeof(float), xmax*ymax, fp) != xmax*ymax)
    { printf("Error reading potential values\n"); return 0; }

  /* write the third record marker */
  fwrite(rec3, 1, 4, fp);

  fclose(fp);

  return 1;
}

