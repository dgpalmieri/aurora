/* ignores earth calculation for now */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

main(int argc, char *argv[])
{
  FILE *fp;
  int xres, yres;
  int be_verbose=1;
  int i,x,y,ptr;
  float  min, max;
  float *values;
  float *exposed;
  float map_min, map_range;
  float alpha;
  unsigned char r, g, b;
  int tmp;
  int    start, stop, step;
  char buf[80];
  int all_xres, all_yres, xyres;


  if (argc != 6) {
    printf("Usage: %s filebase start stop step outfile\n", argv[0]);
    exit(1);
  }

  start = atoi(argv[2]);
  stop  = atoi(argv[3]);
  step  = atoi(argv[4]);

  /* open the first file to determine the sizes */
  sprintf(buf, argv[1], start);
  if ((fp=fopen(buf, "rb")) == NULL) {
    printf("Can't open %s\n", buf);
    exit(1);
  }

  if (fscanf(fp, "#FRAW\n%d %d\n", &xres, &yres) != 2) {
    printf("Error reading header\n");
    exit(1);
  }

  if (xres<0 || yres<0) {
    printf("Invalid values in header\n");
    printf("Read xres=%d yres=%d\n", xres, yres);
    exit(1);
  }

  printf("Resolution = %dx%d\n", xres, yres);

  values = (float *) malloc(xres*yres*sizeof(float));
  if (!values) {
    printf("Can't malloc array to hold read values\n");
    exit(1);
  }

  exposed = (float *) malloc(xres*yres*sizeof(float));
  if (!exposed) {
    printf("Can't malloc array to hold final values\n");
    exit(1);
  }

  all_xres = xres;
  all_yres = yres;
  xyres = xres*yres;

  for ( ptr=0 ; ptr<xyres ; ptr++ )
    exposed[ptr] = 1.0;

  for ( i=start ; i<=stop ; i+=step ) {

    sprintf(buf, argv[1], i);
    printf("Reading file %s\n", buf);
    if ((fp=fopen(buf, "rb")) == NULL) {
      printf("Can't open %s\n", buf);
      exit(1);
    }

    if (fscanf(fp, "#FRAW\n%d %d\n", &xres, &yres) != 2) {
      printf("Error reading header\n");
      exit(1);
    }

    if (xres!=all_xres || yres!=all_yres) {
      printf("Invalid values in header\n");
      printf("Read xres=%d yres=%d\n", xres, yres);
      exit(1);
    }

    if (fread(values, sizeof(float), xres*yres, fp) != xres*yres) {
      printf("Error reading data\n");
      free(values);
      exit(1);
    }

    printf("%d bytes (data) read.\n", xres*yres*sizeof(float));

    fclose(fp);

    for ( ptr=0 ; ptr<xyres ; ptr++ )
      exposed[ptr] += fabs(values[ptr])-1.0;

  }
 
  if ((fp = fopen(argv[5], "wb")) == NULL) {
    printf("Can't open %s\n", argv[5]);
    exit(1);
  }
  fprintf(fp, "#FRAW\n%d %d\n", all_xres, all_yres);
  fwrite(exposed, sizeof(float), xyres, fp);
  fclose(fp);

  free(exposed);
  free(values);
}
