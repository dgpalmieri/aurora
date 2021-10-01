#include <stdio.h>

#define MAX_ALPHA 250.0

main(int argc, char *argv[])
{
  FILE *fp;
  int xres, yres;
  unsigned char *bytes;
  int be_verbose=1;
  int i,ptr;
  float  min, max;
  float *values;
  float map_min, map_range;
  float alpha;
  unsigned char r, g, b;

  if (argc != 3) {
    printf("Usage: %s filename image.ppm\n", argv[0]);
    exit(1);
  }

  if ((fp=fopen(argv[1], "rb")) == NULL) {
    printf("Can't open %s\n", argv[1]);
    exit(1);
  }

  if (be_verbose)
    printf("Reading file %s\n", argv[1]);

  if (fscanf(fp, "#RAW\n%d %d\n", &xres, &yres) != 2) {
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

  bytes = (unsigned char *) malloc(xres*yres*2);
  if (!bytes) {
    printf("Can't malloc array to hold image\n");
    exit(1);
  }
  values = (float *) malloc(xres*yres*sizeof(float));
  if (!values) {
    printf("Can't malloc array to hold values\n");
    exit(1);
  }

  if (fread(bytes, 1, xres*yres*2, fp) != xres*yres*2) {
    printf("Error reading data\n");
    free(bytes);
    exit(1);
  }

  if (be_verbose)
    printf("%d bytes (data) read.\n", xres*yres*2);

  fclose(fp);

#if 0
  min = MAX_ALPHA;
  max = 0.0;
  for ( ptr=0,i=0 ; i<xres*yres ; i++,ptr+=2 ) {
    values[i] = ((((unsigned int) bytes[ptr] << 8)
              | ((unsigned int) bytes[ptr+1])) / 65535.0) * MAX_ALPHA;
/*  printf("%lf\n", values[i]);*/
    if (values[i] < min) min = values[i];
    if (values[i] > max) max = values[i];
  }

  printf("Range of values [%lf, %lf]\n", min, max);
  map_min = min;
  map_range = max - min;
#endif

  for ( ptr=0,i=0 ; i<xres*yres ; i++,ptr+=2 ) {
    values[i] = ((((unsigned int) bytes[ptr] << 8)
              | ((unsigned int) bytes[ptr+1])) / 65535.0) * MAX_ALPHA;
  }

  /* write out a ppm file */
  if ((fp = fopen(argv[2], "w")) == 0) {
    printf("Can't open %s\n",argv[2]);
    exit(1);
  }
  fprintf(fp, "P6\n");
  fprintf(fp, "%d %d\n",xres,yres);
  fprintf(fp, "255\n");
  for ( i=0 ; i<xres*yres ; i++ ) {
#if 0
    alpha = (values[i]-map_min) / map_range;
#endif
    alpha = values[i] / MAX_ALPHA;
    r = (unsigned char) (255*(0.1 * alpha));
    g = (unsigned char) (255*(0.8 * alpha));
    b = (unsigned char) (255*(0.4 * alpha));
    putc(r,fp);
    putc(g,fp);
    putc(b,fp);
  }
  fflush(fp);
  fclose(fp);
 
  free(values);
  free(bytes);
}
