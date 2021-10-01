#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if 0
#define _SHOW_EARTH_AS_BLUE
#define _SWAP_BYTES
#endif

/* #define MAX_ENTRIES 256 */
#define MAX_ENTRIES 1024
/* #define MAX_ENTRIES 65536 */
#define MAX_POINTS   10

#define NUM_CHANNELS 3
char *channels[NUM_CHANNELS] = { "green", "magenta", "red" };
static float color_table[NUM_CHANNELS][MAX_ENTRIES][3];
static unsigned int gamma_table[MAX_ENTRIES];

/* control points used to create color lookup tables */
/* Green Channel */
#if 0
static int   g_ctl_pts = 4;
//static int   g_frame_num[MAX_POINTS] = {  0,  20, 240,  255};
  static int   g_frame_num[MAX_POINTS] = {  0,  80, 960,  1023};
//static int   g_frame_num[MAX_POINTS] = {  0,  61500,  65535};
static float g_red[MAX_POINTS]       = {0.0, 0.058, 0.29, 0.97};
static float g_green[MAX_POINTS]     = {0.0, 0.184, 0.92, 0.97};
static float g_blue[MAX_POINTS]    =   {0.0, 0.122, 0.61, 0.97};
static int   g_ctl_pts = 4;
static int   g_frame_num[MAX_POINTS] = {  0,  40, 960,  1023};
static float g_red[MAX_POINTS]       = {0.0, 0.0261, 0.29, 0.97};
static float g_green[MAX_POINTS]     = {0.0, 0.0828, 0.92, 0.97};
static float g_blue[MAX_POINTS]    =   {0.0, 0.0549, 0.61, 0.97};
#endif

static int   g_ctl_pts = 6;
static int   g_frame_num[MAX_POINTS] = {  0,    1,     10, 760, 960, 1023};
static float g_red[MAX_POINTS]       = {0.0, 0.029, 0.058, 0.29, 0.30, 0.98};
static float g_green[MAX_POINTS]     = {0.0, 0.092, 0.184, 0.92, 0.94, 0.98};
static float g_blue[MAX_POINTS]    =   {0.0, 0.061, 0.122, 0.61, 0.62, 0.98};
                                       /* 0%   10%   20%   100%  102%  Full */

/* Magenta Channel */
static int   m_ctl_pts = 3;
//static int   m_frame_num[MAX_POINTS] = {  0,  240,  255};
static int   m_frame_num[MAX_POINTS] = {  0,  960,  1023};
//static int   m_frame_num[MAX_POINTS] = {  0,  61500,  65535};
static float m_red[MAX_POINTS]       = {0.0, 0.95, 0.97};
static float m_green[MAX_POINTS]     = {0.0, 0.71, 0.97};
static float m_blue[MAX_POINTS]      = {0.0, 0.91, 0.97};

/* Red Channel */
static int   r_ctl_pts = 3;
//static int   r_frame_num[MAX_POINTS] = {  0,   240,  255};
static int   r_frame_num[MAX_POINTS] = {  0,   960,  1023};
//static int   r_frame_num[MAX_POINTS] = {  0,   61500,  65535};
static float r_red[MAX_POINTS]       = {0.0,  0.92, 0.97};
static float r_green[MAX_POINTS]     = {0.0,  0.18, 0.97};
static float r_blue[MAX_POINTS]      = {0.0, 0.078, 0.97};

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
    #if 0
      printf("%d: %d\n",i,gamma_table[i]);
    #endif
  }

}


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

  if (argc != 4 && argc != 10) {
    printf("Usage: %s filebase out.ppm gamma [Gmin Gmax Mmin Mmax Rmin Rmax]\n",
           argv[0]);
    exit(1);
  }

  gamma = atof(argv[3]);
  if (gamma <= 0.0) {
    printf("Gamma (%lf) must be > 0.0\n", gamma);
    exit(1);
  }
  max_entry = MAX_ENTRIES-1;
  set_gamma(MAX_ENTRIES, gamma, 1.0);

  if (argc == 10) {
    calc_min_max = 0;
    min[0] = atof(argv[4]);
    max[0] = atof(argv[5]);
    min[1] = atof(argv[6]);
    max[1] = atof(argv[7]);
    min[2] = atof(argv[8]);
    max[2] = atof(argv[9]);
    if (min >= max) {
       printf("Invalid range [%lf, %lf]\n", min, max);
       exit(1);
    }
  }

  /* create color tables */
  for ( i=1 ; i<g_ctl_pts ; i++ ) {
    low_r = g_red[i-1]; low_g = g_green[i-1]; low_b = g_blue[i-1];
    inc_r = (g_red[i]-g_red[i-1])     / (g_frame_num[i]-g_frame_num[i-1]);
    inc_g = (g_green[i]-g_green[i-1]) / (g_frame_num[i]-g_frame_num[i-1]);
    inc_b = (g_blue[i]-g_blue[i-1])   / (g_frame_num[i]-g_frame_num[i-1]);
    for ( j=g_frame_num[i-1] ; j<g_frame_num[i] ; j++ ) {
      color_table[0][j][0] = low_r; low_r += inc_r;
      color_table[0][j][1] = low_g; low_g += inc_g;
      color_table[0][j][2] = low_b; low_b += inc_b;
    }
  }
  color_table[0][g_frame_num[g_ctl_pts-1]][0] = g_red[g_ctl_pts-1];
  color_table[0][g_frame_num[g_ctl_pts-1]][1] = g_green[g_ctl_pts-1];
  color_table[0][g_frame_num[g_ctl_pts-1]][2] = g_blue[g_ctl_pts-1];
  for ( i=1 ; i<m_ctl_pts ; i++ ) {
    low_r = m_red[i-1]; low_g = m_green[i-1]; low_b = m_blue[i-1];
    inc_r = (m_red[i]-m_red[i-1])     / (m_frame_num[i]-m_frame_num[i-1]);
    inc_g = (m_green[i]-m_green[i-1]) / (m_frame_num[i]-m_frame_num[i-1]);
    inc_b = (m_blue[i]-m_blue[i-1])   / (m_frame_num[i]-m_frame_num[i-1]);
    for ( j=m_frame_num[i-1] ; j<m_frame_num[i] ; j++ ) {
      color_table[1][j][0] = low_r; low_r += inc_r;
      color_table[1][j][1] = low_g; low_g += inc_g;
      color_table[1][j][2] = low_b; low_b += inc_b;
    }
  }
  color_table[1][m_frame_num[m_ctl_pts-1]][0] = m_red[m_ctl_pts-1];
  color_table[1][m_frame_num[m_ctl_pts-1]][1] = m_green[m_ctl_pts-1];
  color_table[1][m_frame_num[m_ctl_pts-1]][2] = m_blue[m_ctl_pts-1];
  for ( i=1 ; i<r_ctl_pts ; i++ ) {
    low_r = r_red[i-1]; low_g = r_green[i-1]; low_b = r_blue[i-1];
    inc_r = (r_red[i]-r_red[i-1])     / (r_frame_num[i]-r_frame_num[i-1]);
    inc_g = (r_green[i]-r_green[i-1]) / (r_frame_num[i]-r_frame_num[i-1]);
    inc_b = (r_blue[i]-r_blue[i-1])   / (r_frame_num[i]-r_frame_num[i-1]);
    for ( j=r_frame_num[i-1] ; j<r_frame_num[i] ; j++ ) {
      color_table[2][j][0] = low_r; low_r += inc_r;
      color_table[2][j][1] = low_g; low_g += inc_g;
      color_table[2][j][2] = low_b; low_b += inc_b;
    }
  }
  color_table[2][r_frame_num[r_ctl_pts-1]][0] = r_red[r_ctl_pts-1];
  color_table[2][r_frame_num[r_ctl_pts-1]][1] = r_green[r_ctl_pts-1];
  color_table[2][r_frame_num[r_ctl_pts-1]][2] = r_blue[r_ctl_pts-1];

  for ( i=0 ; i<MAX_ENTRIES ; i++ ) {
    printf("%d = <%lf,%lf,%lf>\n", i, color_table[0][i][0],
                color_table[0][i][1], color_table[0][i][2]);
  }
#if 0
  for ( i=0 ; i<MAX_ENTRIES ; i++ ) {
    printf("%d = <%lf,%lf,%lf>\n", i, color_table[1][i][0],
                color_table[1][i][1], color_table[1][i][2]);
  }
  for ( i=0 ; i<MAX_ENTRIES ; i++ ) {
    printf("%d = <%lf,%lf,%lf>\n", i, color_table[2][i][0],
                color_table[2][i][1], color_table[2][i][2]);
  }
#endif

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

      index = gamma_table[(int) (max_entry * alpha)];

      raster[3*i]   += color_table[c][index][0];
      raster[3*i+1] += color_table[c][index][1];
      raster[3*i+2] += color_table[c][index][2];

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
