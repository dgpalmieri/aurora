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
  int start, stop, step, frame;
  char filename[256];

  if (argc != 5) {
    printf("Usage: %s filebase start stop step\n", argv[0]);
    exit(1);
  }

  start = atoi(argv[2]);
  stop  = atoi(argv[3]);
  step  = atoi(argv[4]);

  for ( frame=start ; frame<=stop; frame++ ) {

    sprintf(filename, argv[1], frame);
 
    if ((fp=fopen(filename, "rb")) == NULL) {
      printf("Can't open %s\n", filename);
      exit(1);
    }

    if (be_verbose) printf("Reading file %s\n", filename);

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

    /* compute range of values */
    min = MAX_ALPHA;
    max = 0.0;
    for ( ptr=0,i=0 ; i<xres*yres ; i++,ptr+=2 ) {
      values[i] = ((((unsigned int) bytes[ptr] << 8)
                | ((unsigned int) bytes[ptr+1])) / 65535.0) * MAX_ALPHA;
      if (values[i] < min) min = values[i];
      if (values[i] > max) max = values[i];
    }

    printf("Range of values [%lf, %lf]\n", min, max);

    free(values);
    free(bytes);

  }

}
