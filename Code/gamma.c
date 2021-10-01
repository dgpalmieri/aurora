#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* hard code a maximum 16-bit gamma table */
#define MAX_ENTRIES 65536
static unsigned long gamma_table[MAX_ENTRIES];

void set_gamma(n, gamma, gain)
int    n;
double gamma;
double gain;
{
  int    i;
  int    up_limit;
  double max_val;
  double one_over_gamma;

  if (n > MAX_ENTRIES) {
    printf("Need to increase the size of MAX_ENTRIES\n");
    exit(1);
  }

  up_limit       = n-1;
  max_val        = (double) up_limit;
  one_over_gamma = 1.0 / gamma;
  for ( i=0 ; i<n ; i++ ) {
    gamma_table[i] = up_limit*pow(i/max_val*gain,one_over_gamma);
    #if 1
      printf("%d: %d\n",i,gamma_table[i]);
    #endif
  }

}

main(int argc, char *argv[])
{
  FILE *fp;
  int xres, yres;
  int be_verbose=0;
  int i,x,y,ptr;
  float  min, max;
  float *values;
  float map_min, map_range;
  float alpha;
  unsigned char r, g, b;
  int tmp;
  float input_gamma;
  unsigned char val;

  if (argc != 4 && argc != 6) {
    printf("Usage: %s filename image.ppm gamma [min_map max_map]\n", argv[0]);
    exit(1);
  }

  if ((fp=fopen(argv[1], "rb")) == NULL) {
    printf("Can't open %s\n", argv[1]);
    exit(1);
  }

  if (be_verbose)
    printf("Reading file %s\n", argv[1]);

  if (fscanf(fp, "#FRAW\n%d %d\n", &xres, &yres) != 2) {
    printf("Error reading header\n");
    exit(1);
  }

  if (xres<0 || yres<0) {
    printf("Invalid values in header\n");
    printf("Read xres=%d yres=%d\n", xres, yres);
    exit(1);
  }

  if (be_verbose)
    printf("Resolution = %dx%d\n", xres, yres);

  values = (float *) malloc(xres*yres*sizeof(float));
  if (!values) {
    printf("Can't malloc array to hold values\n");
    exit(1);
  }

  tmp = fread(values, sizeof(float), xres*yres, fp);
  if (tmp != xres*yres) {
/*if (fread(values, sizeof(float), xres*yres, fp) != xres*yres) {*/
    printf("Error reading data\n");
    free(values);
    exit(1);
  }

  if (be_verbose)
    printf("%d bytes (data) read.\n", xres*yres*sizeof(float));

  fclose(fp);

  input_gamma = atof(argv[3]);
  if (input_gamma <= 0.0) {
    printf("Gamma must be positive (input value = %lf)\n", input_gamma);
    exit(1);
  }

set_gamma(256, input_gamma, 1.0);

  if (argc == 6) {
    /* use supplied min/max values */
    min = atof(argv[4]);
    max = atof(argv[5]);
    if (min >= max) {
       printf("Invalid range [%lf, %lf]\n", min, max);
       exit(1);
    }
  } else {
    /* find max value */
    min = max = values[0];
    for ( i=1 ; i<xres*yres ; i++ ) {
      if (values[i] > max) max = values[i];
      if (values[i] < min) min = values[i];
    }
  }
  printf("%s: Range of values for mapping [%lf, %lf]\n", argv[1], min, max);
  map_range = max - min;

  /* write out a ppm file */
  if ((fp = fopen(argv[2], "w")) == 0) {
    printf("Can't open %s\n",argv[2]);
    exit(1);
  }

  fprintf(fp, "P6\n");
  fprintf(fp, "%d %d\n",xres,yres);
  fprintf(fp, "255\n");

  /* flip in y as PPM have 0,0 at upper right */
  for ( y=yres-1 ; y>=0 ; y-- ) {
    for ( x=0 ; x<xres ; x++ ) {
      i = y*xres + x;
      if (values[i] <= min) alpha = 0.0;
      else if (values[i] >= max) alpha = 1.0;
      else alpha = (values[i]-min) / map_range;
      val = gamma_table[(int)(255*alpha)];
#if 0
      if (val > 253) {
        r = g = b = 255;
      } else {
#endif
        r = (unsigned char) 0;
        g = (unsigned char) val;
        b = (unsigned char) 0;
#if 0
      }
#endif
      putc(r,fp);
      putc(g,fp);
      putc(b,fp);
    }
  }

  fflush(fp);
  fclose(fp);
 
  free(values);
}
