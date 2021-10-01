#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if 0
#define _SHOW_EARTH_AS_BLUE
#define _SWAP_BYTES
#endif

#define NUM_CHANNELS 3
char *channels[NUM_CHANNELS] = { "green", "magenta", "red" };
double max_color[NUM_CHANNELS][3] = { { 0.29, 0.92, 0.61 },
                                      { 0.95, 0.71, 0.91 },
                                      { 0.92, 0.18, 0.078} };
#if 0
/* as of 06-02-13 */
#define NUM_PTS 14
static double sp_y[NUM_PTS] = {
 -0.1, 0.0, 0.3, 0.5, 0.61, 0.68, 0.73, 0.76, 0.79, 0.81, 0.83, 0.84, 0.84, 0.84 };
#endif

/* new as of 06-02-15 */
#define NUM_PTS 14
static double sp_y[NUM_PTS] = {
-0.09, 0.00, 0.19, 0.38, 0.54, 0.67, 0.78, 0.86, 0.91, 0.94, 0.96, 0.97, 0.99, 1.03 };


#ifdef SWAP_BYTES
float swapfloat( float f ) {
  union {
    float f;
    unsigned char b[4];
  } orig, final;
  orig.f = f;
  final.b[0] = orig.b[3];
  final.b[1] = orig.b[2];
  final.b[2] = orig.b[1];
  final.b[3] = orig.b[0];
  return final.f;
}       
#endif

main(int argc, char *argv[])
{
  FILE *fp;
  int xres=-1;
  int yres=-1;
  int tmpxres;
  int tmpyres;
  int be_verbose=0;
  int i,j,c,x,y,ptr;
  float min[NUM_CHANNELS];
  float max[NUM_CHANNELS];
  float map_range[NUM_CHANNELS];
  float *values = NULL;
  float *raster = NULL;
  float alpha;
  float tmpf;
  unsigned char r, g, b;
  int tmp;
  int index;
  int tmpc;
  char infilename[256];
  int  calc_min_max = 1;
  double gamma;
#ifdef SHOW_EARTH_AS_BLUE
  int not_added=1;
#endif
  int max_entry;
  float low_r, low_g, low_b;
  float inc_r, inc_g, inc_b;
  int u_i;
  double u1, u2, u3;
  double w1, w2, w3, w4;
  double u_value;
  double intensity;


  if (argc != 3 && argc != 9) {
    printf("Usage: %s filebase out.ppm [Gmin Gmax Mmin Mmax Rmin Rmax]\n",
           argv[0]);
    exit(1);
  }

  if (argc == 9) {
    calc_min_max = 0;
    min[0] = atof(argv[3]);
    max[0] = atof(argv[4]);
    min[1] = atof(argv[5]);
    max[1] = atof(argv[6]);
    min[2] = atof(argv[7]);
    max[2] = atof(argv[8]);
    if (min >= max) {
       printf("Invalid range [%lf, %lf]\n", min, max);
       exit(1);
    }
  }

#ifdef SWAP_BYTES
  printf("Endian order will be swapped.\n");
#endif

  for ( c=0 ; c<NUM_CHANNELS ; c++ ) {

    sprintf(infilename, "%s.%s", argv[1], channels[c]);

    if ((fp=fopen(infilename, "rb")) == NULL)
      { printf("%s not found\n", infilename); continue; }

    if (be_verbose) printf("Reading file %s\n", infilename);

    if (fscanf(fp, "#FRAW\n%d %d\n", &tmpxres, &tmpyres) != 2) {
      printf("Error reading header\n");
      exit(1);
    }

    if (xres < 0) {

      /* haven't been set yet */
      if (tmpxres<1 || tmpyres<1) {
        printf("Invalid values in header\n");
        printf("Read xres=%d yres=%d\n", tmpxres, tmpyres);
        exit(1);
      }

      /* set them */
      xres = tmpxres;
      yres = tmpyres;

    } else {

      /* verify these are the same */
      if (xres != tmpxres || yres != tmpyres) {
        printf("Raster resolutions do not match\n");
        exit(1);
      }

    }

    if (be_verbose)
      printf("Resolution = %dx%d\n", xres, yres);

    if (!values) {

      /* need to malloc memory to hold input energy */
      values = (float *) malloc(xres*yres*sizeof(float));
      if (!values) {
        printf("Can't malloc array to hold values\n");
        exit(1);
      }

    }

    if (fread(values, sizeof(float), xres*yres, fp) != xres*yres) {
      printf("Error reading data\n");
      free(values);
      exit(1);
    }

    fclose(fp);

    if (be_verbose)
      printf("%d bytes (data) read.\n", xres*yres*sizeof(float));

    #ifdef SWAP_BYTES
      for ( i=0 ; i<xres*yres ; i++ ) {
        values[i] = swapfloat(values[i]);
      }
    #endif

    if (calc_min_max) {
      min[c] = max[c] = fabs(values[0]) - 1.0;
      for ( i=1 ; i<xres*yres ; i++ ) {
        if ((fabs(values[i])-1.0) > max[c]) max[c] = fabs(values[i])-1.0;
        if ((fabs(values[i])-1.0) < min[c]) min[c] = fabs(values[i])-1.0;
      }
    }
    printf("%s: Range of values for mapping [%lf, %lf]\n", infilename, min[c], max[c]);
    map_range[c] = max[c] - min[c];

    if (!raster) {
      /* allocate memory for a floating point raster */
      raster = (float *) malloc(3*xres*yres*sizeof(float));
      if (!raster)
        { printf("Can't malloc array to hold raster\n"); exit(1); }
      for ( i=0 ; i<3*xres*yres ; i++ )
        raster[i] = 0.0;
    }

    /* add this energy to the raster */
    for ( i=0 ; i<xres*yres ; i++ ) {

      if ((fabs(values[i])-1.0) <= min[c]) alpha = 0.0;
      else if ((fabs(values[i])-1.0) >= max[c]) alpha = 1.0;
      else alpha = (fabs(values[i])-1.0-min[c]) / map_range[c];

      u_value = 1.0 + alpha * 10.0;

      u_i = (int) u_value; u1 = u_value - u_i; u2 = u1 * u1; u3 = u2 * u1;
      w1 = -0.5*u3 +     u2 - 0.5*u1;
      w2 =  1.5*u3 - 2.5*u2 + 1.0;
      w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
      w4 =  0.5*u3 - 0.5*u2;

      intensity = w1*sp_y[u_i-1] + w2*sp_y[u_i] + w3*sp_y[u_i+1] + w4*sp_y[u_i+2];

      if (intensity < 0.0) intensity = 0.0;
      if (intensity > 1.0) intensity = 1.0;

      raster[3*i]   += intensity * max_color[c][0];
      raster[3*i+1] += intensity * max_color[c][1];
      raster[3*i+2] += intensity * max_color[c][2];

      #ifdef SHOW_EARTH_AS_BLUE
      if (not_added) {
        if (values[i] < 0.0) raster[3*i+2] += 0.2;
      }
      #endif

    }

  #ifdef SHOW_EARTH_AS_BLUE
    not_added=0;
  #endif

  } /* end for c */

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
      if (raster[3*i] <= 0.0)        r = (unsigned char) 0;
      else if (raster[3*i] >= 1.0)   r = (unsigned char) 255;
      else                           r = (unsigned char) (255*raster[3*i]);
      if (raster[3*i+1] <= 0.0)      g = (unsigned char) 0;
      else if (raster[3*i+1] >= 1.0) g = (unsigned char) 255;
      else                           g = (unsigned char) (255*raster[3*i+1]);
      if (raster[3*i+2] <= 0.0)      b = (unsigned char) 0;
      else if (raster[3*i+2] >= 1.0) b = (unsigned char) 255;
      else                           b = (unsigned char) (255*raster[3*i+2]);
      putc(r,fp);
      putc(g,fp);
      putc(b,fp);
    }
  }
  fflush(fp);
  fclose(fp);
 
  free(raster);
  free(values);
}
