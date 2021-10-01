#include <stdio.h>
#include <stdlib.h>

main(int argc, char *argv[])
{
  FILE *fp;
  int xres, yres, maxval;
  int be_verbose=1;
  int i,j,ptr;
  int tmp;
  unsigned char *values;
  int newxres, newyres;
  int left, right, bottom, top;

  if (argc != 7) {
    printf("Usage: %s in.ppm out.ppm left right bottom top\n", argv[0]);
    exit(1);
  }

  if ((fp=fopen(argv[1], "rb")) == NULL) {
    printf("Can't open %s\n", argv[1]);
    exit(1);
  }

  if (be_verbose)
    printf("Reading file %s\n", argv[1]);

  if (fscanf(fp, "P6\n%d %d\n%d\n", &xres, &yres, &maxval) != 3) {
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

  left = atoi(argv[3]);
  right = atoi(argv[4]);
  bottom = atoi(argv[5]);
  top = atoi(argv[6]);
  if (left < 0 || bottom < 0 || right >= xres || top >= yres ||
      left > right || bottom > top) {
    printf("Invalid subrange X:(%d,%d) Y:(%d,%d)\n", left, right, bottom, top);
    exit(1);
  }

  newxres = right - left + 1;
  newyres = top - bottom + 1;
  if (be_verbose)
    printf("Cropping to %dx%d at %d,%d\n", newxres, newyres, left, bottom);

  values = (unsigned char *) malloc(xres*yres*sizeof(unsigned char)*3);
  if (!values) {
    printf("Can't malloc array to hold values\n");
    exit(1);
  }

  tmp = fread(values, sizeof(unsigned char), xres*yres*3, fp);
  printf("%d values read\n", tmp);
  if (tmp != xres*yres*3) {
    printf("Error reading data\n");
    free(values);
    exit(1);
  }

  if (be_verbose)
    printf("%d bytes (data) read.\n", xres*yres*sizeof(unsigned char)*3);

  fclose(fp);



  /* write out a ppm file */
  if ((fp = fopen(argv[2], "w")) == 0) {
    printf("Can't open %s\n",argv[2]);
    exit(1);
  }
  fprintf(fp, "P6\n");
  fprintf(fp, "%d %d\n",newxres,newyres);
  fprintf(fp, "255\n");
  for ( j=bottom ; j<=top ; j++ ) {
/*  for ( j=top ; j>=bottom ; j-- ) {*/
    for ( i=left ; i<=right ; i++ ) {
      putc(values[3*(j*xres+i)]  ,fp);
      putc(values[3*(j*xres+i)+1],fp);
      putc(values[3*(j*xres+i)+2],fp);
    }
  }
  fflush(fp);
  fclose(fp);
 
  free(values);
}
