/* 06-03-26 added DO_NEG_FISHEYE option */
/* 06-03-26 added CLIP_CURTAINS option */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>          /* file handling w/integer file pointers */ 
#include <string.h>
#include "atomics.h"
#include "raster.h"
#include "render.h"
#include "err_msg.h"
#include "rays.h"
#include "view.h"
#include "timing.h"
#include "noise.h"
#include "stack.h"          /* includes octree.h */

/***** COMPUTATION OPTIONS *****/
#define _PRODUCTION_RUN
#define _FIND_BOUNDS
#define  DO_FISHEYE
#define _DO_90_DEGREES
#define  DO_NEG_FISHEYE
#define  CLIP_CURTAINS
#define  DO_EARTH
#define  DO_MIP_MAP
#define  DO_INCLINATION
#define  USE_OCTREE
#define  NOISE_TO_ELEVATION

/***** FUNCTION DECLARATIONS *****/
short green_deposition(short, double);
short magenta_deposition(short, double);
short red_deposition(short, double);
short (*deposition[3]) (short, double) =
 { green_deposition, magenta_deposition, red_deposition };
static short (*depo)(short, double);

/***** CONSTANTS USED IN THIS FILE ONLY *****/
#define EARTH_RADIUS 6371.01
static const double earth_rad2 = EARTH_RADIUS*EARTH_RADIUS;

static char *dashes = "---------------------------------------------------------------------------------------------\n";
static char *sharps = "#############################################################################\n";

#define TO_SHORT 10000.0
#define FROM_SHORT 0.0001
#define DEG2RAD (M_PI/180.0)
#define BYTE2ENERGY 31
#define MIN_ENERGY 3000
#define RAD2DEG (180.0/M_PI)
#define EARTH_TILT_RAD ((-23.4*M_PI)/180.0)
#define FUDGE 0.00001
#define COS_97_3 -0.127064608601
#define COS_90    0.0

static double itexture;
double maxtexture=211.0;

/*** these values are set in epr.c ***/

/* timing variables */
extern uLong totalStartTime;
extern uLong totalEndTime;
extern double totalTime;

double channelTime = 0;

/* bounding box (world coordinates) of aurora at any time */
extern const double VolumeXmin, VolumeXmax;
extern const double VolumeYmin, VolumeYmax;
extern const double VolumeZmin, VolumeZmax;

/* resolution of the grid to sample the curtain to */
extern const int super_x, super_y, super_z;
extern int sub_x, sub_y, sub_z;

/* properties of the curtain */
extern const double CurtainWidth;

/* defines the area where red/magenta curtains are visible */
/* last point of clouds.data in frame 22200 */
static const Point  RedFocusPoint = { 418.8, 5467.8, -3446.7 };
static const Point  MagentaFocusPoint = { 418.8, 5467.8, -3446.7 };
static const double RedFocusDist1 = 1500000.0;
static const double MagentaFocusDist1 = 1500000.0;
static const double RedFocusDist2 = 2000000.0;
static const double MagentaFocusDist2 = 2000000.0;

#define MIN_RED_ENERGY 500

/* set in render.c */
static double GreenHeight, MagentaHeight, RedHeight;
static double GreenOffset, RedOffset;

/* bounds of the deposition splines */
static const double MagentaLower = 6458.9;
static const double MagentaUpper = 6484.3;
static const double GreenLower = 6471.6;
static const double GreenUpper = 6789.1;
static const double RedLower = 6617.65;
static const double RedUpper = 6871.0;

/* elevations are mapped to U based on these constants */
#define NUM_DEP_PTS 81
#define ElevationMin 6370.0
#define ElevationRange 508.0
/* 80.0 / 508.0 */
#define ELEV_2_U 0.157480314961

static const float GreenCtlPts[NUM_DEP_PTS] = {
  /* version 06-02-27 */
  /* depB.txt */
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.635000,0.965000,1.172504,1.297507,1.355008,1.367508,1.365008,
  1.355008,1.327507,1.270006,1.175004,1.077501,0.967500,0.862500,0.770000,
  0.692500,0.630000,0.575000,0.530000,0.492500,0.457499,0.427499,0.397500,
  0.372500,0.347500,0.325000,0.300000,0.277500,0.255000,0.235000,0.212500,
  0.192500,0.172500,0.155000,0.137500,0.122500,0.110000,0.095000,0.082500,
  0.070000,0.060000,0.050000,0.040000,0.030000,0.022500,0.015000,0.007500,
  0.002500,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000 };
static const float MagentaCtlPts[NUM_DEP_PTS] = {
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.247500,
  0.912500,0.247500,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000 };
static const float RedCtlPts[NUM_DEP_PTS] = {
  /* version 06-02-27 */
  /* depB.txt */
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,
  0.002500,0.017500,0.037500,0.065000,0.100000,0.145000,0.197500,0.265000,
  0.362500,0.462500,0.565000,0.670000,0.755000,0.820000,0.874999,0.917499,
  0.957499,0.992499,1.024999,1.045000,1.060000,1.065000,1.060000,1.045000,
  1.019999,0.987499,0.937499,0.862499,0.750000,0.630000,0.510000,0.402500,
  0.317500,0.247500,0.182500,0.125000,0.082500,0.042500,0.007500,0.000000,
  0.000000 };

/* texture path to apply to the bottom of the curtain */
extern char *CurtainTexturePath;

/* what frame number to use */
extern const double CurtainFrameNum;

/* how many curtains are there? */
extern const int num_curtains;

/* sampling rate for each curtain */
extern const double seg_step_size[MAX_CURTAINS];

extern const double UVflow[MAX_CURTAINS];

/* holds raster/window information */
extern Screen s;


extern int num_frames[MAX_CURTAINS];
extern int frame_no[MAX_CURTAINS][MAX_FRAMES];
extern int num_points[MAX_CURTAINS][MAX_FRAMES];
extern double spX[MAX_CURTAINS][MAX_FRAMES][MAX_POINTS];
extern double spY[MAX_CURTAINS][MAX_FRAMES][MAX_POINTS];
extern double spZ[MAX_CURTAINS][MAX_FRAMES][MAX_POINTS];
extern float visible[MAX_CURTAINS][MAX_FRAMES];
extern float texture[MAX_CURTAINS][MAX_FRAMES];
extern float brightness[MAX_CURTAINS][MAX_POINTS];
extern float uv[MAX_CURTAINS][MAX_POINTS];
extern float **inclination;

/* calculated and only used in render.c */
static uLong vol_xres, vol_yres, vol_zres;
static uLong vol_xyres;
static uLong super_xy;
static uLong sub_xy;
static unsigned int shift_x;
static unsigned int shift_y;
static unsigned int shift_z;
static uLong mask_x;
static uLong mask_y;
static uLong mask_z;
static double target_area;
static double mip_frac;
static Ray apex;

/*** set in calc_volume() and used in render_image() ***/
static double VxScale, VyScale, VzScale;
static double VolXrange, VolXrangeDivXres, VolXresDivXrange;
static double VolYrange, VolYrangeDivYres, VolYresDivYrange;
static double VolZrange, VolZrangeDivZres, VolZresDivZrange;
static double Vwidth;
static double Vheight;
static double Vdepth;
static double Vmin;

static int RENDER_YMIN;
static int RENDER_YMAX;
static int Y_STOP;
static int NUM_SLABS = 16; 
static double SlabYrange;

static long int super_num_samples;
static Point low, up;
static Point slab_low, slab_up;

/* Octree information */
OCTREE octree;
OCTREE blanknode;
OCTREE *root = &octree;
OCTREE *x_node;
OCTREE *y_node;
OCTREE *xy_node;
OCTREE *z_node;
OCTREE *xz_node;
OCTREE *yz_node;
OCTREE *xyz_node;
int oct_depth;
int original_oct_depth; //allows for non power of 2 supers
unsigned int num_children; /* number of nodes created by octree */

/* Counters used for volume stats */
unsigned int        sub_num_samples; //declared extern in octree.h
static unsigned int num_inserts = 0; /* number of insertions, ==0, skip render */
unsigned int        num_sub_allocated = 0;
static unsigned int num_overwrites = 0;

#ifdef FIND_BOUNDS
static double Bxmin, Bxmax;
static double Bymin, Bymax;
static double Bzmin, Bzmax;
static double gBxmin = INF;
static double gBxmax = -INF;
static double gBymin = INF;
static double gBymax = -INF;
static double gBzmin = INF;
static double gBzmax = -INF;
static uLong vox_count=0;
#endif

#ifdef DO_MIP_MAP
extern int LowerRes[MAX_CURTAINS];
extern int HigherRes[MAX_CURTAINS];
TurbMap higher[MAX_CURTAINS]; /* higher res (than needed) texture map */
TurbMap lower[MAX_CURTAINS];  /* lower res (than needed) texture map */
#endif

/*---------------------------------------------------------------------------*/

#ifdef DO_MIP_MAP

int get_turbulence_map(
int    curtain_num,
double frame_num
)
{
  FILE *fp;
  char tmpname[1024];
  int  xres, yres;
  int  maxval;
  int  f0, f1;
  int  frame_no;
  double frac;
  int  u,v;
  unsigned char b;
  short *tmp;
  short *orig;
  int i;
  int num_samples;

  f0 = (int) frame_num;
  frac = frame_num - f0;
  f1 = f0+1;

  /* read in the lower resolution texture map at previous integral frame */
  sprintf(tmpname, CurtainTexturePath, LowerRes[curtain_num], f0);

  /* try to open the texture image */
  if ((fp = fopen(tmpname, "r")) == 0)
    { printf("Can't open %s\n", tmpname); exit(1); }

  /* get pgm header */
  fscanf(fp, "P5\n");
  if (fscanf(fp, "%d %d\n%d\n", &xres, &yres, &maxval) != 3)
    { printf("Error reading header\n"); exit(1); }
  if (xres <= 0 || yres <= 0)
    { printf("Invalid size for texture map\n"); exit(1); }

  /* malloc memory for mip-mapped texture */
  lower[curtain_num].values = (short *) malloc( sizeof(short)*xres*yres );
  if (!(lower[curtain_num].values))
    { printf("Can't malloc memory for texture map\n"); exit(1); }

  lower[curtain_num].Ures = xres;
  lower[curtain_num].Vres = yres;

  /* read the texture map in */
  for ( v=0 ; v<yres ; v++ ) {
    for ( u=0 ; u<xres ; u++ ) {
      b = (u_char) getc(fp);
      lower[curtain_num].values[v+u*yres] = ((short) b) * BYTE2ENERGY;
    }
  }
  fclose(fp);

#if 0
  printf("  Processed texture %s (%dx%d)\n",
         tmpname,lower[curtain_num].Ures,lower[curtain_num].Vres);
#endif

  /* read in the lower resolution texture map at next integral frame */
  sprintf(tmpname, CurtainTexturePath, LowerRes[curtain_num], f1);

  /* try to open the texture image */
  if ((fp = fopen(tmpname, "r")) == 0)
    { printf("Can't open %s\n", tmpname); exit(1); }

  /* get pgm header */
  fscanf(fp, "P5\n");
  if (fscanf(fp, "%d %d\n%d\n", &xres, &yres, &maxval) != 3)
    { printf("Error reading header\n"); exit(1); }
  if (xres <= 0 || yres <= 0)
    { printf("Invalid size for texture map\n"); exit(1); }

  /* malloc memory for mip-mapped texture */
  tmp = (short *) malloc( sizeof(short)*xres*yres );
  if (!tmp)
    { printf("Can't malloc memory for texture map\n"); exit(1); }

  if (xres != lower[curtain_num].Ures || yres != lower[curtain_num].Vres)
    { printf("Resolutions do not match\n"); exit(1); }

  /* read the texture map in */
  for ( v=0 ; v<yres ; v++ ) {
    for ( u=0 ; u<xres ; u++ ) {
      b = (u_char) getc(fp);
      tmp[v+u*yres] = ((short) b) * BYTE2ENERGY;
    }
  }
  fclose(fp);
#if 0
  printf("  Processed texture %s (%dx%d)\n",tmpname,xres,yres);
#endif

  /* interpolate between the 2 frames */
  num_samples = xres*yres;
  orig = lower[curtain_num].values;
  for ( i=0 ; i<num_samples ; i++ )
    orig[i] += (short) (frac * (tmp[i] - orig[i]));

  free(tmp);

  /* read in the higher resolution texture map at previous integral frame */
  sprintf(tmpname, CurtainTexturePath, HigherRes[curtain_num], f0);

  /* try to open the texture image */
  if ((fp = fopen(tmpname, "r")) == 0)
    { printf("Can't open %s\n", tmpname); exit(1); }

  /* get pgm header */
  fscanf(fp, "P5\n");
  if (fscanf(fp, "%d %d\n%d\n", &xres, &yres, &maxval) != 3)
    { printf("Error reading header\n"); exit(1); }
  if (xres <= 0 || yres <= 0)
    { printf("Invalid size for texture map\n"); exit(1); }

  /* malloc memory for mip-mapped texture */
  higher[curtain_num].values = (short *) malloc( sizeof(short)*xres*yres );
  if (!(higher[curtain_num].values))
    { printf("Can't malloc memory for texture map\n"); exit(1); }

  higher[curtain_num].Ures = xres;
  higher[curtain_num].Vres = yres;

  /* read the texture map in */
  for ( v=0 ; v<yres ; v++ ) {
    for ( u=0 ; u<xres ; u++ ) {
      b = (u_char) getc(fp);
      higher[curtain_num].values[v+u*yres] = ((short) b) * BYTE2ENERGY;
    }
  }
  fclose(fp);

#if 0
  printf("  Processed texture %s (%dx%d)\n",
         tmpname,higher[curtain_num].Ures,higher[curtain_num].Vres);
#endif

  /* read in the higher resolution texture map at next integral frame */
  sprintf(tmpname, CurtainTexturePath, HigherRes[curtain_num], f1);

  /* try to open the texture image */
  if ((fp = fopen(tmpname, "r")) == 0)
    { printf("Can't open %s\n", tmpname); exit(1); }

  /* get pgm header */
  fscanf(fp, "P5\n");
  if (fscanf(fp, "%d %d\n%d\n", &xres, &yres, &maxval) != 3)
    { printf("Error reading header\n"); exit(1); }
  if (xres <= 0 || yres <= 0)
    { printf("Invalid size for texture map\n"); exit(1); }

  /* malloc memory for mip-mapped texture */
  tmp = (short *) malloc( sizeof(short)*xres*yres );
  if (!tmp)
    { printf("Can't malloc memory for texture map\n"); exit(1); }

  if (xres != higher[curtain_num].Ures || yres != higher[curtain_num].Vres)
    { printf("Resolutions do not match\n"); exit(1); }

  /* read the texture map in */
  for ( v=0 ; v<yres ; v++ ) {
    for ( u=0 ; u<xres ; u++ ) {
      b = (u_char) getc(fp);
      tmp[v+u*yres] = ((short) b) * BYTE2ENERGY;
    }
  }
  fclose(fp);
#if 0
  printf("  Processed texture %s (%dx%d)\n",tmpname,xres,yres);
#endif

  /* interpolate between the 2 frames */
  num_samples = xres*yres;
  orig = higher[curtain_num].values;
  for ( i=0 ; i<num_samples ; i++ )
    orig[i] += (short) (frac * (tmp[i] - orig[i]));

  free(tmp);

}
#endif

/*---------------------------------------------------------------------------*/

Point polar2cartesian(
double lat,
double lon
)
{
  /* assumes that long=0 is at <-1, 0, 0> */
  /* assumes that lat=90.0 is <0, 1, 0> */
  Point out;
  double phi, theta;
  double sphi;

  phi = (90.0 - lat) * DEG2RAD;
  theta = (lon - 90.0) * DEG2RAD;
  sphi = sin(phi);
  out.y = cos(phi);
  out.x = sphi * sin(theta);
  out.z = -sphi * -cos(theta);
  out = normalize(out);

  return out;
}


#ifdef DO_INCLINATION
Vector adjust_inclination(
Vector in
)
{
  Vector out;   /* output vector */
  double phi;   /* angle down from <0,1,0> (north pole) */
  double theta; /* angle around from <0,0,1> where long=-180 */
  double Dist;  /* length of vector <x,z> */
  double lat;   /* latitude 90 == <0,1,0> and 0 == <-,0,-> */
  double lon;   /* longitude -180==<0,0,1> 0==<0,0,-1> 180==<0,0,1> */
  int    ila;   /* truncated lat to get table lookup value */
  int    ilo;   /* truncated lon to get table lookup value */
  double sphi;  /* sin(phi) */
  double lo_frac, la_frac;
  double i00, i01, i10, i11, ix0, ix1, ixx;
  double tlo, tla;
Vector NorthPole;
double Ncostheta;
Vector S;

  /* determine the latitude/longitude of this point */
  Dist = in.x*in.x + in.z*in.z;
  if (Dist < 0.000001) {
    /* north pole - shouldn't really happen with aurora */
    phi = 0.0;
    theta = 0.0;
  } else {
    Dist = sqrt(Dist);
    phi = acos(in.y);
    if (in.x>=0.0) theta = M_PI - acos(in.z/Dist);
    else        theta = acos(in.z/Dist) - M_PI;
  }
  lat = 90.0 - (phi * RAD2DEG); /* invert as north pole is lat=90 */;
  lon = (theta * RAD2DEG);      /* long=0 is down -z axis */

#if 0
printf("Phi(%.2lf,%.2lf) ", lat, lon);
#endif

  if (lat < LOWER_LAT_RANGE) {

    /* look up based on long at LOWER_LAT_RANGE */
    tlo = lon + 180.0;
    ilo = (int) tlo;
    lo_frac = tlo - ilo;

    i00 = inclination[LOWER_LAT][ilo];
    i10 = inclination[LOWER_LAT][(ilo+1)%360];

    ixx = i00 + (i10 - i00)*lo_frac;

  } else {

    tlo = lon + 180.0;
    ilo = (int) tlo;
    lo_frac = tlo - ilo;

    tla = 90.0 - lat;
    ila = (int) tla;
    la_frac = tla - ila;

    i00 = inclination[ila][ilo];
    i10 = inclination[ila][(ilo+1)%360];
    i01 = inclination[ila+1][ilo];
    i11 = inclination[ila+1][(ilo+1)%360];

    ix0 = i00 + (i10 - i00)*lo_frac;
    ix1 = i01 + (i11 - i01)*lo_frac;
    ixx = ix0 + (ix1 - ix0)*la_frac;

  }

#if 0
printf("= %.2lf ", ixx);
#endif

/*phi = (lat - (90.0 - ixx)) * DEG2RAD;*/
  phi = (90.0 - ixx) * DEG2RAD;

#if 0
printf(" -> phi=%lf ", 90.0 - ixx);
#endif

/* will make this the geomag north pole for now */
NorthPole.x = 0.0;
NorthPole.y = 1.0;
NorthPole.z = 0.0;

Ncostheta = dot_product(in, NorthPole);
#if 0
printf(" -> Ncostheta=%lf ", Ncostheta);
#endif
S = normalize(subtract(multiply(Ncostheta,in), NorthPole));
out = add(multiply(Ncostheta, in), multiply(sin(phi), S));
out = normalize(out);

#if 0
  printf("V=<%.3lf,%.3lf,%.3lf> ", in.x, in.y, in.z);
  printf("lat=%.2lf lon=%.2lf => sub %.2lf => new lat %.2lf\n",
         lat, lon, 90.0-ixx, lat - (90.0-ixx));
#endif

/*
  out.y = cos(phi);
  sphi = sin(phi);
  out.x = sphi * sin(theta);
  out.z = - sphi * cos(theta);
  out = normalize(out);
*/

#if 0
printf("DP(in,out) = %lf\n", dot_product(in,out));
#endif

  return out;
}
#endif

/*---------------------------------------------------------------------------*/

void set_vol(
short value,
uLong x,
uLong y,
uLong z
)
{
  int c;
  int subblock;
  OCTREE *node;
  double subblocksize;
  double octree_size;
  double subblocks_size;

#if 0
if (value < 0 || value > 10000)
  printf("Setting voxel <%d,%d,%d> to %d\n", x, y, z, value);
#endif

#if 0
  /* do range checking */
  if (x < 0 || x >= vol_xres) { printf("x out of range\n"); exit(1); }
  if (y < 0 || y >= vol_yres) { printf("y out of range\n"); exit(1); }
  if (z < 0 || z >= vol_zres) { printf("z out of range\n"); exit(1); }
#endif

  /* don't set voxels with value=0 */
  if (value == 0) return;

  num_inserts++;
  node = octree_insert(&octree, oct_depth, x>>shift_x, y>>shift_y, z>>shift_z);
  subblock = (x&mask_x) + (y&mask_y)*sub_x + (z&mask_z)*sub_xy;

  if(node->data == NULL) {
    num_sub_allocated++;
    if ((node->data = (short *) malloc(sizeof(short)*sub_num_samples)) == NULL) {
      subblocksize = sizeof(short) * sub_num_samples;
      octree_size  = (double)(((num_children+1)*sizeof(OCTREE))/1048576.0);
      subblocks_size = (double)(num_sub_allocated*subblocksize/1048576.0);
      printf("ERROR: Can't allocate subblock: %d at (%d %d %d, level: %d)\n",
             num_sub_allocated, node->xLoc, node->yLoc, node->zLoc, node->depth);
      printf("   Octree size: %.02f MB, Subblocks size: %.02f MB, TOTAL: %.02f MB\n",
             octree_size, subblocks_size, subblocks_size+octree_size);
      exit(1);
    }
    for ( c=0 ; c<sub_num_samples ; c++ ) node->data[c] = 0.0;
  }

  if (node->data[subblock] != 0) num_overwrites++;

  node->data[subblock] = value;

}

/*---------------------------------------------------------------------------*/

int get_energy(
double *energy,
OCTREE *node,        /* We assume that node's x,y,z == s_0[3] */
unsigned long x0,
unsigned long y0,
unsigned long z0,
unsigned long s_0[3],
unsigned long s_1[3]
)
{
  unsigned char which_nodes = 0;
  which_nodes += (s_0[0] == s_1[0]) ? 0 : 4;
  which_nodes += (s_0[1] == s_1[1]) ? 0 : 2;
  which_nodes += (s_0[2] == s_1[2]) ? 0 : 1;
  unsigned int vox_000 = (x0&mask_x)     + (y0&mask_y)*sub_x     + (z0&mask_z)*sub_xy;
  unsigned int vox_100 = ((x0+1)&mask_x) + (y0&mask_y)*sub_x     + (z0&mask_z)*sub_xy;
  unsigned int vox_010 = (x0&mask_x)     + ((y0+1)&mask_y)*sub_x + (z0&mask_z)*sub_xy;
  unsigned int vox_110 = ((x0+1)&mask_x) + ((y0+1)&mask_y)*sub_x + (z0&mask_z)*sub_xy;
  unsigned int vox_001 = (x0&mask_x)     + (y0&mask_y)*sub_x     + ((z0+1)&mask_z)*sub_xy;
  unsigned int vox_101 = ((x0+1)&mask_x) + (y0&mask_y)*sub_x     + ((z0+1)&mask_z)*sub_xy;
  unsigned int vox_011 = (x0&mask_x)     + ((y0+1)&mask_y)*sub_x + ((z0+1)&mask_z)*sub_xy;
  unsigned int vox_111 = ((x0+1)&mask_x) + ((y0+1)&mask_y)*sub_x + ((z0+1)&mask_z)*sub_xy;
  switch(which_nodes) {
    case 0: //000 all data in one node 
      if (node->data) {
        energy[0] = node->data[vox_000]*FROM_SHORT;
        energy[1] = node->data[vox_100]*FROM_SHORT;
        energy[2] = node->data[vox_010]*FROM_SHORT;
        energy[3] = node->data[vox_110]*FROM_SHORT;
        energy[4] = node->data[vox_001]*FROM_SHORT;
        energy[5] = node->data[vox_101]*FROM_SHORT;
        energy[6] = node->data[vox_011]*FROM_SHORT;
        energy[7] = node->data[vox_111]*FROM_SHORT;
        return 1;
      }
    return 0; /* all the data should be here, but its null */
    break;

    case 1: //001 only z differs
      z_node = octree_traverse(&octree, oct_depth, s_0[0], s_0[1], s_1[2]);
      if (z_node->data || node->data) {
        energy[0] = (node->data)   ? node->data[vox_000]*FROM_SHORT   : 0;
        energy[1] = (node->data)   ? node->data[vox_100]*FROM_SHORT   : 0;
        energy[2] = (node->data)   ? node->data[vox_010]*FROM_SHORT   : 0;
        energy[3] = (node->data)   ? node->data[vox_110]*FROM_SHORT   : 0;
        energy[4] = (z_node->data) ? z_node->data[vox_001]*FROM_SHORT : 0;
        energy[5] = (z_node->data) ? z_node->data[vox_101]*FROM_SHORT : 0;
        energy[6] = (z_node->data) ? z_node->data[vox_011]*FROM_SHORT : 0;
        energy[7] = (z_node->data) ? z_node->data[vox_111]*FROM_SHORT : 0;
        return 1;
      }
      return 0; //both nodes are null! 
    break;

    case 2: //010 only y differs
      y_node = octree_traverse(&octree, oct_depth, s_0[0], s_1[1], s_0[2]);
      if (y_node->data || node->data) {
        energy[0] = (node->data)   ? node->data[vox_000]*FROM_SHORT   : 0;
        energy[1] = (node->data)   ? node->data[vox_100]*FROM_SHORT   : 0;
        energy[2] = (y_node->data) ? y_node->data[vox_010]*FROM_SHORT : 0;
        energy[3] = (y_node->data) ? y_node->data[vox_110]*FROM_SHORT : 0;
        energy[4] = (node->data)   ? node->data[vox_001]*FROM_SHORT   : 0;
        energy[5] = (node->data)   ? node->data[vox_101]*FROM_SHORT   : 0;
        energy[6] = (y_node->data) ? y_node->data[vox_011]*FROM_SHORT : 0;
        energy[7] = (y_node->data) ? y_node->data[vox_111]*FROM_SHORT : 0;
        return 1;
      }
      return 0; //both nodes are null! 
    break;

    case 3: //011 y and z differ
      y_node  = octree_traverse(&octree, oct_depth, s_0[0], s_1[1], s_0[2]);
      z_node  = octree_traverse(&octree, oct_depth, s_0[0], s_0[1], s_1[2]);
      yz_node = octree_traverse(&octree, oct_depth, s_0[0], s_1[1], s_1[2]);
      if (node->data || y_node->data || z_node->data || yz_node->data) {
        energy[0] = (node->data)    ? node->data[vox_000]*FROM_SHORT    : 0;
        energy[1] = (node->data)    ? node->data[vox_100]*FROM_SHORT    : 0;
        energy[2] = (y_node->data)  ? y_node->data[vox_010]*FROM_SHORT  : 0;
        energy[3] = (y_node->data)  ? y_node->data[vox_110]*FROM_SHORT  : 0;
        energy[4] = (z_node->data)  ? z_node->data[vox_001]*FROM_SHORT  : 0;
        energy[5] = (z_node->data)  ? z_node->data[vox_101]*FROM_SHORT  : 0;
        energy[6] = (yz_node->data) ? yz_node->data[vox_011]*FROM_SHORT : 0;
        energy[7] = (yz_node->data) ? yz_node->data[vox_111]*FROM_SHORT : 0;
        return 1;
      }
      return 0; //both nodes are null
    break;

    case 4: //100 only x differs
      x_node = octree_traverse(&octree, oct_depth, s_1[0], s_0[1], s_0[2]);
      if (x_node->data || node->data) {
        energy[0] = (node->data)   ? node->data[vox_000]*FROM_SHORT   : 0;
        energy[1] = (x_node->data) ? x_node->data[vox_100]*FROM_SHORT : 0;
        energy[2] = (node->data)   ? node->data[vox_010]*FROM_SHORT   : 0;
        energy[3] = (x_node->data) ? x_node->data[vox_110]*FROM_SHORT : 0;
        energy[4] = (node->data)   ? node->data[vox_001]*FROM_SHORT   : 0;
        energy[5] = (x_node->data) ? x_node->data[vox_101]*FROM_SHORT : 0;
        energy[6] = (node->data)   ? node->data[vox_011]*FROM_SHORT   : 0;
        energy[7] = (x_node->data) ? x_node->data[vox_111]*FROM_SHORT : 0;
        return 1;
      }
      return 0; //both nodes are null!
    break;

    case 5: //101 x and z differ
      x_node = octree_traverse(&octree, oct_depth, s_1[0], s_0[1], s_0[2]);
      z_node = octree_traverse(&octree, oct_depth, s_0[0], s_0[1], s_1[2]);
      xz_node = octree_traverse(&octree, oct_depth, s_1[0], s_0[1], s_1[2]);
      if (node->data || x_node->data || z_node->data || xz_node->data) {
        energy[0] = (node->data)    ? node->data[vox_000]*FROM_SHORT    : 0;
        energy[1] = (x_node->data)  ? x_node->data[vox_100]*FROM_SHORT  : 0;
        energy[2] = (node->data)    ? node->data[vox_010]*FROM_SHORT    : 0;
        energy[3] = (x_node->data)  ? x_node->data[vox_110]*FROM_SHORT  : 0;
        energy[4] = (z_node->data)  ? z_node->data[vox_001]*FROM_SHORT  : 0;
        energy[5] = (xz_node->data) ? xz_node->data[vox_101]*FROM_SHORT : 0;
        energy[6] = (z_node->data)  ? z_node->data[vox_011]*FROM_SHORT  : 0;
        energy[7] = (xz_node->data) ? xz_node->data[vox_111]*FROM_SHORT : 0;
        return 1;
      }
      return 0; //all nodes are null
    break;

    case 6: //110 x & y differ, so need 3 other nodes besides current
      x_node  = octree_traverse(&octree, oct_depth, s_1[0], s_0[1], s_0[2]);
      y_node  = octree_traverse(&octree, oct_depth, s_0[0], s_1[1], s_0[2]);
      xy_node = octree_traverse(&octree, oct_depth, s_1[0], s_1[1], s_0[2]);
      if (node->data || x_node->data || y_node->data || xy_node->data) {
        energy[0] = (node->data)    ? node->data[vox_000]*FROM_SHORT    : 0;
        energy[1] = (x_node->data)  ? x_node->data[vox_100]*FROM_SHORT  : 0;
        energy[2] = (y_node->data)  ? y_node->data[vox_010]*FROM_SHORT  : 0;
        energy[3] = (xy_node->data) ? xy_node->data[vox_110]*FROM_SHORT : 0;
        energy[4] = (node->data)    ? node->data[vox_001]*FROM_SHORT    : 0;
        energy[5] = (x_node->data)  ? x_node->data[vox_101]*FROM_SHORT  : 0;
        energy[6] = (y_node->data)  ? y_node->data[vox_011]*FROM_SHORT  : 0;
        energy[7] = (xy_node->data) ? xy_node->data[vox_111]*FROM_SHORT : 0;
        return 1;
      }
      return 0; //all are null
    break;

    case 7: //111 x, y and z differ... THE WORST CASE SCENARIO MY FRIENDS!
      x_node   = octree_traverse(&octree, oct_depth, s_1[0], s_0[1], s_0[2]);
      y_node   = octree_traverse(&octree, oct_depth, s_0[0], s_1[1], s_0[2]);
      xy_node  = octree_traverse(&octree, oct_depth, s_1[0], s_1[1], s_0[2]);
      z_node   = octree_traverse(&octree, oct_depth, s_0[0], s_0[1], s_1[2]);
      xz_node  = octree_traverse(&octree, oct_depth, s_1[0], s_0[1], s_1[2]);
      yz_node  = octree_traverse(&octree, oct_depth, s_0[0], s_1[1], s_1[2]);
      xyz_node = octree_traverse(&octree, oct_depth, s_1[0], s_1[1], s_1[2]);
      if (node->data || x_node->data || y_node->data || xy_node->data ||
         z_node->data || xz_node->data || yz_node->data || xyz_node->data) {
        energy[0] = (node->data)     ? node->data[vox_000]*FROM_SHORT     : 0;
        energy[1] = (x_node->data)   ? x_node->data[vox_100]*FROM_SHORT   : 0;
        energy[2] = (y_node->data)   ? y_node->data[vox_010]*FROM_SHORT   : 0;
        energy[3] = (xy_node->data)  ? xy_node->data[vox_110]*FROM_SHORT  : 0;
        energy[4] = (z_node->data)   ? z_node->data[vox_001]*FROM_SHORT   : 0;
        energy[5] = (xz_node->data)  ? xz_node->data[vox_101]*FROM_SHORT  : 0;
        energy[6] = (yz_node->data)  ? yz_node->data[vox_011]*FROM_SHORT  : 0;
        energy[7] = (xyz_node->data) ? xyz_node->data[vox_111]*FROM_SHORT : 0;
        return 1;
      }
      return 0; //all nodes are null
    break;

  } /* End of switch(which_case) */

  printf("It's impossible to reach this place.  If we get here we will die.\n");
  exit(1);

} /* end get_energy() */

/*---------------------------------------------------------------------------*/

Point world2voxel(Point w)
{
  Point v;
  v.x = (w.x - VolumeXmin) * VolXresDivXrange;
  v.y = (w.y - VolumeYmin) * VolYresDivYrange;
  v.z = (w.z - VolumeZmin) * VolZresDivZrange;
  return v;
}

/*---------------------------------------------------------------------------*/

Point voxel2world(Point v)
{
  Point w;
  w.x = VolumeXmin + (v.x * VolXrangeDivXres);
  w.y = VolumeYmin + (v.y * VolYrangeDivYres);
  w.z = VolumeZmin + (v.z * VolZrangeDivZres);
  return w;
}

/*---------------------------------------------------------------------------*/

float interp_brightness(int curtain_no, double u)
{
  float result;
  int i;
  double u1, u2, u3;
  double w1, w2, w3, w4;

  i = (int) u;
  if (i<1 || i>= 103-2) return 0.0;

  u1 = u - i;
  u2 = u1 * u1;
  u3 = u2 * u1;

  w1 = -0.5*u3 +     u2 - 0.5*u1;
  w2 =  1.5*u3 - 2.5*u2 + 1.0;
  w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
  w4 =  0.5*u3 - 0.5*u2;

  result = w1 * brightness[curtain_no][i-1]
         + w2 * brightness[curtain_no][i]
         + w3 * brightness[curtain_no][i+1]
         + w4 * brightness[curtain_no][i+2];

  if (result < 0.0) return 0.0;
  else if (result > 1.0) return 1.0;
  else return result;

} /* end interp_brightness() */

/*---------------------------------------------------------------------------*/

float interp_uv(int curtain_no, double u)
{
  float result;
  int i;
  double u1, u2, u3;
  double w1, w2, w3, w4;

  i = (int) u;

  if (i<1) return uv[curtain_no][1];
  if (i>=103-2) return uv[curtain_no][103-2];

  u1 = u - i;
  u2 = u1 * u1;
  u3 = u2 * u1;

  w1 = -0.5*u3 +     u2 - 0.5*u1;
  w2 =  1.5*u3 - 2.5*u2 + 1.0;
  w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
  w4 =  0.5*u3 - 0.5*u2;

  result = w1 * uv[curtain_no][i-1]
         + w2 * uv[curtain_no][i]
         + w3 * uv[curtain_no][i+1]
         + w4 * uv[curtain_no][i+2];

  return result;

} /* end interp_uv() */

/*---------------------------------------------------------------------------*/

Point spline_point(int s, double frame_num, double u)
{
  Point P;
  int i;
  double u1, u2, u3;
  double w1, w2, w3, w4;
  int f0, f1;
  double frac;

  #if 0
  printf("sp: %d %.4lf %.4lf\n", s, frame_num, u);
  #endif

  f0 = (int) frame_num;
  f1 = f0 + 1;
  frac = frame_num - f0;
  i = (int) u;
  if (i < 1 || i>=num_points[s][f0]-2 || i>=num_points[s][f1]-2)
    { P.x = P.y = P.z = 0.0; return P; }

  u1 = u - i;
  u2 = u1 * u1;
  u3 = u2 * u1;

  w1 = -0.5*u3 +     u2 - 0.5*u1;
  w2 =  1.5*u3 - 2.5*u2 + 1.0;
  w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
  w4 =  0.5*u3 - 0.5*u2;

  P.x = w1 * (spX[s][f0][i-1] + (spX[s][f1][i-1] - spX[s][f0][i-1]) * frac)
      + w2 * (spX[s][f0][i]   + (spX[s][f1][i]   - spX[s][f0][i]  ) * frac)
      + w3 * (spX[s][f0][i+1] + (spX[s][f1][i+1] - spX[s][f0][i+1]) * frac)
      + w4 * (spX[s][f0][i+2] + (spX[s][f1][i+2] - spX[s][f0][i+2]) * frac);

  P.y = w1 * (spY[s][f0][i-1] + (spY[s][f1][i-1] - spY[s][f0][i-1]) * frac)
      + w2 * (spY[s][f0][i]   + (spY[s][f1][i]   - spY[s][f0][i]  ) * frac)
      + w3 * (spY[s][f0][i+1] + (spY[s][f1][i+1] - spY[s][f0][i+1]) * frac)
      + w4 * (spY[s][f0][i+2] + (spY[s][f1][i+2] - spY[s][f0][i+2]) * frac);

  P.z = w1 * (spZ[s][f0][i-1] + (spZ[s][f1][i-1] - spZ[s][f0][i-1]) * frac)
      + w2 * (spZ[s][f0][i]   + (spZ[s][f1][i]   - spZ[s][f0][i]  ) * frac)
      + w3 * (spZ[s][f0][i+1] + (spZ[s][f1][i+1] - spZ[s][f0][i+1]) * frac)
      + w4 * (spZ[s][f0][i+2] + (spZ[s][f1][i+2] - spZ[s][f0][i+2]) * frac);
  
  return P;

} /* end spline_point() */

/*---------------------------------------------------------------------------*/

short magenta_deposition(
short  energy,
double elevation
)
{
  int i;
  double u1, u2, u3;
  double w1, w2, w3, w4;
  double weight;
  double u;

  if (elevation < MagentaLower || elevation > MagentaUpper) return 0;

  u = (elevation - ElevationMin) * ELEV_2_U;

  i = (int) u;

  u1 = u - i; u2 = u1 * u1; u3 = u2 * u1;
  w1 = -0.5*u3 +     u2 - 0.5*u1;
  w2 =  1.5*u3 - 2.5*u2 + 1.0;
  w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
  w4 =  0.5*u3 - 0.5*u2;

  weight = w1*MagentaCtlPts[i-1] +
           w2*MagentaCtlPts[i] +
           w3*MagentaCtlPts[i+1] +
           w4*MagentaCtlPts[i+2];

  return weight * energy;

} /* end magenta_deposition() */

/*---------------------------------------------------------------------------*/

short green_deposition(
short  energy,
double elevation
)
{
  int i;
  double u1, u2, u3;
  double weight;
  double u;
  double adj_elev;
  double percentage;

  if (energy <= 0) return 0;

  if (elevation < GreenLower || elevation > GreenUpper) return 0;

  /* add to elevation based on energy at bottom  e=0->max, e=max->0.0*/
  percentage = 1.25 - 0.25 * energy * FROM_SHORT;

  /* this will run thru the spline deposition function faster */
  adj_elev = GreenLower + (elevation - GreenLower) * percentage;

  if (adj_elev > GreenUpper) return 0;

  u = (adj_elev - ElevationMin) * ELEV_2_U;

#if 0
  /* shorten curtain based on amount of energy */
  adj_elev = GreenLower + (elevation - GreenLower) * (0.75 + 0.25 * energy * FROM_SHORT);
  u = (adj_elev - ElevationMin) * ELEV_2_U;
#endif

  i = (int) u;

  u1 = u - i; u2 = u1 * u1; u3 = u2 * u1;

  weight = (-0.5*u3 + u2 - 0.5*u1)     * GreenCtlPts[i-1] +
           (1.5*u3 - 2.5*u2 + 1.0)     * GreenCtlPts[i] +
           (-1.5*u3 + 2.0*u2 + 0.5*u1) * GreenCtlPts[i+1] +
           (0.5*u3 - 0.5*u2)           * GreenCtlPts[i+2];

  if (weight < 0.0) return 0;
  else              return weight * energy;

} /* end green_deposition() */

/*---------------------------------------------------------------------------*/

short red_deposition(
short  energy,
double elevation
)
{
  int i;
  double u1, u2, u3;
  double weight, u, adj_elev, percentage;

  if (energy <= MIN_RED_ENERGY) return 0;

  if (elevation < RedLower || elevation > RedUpper) return 0;

  percentage = 1.25 - 0.25 * energy * FROM_SHORT;

  energy -= MIN_RED_ENERGY;

  adj_elev = GreenLower + (elevation - GreenLower) * percentage;

  if (adj_elev > RedUpper) return 0;

  u = (adj_elev - ElevationMin) * ELEV_2_U;

  i = (int) u;
  u1 = u - i; u2 = u1 * u1; u3 = u2 * u1;

  weight = (-0.5*u3 + u2 - 0.5*u1)*RedCtlPts[i-1] +
           ( 1.5*u3 - 2.5*u2 + 1.0)*RedCtlPts[i] +
           (-1.5*u3 + 2.0*u2 + 0.5*u1)*RedCtlPts[i+1] +
           ( 0.5*u3 - 0.5*u2)*RedCtlPts[i+2];

  if (weight < 0.0) return 0;
  else              return weight * energy;

} /* end red_deposition() */

/*---------------------------------------------------------------------------*/

void scan_convert(
Vertex P1,
Vertex P2,
Vertex P3,
double y,
int curtain,
int color_band
)
{
  Vertex tmp;
  Vertex A, B, C, D;
  double x0, x1, xs0, xs1; /* used to interpolate x along edges */
  double u0, u1, us0, us1; /* used to interpolate u along edges */
  double v0, v1, vs0, vs1; /* used to interpolate v along edges */
  double d0, d1, ds0, ds1; /* used to interpolate brightness along edges */
  double e0, e1, es0, es1; /* used to interpolate elevation along edges */
  double ru, rus0; /* used to interpolate u along the scan line */
  double rv, rvs0; /* used to interpolate v along the scan line */
  double rd, rds0; /* used to interpolate dropoff along the scan line */
  double re, res0; /* used to interpolate elevation along the scan line */
  double rx; /* used to interpolate x along scan line */
  double z;
  double frac;
  double val;
#ifdef DO_MIP_MAP
  int u, v;
  double fl_u, fl_v;
  double u_frac, v_frac;
  short l00, l10, l01, l11;
  short u00, u10, u01, u11;
  short i00, i10, i01, i11;
  short lx0, lx1, lxx;
  short ux0, ux1, uxx;
  short ix0, ix1, ixx;
  double turb;
  double tur_x, tur_y, tur_z;
#else
  short ixx;
#endif
  Vector pt;
  double elev;
  double tmp1, tmp2;
  double tmpru, tmprv;

  /* find the middle z value with simple bubble sort */
  if (P2.z < P1.z) { tmp=P1; P1=P2; P2=tmp; }
  if (P3.z < P2.z) { tmp=P2; P2=P3; P3=tmp; }
  if (P2.z < P1.z) { tmp=P1; P1=P2; P2=tmp; }

  /* find point B on line P1P2 with z=P2.z */
  tmp1 = (P2.z-P1.z) / (P3.z-P1.z);
  B.x = P1.x + (P3.x-P1.x)*tmp1;
  B.d = P1.d + (P3.d-P1.d)*tmp1;
  B.e = P1.e + (P3.e-P1.e)*tmp1;
  B.u = P1.u + (P3.u-P1.u)*tmp1;
  B.v = P1.v + (P3.v-P1.v)*tmp1;
  B.z = P2.z;
  A = P2; C = P3; D = P1;

  /* A.z == B.z */
  tmp1 = 1.0 / (C.z - A.z);
  if (A.x < B.x) {
    x0 = A.x; xs0 = (C.x-A.x) * tmp1;
    x1 = B.x; xs1 = (C.x-B.x) * tmp1;
    d0 = A.d; ds0 = (C.d-A.d) * tmp1;
    d1 = B.d; ds1 = (C.d-B.d) * tmp1;
    e0 = A.e; es0 = (C.e-A.e) * tmp1;
    e1 = B.e; es1 = (C.e-B.e) * tmp1;
    u0 = A.u; us0 = (C.u-A.u) * tmp1;
    u1 = B.u; us1 = (C.u-B.u) * tmp1;
    v0 = A.v; vs0 = (C.v-A.v) * tmp1;
    v1 = B.v; vs1 = (C.v-B.v) * tmp1;
  } else {
    x0 = B.x; xs0 = (C.x-B.x) * tmp1;
    x1 = A.x; xs1 = (C.x-A.x) * tmp1;
    d0 = B.d; ds0 = (C.d-B.d) * tmp1;
    d1 = A.d; ds1 = (C.d-A.d) * tmp1;
    e0 = B.e; es0 = (C.e-B.e) * tmp1;
    e1 = A.e; es1 = (C.e-A.e) * tmp1;
    u0 = B.u; us0 = (C.u-B.u) * tmp1;
    u1 = A.u; us1 = (C.u-A.u) * tmp1;
    v0 = B.v; vs0 = (C.v-B.v) * tmp1;
    v1 = A.v; vs1 = (C.v-A.v) * tmp1;
  }

  z = ceil(A.z);
  frac = z - A.z;
  x0 += frac*xs0; x1 += frac*xs1;
  d0 += frac*ds0; d1 += frac*ds1;
  e0 += frac*es0; e1 += frac*es1;
  u0 += frac*us0; u1 += frac*us1;
  v0 += frac*vs0; v1 += frac*vs1;
  while ( z <= C.z ) {
    tmp2 = 1.0 / (x1 - x0);
    rx = ceil(x0);
    rus0 = (u1 - u0) * tmp2; ru = u0 + (rx - x0) * rus0;
    rvs0 = (v1 - v0) * tmp2; rv = v0 + (rx - x0) * rvs0;
    rds0 = (d1 - d0) * tmp2; rd = d0 + (rx - x0) * rds0;
    res0 = (e1 - e0) * tmp2; re = e0 + (rx - x0) * res0;
#if 0
printf("rus=%lf rvs=%lf\n", rus0, rvs0);
#endif
    while (rx < x1) {

#ifdef DO_MIP_MAP
      tmprv = rv - floor(rv);

      /* bi-linear lookup into low-res texture map */
      fl_u = ru * (lower[curtain].Ures-1);
      u = (int) fl_u; u_frac = fl_u - u;
      fl_v = tmprv * (lower[curtain].Vres-1);
      v = (int) fl_v; v_frac = fl_v - v;
      l00 = lower[curtain].values[v   + u*lower[curtain].Vres];
      l01 = lower[curtain].values[v+1 + u*lower[curtain].Vres];
      l10 = lower[curtain].values[v   + (u+1)*lower[curtain].Vres];
      l11 = lower[curtain].values[v+1 + (u+1)*lower[curtain].Vres];
      lx0 = l00 + (short)((l10 - l00)*u_frac);
      lx1 = l01 + (short)((l11 - l01)*u_frac);
      lxx = lx0 + (short)((lx1 - lx0)*v_frac);

      /* bi-linear lookup into high_res texture map */
      fl_u = ru * (higher[curtain].Ures-1);
      u = (int) fl_u; u_frac = fl_u - u;
      fl_v = tmprv * (higher[curtain].Vres-1);
      v = (int) fl_v; v_frac = fl_v - v;
      u00 = higher[curtain].values[v   + u*higher[curtain].Vres];
      u01 = higher[curtain].values[v+1 + u*higher[curtain].Vres];
      u10 = higher[curtain].values[v   + (u+1)*higher[curtain].Vres];
      u11 = higher[curtain].values[v+1 + (u+1)*higher[curtain].Vres];
      ux0 = u00 + (short)((u10 - u00)*u_frac);
      ux1 = u01 + (short)((u11 - u01)*u_frac);
      uxx = ux0 + (short)((ux1 - ux0)*v_frac);

      /* now lerp between resolutions */
      ixx = uxx + (short)((lxx - uxx)*mip_frac);

      if (ru >= 0.3 && ru <= 0.7) {
        tmpru = (0.2 - fabs(ru-0.5)) * 5.0;
        ixx += tmpru * MIN_ENERGY;
      }

      #ifdef NOISE_TO_ELEVATION
        pt.x = rx*6.0; pt.y = y*6.0; pt.z = z*6.0;
        elev = 25.0 * noise(pt) - 7.5;
        /* printf("Elev change = %lf km\n", elev); */
        ixx = (*depo)(ixx, re+elev);
      #else
        ixx = (*depo)(ixx, re);
      #endif

      #ifdef FIND_BOUNDS
        if (rx < Bxmin) Bxmin = rx; if (rx > Bxmax) Bxmax = rx;
        if (y  < Bymin) Bymin = y;  if (y  > Bymax) Bymax = y;
        if (z  < Bzmin) Bzmin = z;  if (z  > Bzmax) Bzmax = z;
        vox_count ++;
      #else
        set_vol((short) (ixx*rd), (uLong) rx, (uLong) y, (uLong) z);
      #endif

#else

      ixx=10000;
      set_vol((short) (ixx*rd), (uLong) rx, (uLong) y, (uLong) z);

#endif

      ru += rus0; rv += rvs0;
      rd += rds0; re += res0;
      rx += 1.0;
    }
    x0 += xs0; x1 += xs1;
    d0 += ds0; d1 += ds1;
    e0 += es0; e1 += es1;
    u0 += us0; u1 += us1;
    v0 += vs0; v1 += vs1;
    z+=1.0;
  }

  /* now do bottom of triangle */
  /* A.z == B.z */
  tmp1 = 1.0 / (D.z - A.z);
  if (A.x < B.x) {
    x0 = A.x; xs0 = (D.x-A.x) * tmp1;
    x1 = B.x; xs1 = (D.x-B.x) * tmp1;
    d0 = A.d; ds0 = (D.d-A.d) * tmp1;
    d1 = B.d; ds1 = (D.d-B.d) * tmp1;
    e0 = A.e; es0 = (D.e-A.e) * tmp1;
    e1 = B.e; es1 = (D.e-B.e) * tmp1;
    u0 = A.u; us0 = (D.u-A.u) * tmp1;
    u1 = B.u; us1 = (D.u-B.u) * tmp1;
    v0 = A.v; vs0 = (D.v-A.v) * tmp1;
    v1 = B.v; vs1 = (D.v-B.v) * tmp1;
  } else {
    x0 = B.x; xs0 = (D.x-B.x) * tmp1;
    x1 = A.x; xs1 = (D.x-A.x) * tmp1;
    d0 = B.d; ds0 = (D.d-B.d) * tmp1;
    d1 = A.d; ds1 = (D.d-A.d) * tmp1;
    e0 = B.e; es0 = (D.e-B.e) * tmp1;
    e1 = A.e; es1 = (D.e-A.e) * tmp1;
    u0 = B.u; us0 = (D.u-B.u) * tmp1;
    u1 = A.u; us1 = (D.u-A.u) * tmp1;
    v0 = B.v; vs0 = (D.v-B.v) * tmp1;
    v1 = A.v; vs1 = (D.v-A.v) * tmp1;
  }
  z = floor(A.z);
  frac = A.z - z;
  x0 -= frac*xs0; x1 -= frac*xs1;
  d0 -= frac*ds0; d1 -= frac*ds1;
  e0 -= frac*es0; e1 -= frac*es1;
  u0 -= frac*us0; u1 -= frac*us1;
  v0 -= frac*vs0; v1 -= frac*vs1;
  while( z >= D.z) {
    tmp2 = 1.0 / (x1 - x0);
    rx = ceil(x0);
    rus0 = (u1 - u0) * tmp2; ru = u0 + (rx - x0) * rus0;
    rvs0 = (v1 - v0) * tmp2; rv = v0 + (rx - x0) * rvs0;
    rds0 = (d1 - d0) * tmp2; rd = d0 + (rx - x0) * rds0;
    res0 = (e1 - e0) * tmp2; re = e0 + (rx - x0) * res0;
#if 0
printf("rus=%lf rvs=%lf\n", rus0, rvs0);
#endif
    while (rx < x1) {

#ifdef DO_MIP_MAP
      tmprv = rv - floor(rv);

      /* bi-linear lookup into low-res texture map */
      fl_u = ru * (lower[curtain].Ures-1);
      u = (int) fl_u; u_frac = fl_u - u;
      fl_v = tmprv * (lower[curtain].Vres-1);
      v = (int) fl_v; v_frac = fl_v - v;
      l00 = lower[curtain].values[v   + u*lower[curtain].Vres];
      l01 = lower[curtain].values[v+1 + u*lower[curtain].Vres];
      l10 = lower[curtain].values[v   + (u+1)*lower[curtain].Vres];
      l11 = lower[curtain].values[v+1 + (u+1)*lower[curtain].Vres];
      lx0 = l00 + (short)((l10 - l00)*u_frac);
      lx1 = l01 + (short)((l11 - l01)*u_frac);
      lxx = lx0 + (short)((lx1 - lx0)*v_frac);

      /* bi-linear lookup into high_res texture map */
      fl_u = ru * (higher[curtain].Ures-1);
      u = (int) fl_u; u_frac = fl_u - u;
      fl_v = tmprv * (higher[curtain].Vres-1);
      v = (int) fl_v; v_frac = fl_v - v;
      u00 = higher[curtain].values[v   + u*higher[curtain].Vres];
      u01 = higher[curtain].values[v+1 + u*higher[curtain].Vres];
      u10 = higher[curtain].values[v   + (u+1)*higher[curtain].Vres];
      u11 = higher[curtain].values[v+1 + (u+1)*higher[curtain].Vres];
      ux0 = u00 + (short)((u10 - u00)*u_frac);
      ux1 = u01 + (short)((u11 - u01)*u_frac);
      uxx = ux0 + (short)((ux1 - ux0)*v_frac);

      /* now lerp between resolutions */
      ixx = uxx + (short)((lxx - uxx)*mip_frac);

      if (ru >= 0.3 && ru <= 0.7) {
        tmpru = (0.2 - fabs(ru-0.5)) * 5.0;
        ixx += tmpru * MIN_ENERGY;
      }

      #ifdef NOISE_TO_ELEVATION
        pt.x = rx*6.0; pt.y = y*6.0; pt.z = z*6.0;
        elev = 25.0 * noise(pt) - 7.5;
        /* printf("Elev change = %lf km\n", elev); */
        ixx = (*depo)(ixx, re+elev);
      #else
        ixx = (*depo)(ixx, re);
      #endif

      #ifdef FIND_BOUNDS
        if (rx < Bxmin) Bxmin = rx; if (rx > Bxmax) Bxmax = rx;
        if (y  < Bymin) Bymin = y;  if (y  > Bymax) Bymax = y;
        if (z  < Bzmin) Bzmin = z;  if (z  > Bzmax) Bzmax = z;
        vox_count ++;
      #else
        set_vol((short) (ixx*rd), (uLong) rx, (uLong) y, (uLong) z);
      #endif

#else

      ixx=10000;
      set_vol((short) (ixx*rd), (uLong) rx, (uLong) y, (uLong) z);

#endif

      ru += rus0; rv += rvs0;
      rd += rds0; re += res0;
      rx += 1.0;
    }
    x0 -= xs0; x1 -= xs1;
    d0 -= ds0; d1 -= ds1;
    e0 -= es0; e1 -= es1;
    u0 -= us0; u1 -= us1;
    v0 -= vs0; v1 -= vs1;
    z -= 1.0;
  }

} /* end scan_convert() */

/*---------------------------------------------------------------------------*/

void calc_vol_params(
)
{
  uLong tsub;
  uLong mask;

  /* calculate bit shift and masks for voxel lookups */
  tsub = sub_x; shift_x = 0; mask = 1;
  while (tsub > 1) { tsub >>= 1; shift_x ++; mask *= 2; }
  mask_x = mask - 1; sub_x = mask;  /* set to highest power of 2 <= sub_x */
/*printf("sub_x = %d shift_x = %d mask_x = %d\n", sub_x, shift_x, mask_x);*/

  tsub = sub_y; shift_y = 0; mask = 1;
  while (tsub > 1) { tsub >>= 1; shift_y ++; mask *= 2; }
  mask_y = mask - 1; sub_y = mask;
/*printf("sub_y = %d shift_y = %d mask_y = %d\n", sub_y, shift_y, mask_y);*/

  tsub = sub_z; shift_z = 0; mask = 1;
  while (tsub > 1) { tsub >>= 1; shift_z ++; mask *= 2; }
  mask_z = mask - 1; sub_z = mask;
/*printf("sub_z = %d shift_z = %d mask_z = %d\n", sub_z, shift_z, mask_z);*/

  /* set the overall volume dimensions */
  vol_xres = super_x * sub_x;
  vol_yres = super_y * sub_y;
  vol_zres = super_z * sub_z;
  vol_xyres = vol_xres*vol_yres;

  /* set the number of samples needed for each volume representation */
  super_num_samples = super_x * super_y * super_z;
  sub_num_samples = sub_x * sub_y * sub_z;
  super_xy = super_x * super_y;
  sub_xy = sub_x * sub_y;

  /* set bounding box of volume */
  low.x = VolumeXmin; up.x = VolumeXmax;
  low.y = VolumeYmin; up.y = VolumeYmax;
  low.z = VolumeZmin; up.z = VolumeZmax;

  /* set range in X, Y, and Z */
  VolXrange = VolumeXmax - VolumeXmin;
  VolYrange = VolumeYmax - VolumeYmin;
  VolZrange = VolumeZmax - VolumeZmin;

  /* set mappings for conversions between world and grid coordinates */
  VolXrangeDivXres = VolXrange / vol_xres;
  VolYrangeDivYres = VolYrange / vol_yres;
  VolZrangeDivZres = VolZrange / vol_zres;
  VolXresDivXrange = vol_xres / VolXrange;
  VolYresDivYrange = vol_yres / VolYrange;
  VolZresDivZrange = vol_zres / VolZrange;

  SlabYrange = VolYrange / NUM_SLABS;

  /* set dimensions of a voxel */
  Vwidth  = VolXrange / (vol_xres - 1);
  Vheight = VolYrange / (vol_yres - 1);
  Vdepth  = VolZrange / (vol_zres - 1);
  Vmin = Vwidth;
  Vmin = (Vmin < Vheight) ? Vmin : Vheight;
  Vmin = (Vmin < Vdepth) ? Vmin : Vdepth;

  /* expand slightly so converting to ints give [0...[xyz]res) */
  VxScale = (vol_xres - 1) / (VolXrange * 1.000001);
  VyScale = (vol_yres - 1) / (VolYrange * 1.000001);
  VzScale = (vol_zres - 1) / (VolZrange * 1.000001);

} /* end calc_vol_params() */

/*---------------------------------------------------------------------------*/

void calc_volume(
int curtain_start,
int curtain_end,
int color_band
)
{
  Point  iP[6];
  Vector iV[4];
  Vector iN[4];
  Vector iC[4];
  Point  Q[4];
  Vector Qdir[4];
  Point  R[4];
  Point  T[4];
  Point  U[4];
  double drop[4];
  double elev[4];
  double seg;
  double seg_step;
  double next_seg;
  Vertex P1, P2, P3;
  double y;
  int    c;
  double min_y, max_y;
  double y_value;
  int    outfile;           /* fp to actual image file */
  int    f0, f1;       /* used to interpolate between frames */
  double t_val;
  double v0, v1;
  double seg1, seg2, seg3;
  double visibility;
  double cur_length, seg_length;
  double tmp0, tmp1;
  double frac;
double vis1, vis2;
Vector tmpv;
double dp;



  /* process all of the curtains */
  for ( c=curtain_start ; c<=curtain_end ; c++ ) {

    f0 = (int) CurtainFrameNum;
    f1 = f0 + 1;
    if (num_points[c][f0]==0 || num_points[c][f1]==0) continue;

    /* calculate visibility and skip if this frame isn't visible */
    frac = CurtainFrameNum - f0;
    visibility = visible[c][f0] + (visible[c][f1]-visible[c][f0])*frac;
    if (visibility < 0.00001) continue;

    /* keep track of the length of the curtain as it is generated */
    cur_length = 0.0;
    seg_step = seg_step_size[c];

    for ( seg=1.0 ; seg<99.0 ; seg+=seg_step, cur_length+=seg_length ) {

      #if 0
        printf("c=%d f=%.2lf seg=%lf\n", c, CurtainFrameNum, seg);
        fflush(stdout);
      #endif
      
      seg1 = seg+seg_step;
      seg2 = seg+seg_step+seg_step;
      seg3 = seg+seg_step+seg_step+seg_step;

      iP[0] = spline_point(c, CurtainFrameNum, seg);
      iP[1] = spline_point(c, CurtainFrameNum, seg1);
      iP[2] = spline_point(c, CurtainFrameNum, seg2);
      iP[3] = spline_point(c, CurtainFrameNum, seg3);

      if (color_band == MAGENTA_CHANNEL) {

        /* check to see if Magenta is active in this region */
        vis1 = dist2(iP[1], MagentaFocusPoint);
        vis2 = dist2(iP[2], MagentaFocusPoint);

        /* it is outside, so we can skip */
        if (vis1 > MagentaFocusDist2 && vis2 > MagentaFocusDist2) continue;

        /* set a weight based on distance from focus point */
        if (vis1 >= MagentaFocusDist2) vis1 = 0.0;
        else if (vis1 <= MagentaFocusDist1) vis1 = 1.0;
        else vis1 = (vis1-MagentaFocusDist1) / (MagentaFocusDist2-MagentaFocusDist1);
        if (vis2 >= MagentaFocusDist2) vis2 = 0.0;
        else if (vis2 <= MagentaFocusDist1) vis2 = 1.0;
        else vis2 = (vis2-MagentaFocusDist1) / (MagentaFocusDist2-MagentaFocusDist1);

      } else if (color_band == GREEN_CHANNEL) {

        /* green is always active */
        vis1 = vis2 = 1.0;

      } else if (color_band == RED_CHANNEL) {

        /* check to see if Red is active in this region */
        vis1 = dist2(iP[1], RedFocusPoint);
        vis2 = dist2(iP[2], RedFocusPoint);

        /* it is outside, so we can skip */
        if (vis1 > RedFocusDist2 && vis2 > RedFocusDist2) continue;

        /* set a weight based on distance from focus point */
        if (vis1 >= RedFocusDist2) vis1 = 0.0;
        else if (vis1 <= RedFocusDist1) vis1 = 1.0;
        else vis1 = (vis1-RedFocusDist1) / (RedFocusDist2-RedFocusDist1);
        if (vis2 >= RedFocusDist2) vis2 = 0.0;
        else if (vis2 <= RedFocusDist1) vis2 = 1.0;
        else vis2 = (vis2-RedFocusDist1) / (RedFocusDist2-RedFocusDist1);
      }

      /* set the lower bound of the curtain */
      iP[0] = multiply(MagentaLower, normalize(iP[0]));
      iP[1] = multiply(MagentaLower, normalize(iP[1]));
      iP[2] = multiply(MagentaLower, normalize(iP[2]));
      iP[3] = multiply(MagentaLower, normalize(iP[3]));

      seg_length = distance(iP[1],iP[2]);

      #if 0
        printf("Dist to this point=%lf seg=%lf\n", cur_length, seg_length);
      #endif

      iV[1] = normalize(subtract(iP[2], iP[0]));
      iV[2] = normalize(subtract(iP[3], iP[1]));
      iN[1] = normalize(iP[1]);
      iN[2] = normalize(iP[2]);

      #ifdef DO_INCLINATION
        if (inclination) {
          #if 0
            printf("iN[1]=<%.5lf,%.5lf,%.5lf> -> ", iN[1].x, iN[1].y, iN[1].z);
          #endif
          iN[1] = adjust_inclination(iN[1]);
          #if 0
            printf("<%.5lf,%.5lf,%.5lf>\n", iN[1].x, iN[1].y, iN[1].z);
            fflush(stdout);
          #endif
          iN[2] = adjust_inclination(iN[2]);
        }
      #endif

      iC[1] = cross(iV[1], iN[1]); 
      iC[2] = cross(iV[2], iN[2]);

      /* calculate the 4 corner points of lower bound of curtain */
      Q[0] = add(iP[1], multiply( CurtainWidth*0.5, iC[1]));
      Q[1] = add(iP[1], multiply(-CurtainWidth*0.5, iC[1]));
      Q[2] = add(iP[2], multiply(-CurtainWidth*0.5, iC[2]));
      Q[3] = add(iP[2], multiply( CurtainWidth*0.5, iC[2]));

      /* default normal is normal to the earth */
      Qdir[0] = normalize(Q[0]);
      Qdir[1] = normalize(Q[1]);
      Qdir[2] = normalize(Q[2]);
      Qdir[3] = normalize(Q[3]);

      #if 0
        printf("Qdir[0] = <%lf,%lf,%lf>\n",Qdir[0].x,Qdir[0].y,Qdir[0].z);
        printf("Qdir[1] = <%lf,%lf,%lf>\n",Qdir[1].x,Qdir[1].y,Qdir[1].z);
        printf("Qdir[2] = <%lf,%lf,%lf>\n",Qdir[2].x,Qdir[2].y,Qdir[2].z);
        printf("Qdir[3] = <%lf,%lf,%lf>\n",Qdir[3].x,Qdir[3].y,Qdir[3].z);
        fflush(stdout);
      #endif

      #ifdef DO_INCLINATION
        if (inclination) {
          Qdir[0] = adjust_inclination(Qdir[0]);
          Qdir[1] = adjust_inclination(Qdir[1]);
          Qdir[2] = adjust_inclination(Qdir[2]);
          Qdir[3] = adjust_inclination(Qdir[3]);
        }
      #endif

      #if 0
        printf("Adj Qdir[0] = <%lf,%lf,%lf>\n",Qdir[0].x,Qdir[0].y,Qdir[0].z);
        printf("Adj Qdir[1] = <%lf,%lf,%lf>\n",Qdir[1].x,Qdir[1].y,Qdir[1].z);
        printf("Adj Qdir[2] = <%lf,%lf,%lf>\n",Qdir[3].x,Qdir[2].y,Qdir[2].z);
        printf("Adj Qdir[3] = <%lf,%lf,%lf>\n",Qdir[3].x,Qdir[3].y,Qdir[3].z);
        fflush(stdout);
      #endif

      /* Q[*] are at bottom of magenta channel, need to move points to */
      /* appropriate elevation along Qdir if doing green and red */
      if (color_band == GREEN_CHANNEL) {
        Q[0] = add(Q[0], multiply(GreenOffset, Qdir[0]));
        Q[1] = add(Q[1], multiply(GreenOffset, Qdir[1]));
        Q[2] = add(Q[2], multiply(GreenOffset, Qdir[2]));
        Q[3] = add(Q[3], multiply(GreenOffset, Qdir[3]));
      } else if (color_band == RED_CHANNEL) {
        Q[0] = add(Q[0], multiply(RedOffset, Qdir[0]));
        Q[1] = add(Q[1], multiply(RedOffset, Qdir[1]));
        Q[2] = add(Q[2], multiply(RedOffset, Qdir[2]));
        Q[3] = add(Q[3], multiply(RedOffset, Qdir[3]));
      }

      /* find the minimum Y value of the 4 lower corners */
      min_y = (Q[0].y < Q[1].y) ? Q[0].y : Q[1].y;
      min_y = (min_y < Q[2].y) ? min_y : Q[2].y;
      min_y = (min_y < Q[3].y) ? min_y : Q[3].y;

      /* find which y value in the grid that this corresponds to */
      min_y = (min_y - VolumeYmin) / (VolumeYmax - VolumeYmin) * vol_yres;
      min_y = floor(min_y);
      if (min_y < 0) { printf("min_y out of range\n"); exit(1); }

      /* set the upper bound of the curtain based on color channel */
      if (color_band == MAGENTA_CHANNEL) {
        U[0] = add(Q[0], multiply(MagentaHeight, Qdir[0]));
        U[1] = add(Q[1], multiply(MagentaHeight, Qdir[1]));
        U[2] = add(Q[2], multiply(MagentaHeight, Qdir[2]));
        U[3] = add(Q[3], multiply(MagentaHeight, Qdir[3]));
      } else if (color_band == GREEN_CHANNEL) {
        U[0] = add(Q[0], multiply(GreenHeight, Qdir[0]));
        U[1] = add(Q[1], multiply(GreenHeight, Qdir[1]));
        U[2] = add(Q[2], multiply(GreenHeight, Qdir[2]));
        U[3] = add(Q[3], multiply(GreenHeight, Qdir[3]));
      } else if (color_band == RED_CHANNEL) {
        U[0] = add(Q[0], multiply(RedHeight, Qdir[0]));
        U[1] = add(Q[1], multiply(RedHeight, Qdir[1]));
        U[2] = add(Q[2], multiply(RedHeight, Qdir[2]));
        U[3] = add(Q[3], multiply(RedHeight, Qdir[3]));
      }

      #ifdef CLIP_CURTAINS

        /* skip this segment IF Q[*] and U[*] are outside view */

        /* try one point as a quick check */
        dp = dot_product(apex.dir, create_vector( /*fr*/ apex.org, /*to*/ Q[0]));
        if (dp < COS_97_3) {

           /* do the other 7 */
           if (dot_product(apex.dir, create_vector(apex.org, Q[1])) < COS_97_3 &&
               dot_product(apex.dir, create_vector(apex.org, Q[2])) < COS_97_3 &&
               dot_product(apex.dir, create_vector(apex.org, Q[3])) < COS_97_3 &&
               dot_product(apex.dir, create_vector(apex.org, U[0])) < COS_97_3 &&
               dot_product(apex.dir, create_vector(apex.org, U[1])) < COS_97_3 &&
               dot_product(apex.dir, create_vector(apex.org, U[2])) < COS_97_3 &&
               dot_product(apex.dir, create_vector(apex.org, U[0])))

             continue;
          /*   printf("found a segment to clip\n"); */

        }

      #endif

      /* find the maximum Y value of the 4 upper corners */
      max_y = (U[0].y > U[1].y) ? U[0].y : U[1].y;
      max_y = (max_y > U[2].y) ? max_y : U[2].y;
      max_y = (max_y > U[3].y) ? max_y : U[3].y;

      /* find the y value in the grid that this corresponds to */
      max_y = (max_y - VolumeYmin) / (VolumeYmax - VolumeYmin) * vol_yres;
      max_y = floor(max_y);

      #if 0 
        printf("c=%d f=%.2lf seg=%lf\n", c, CurtainFrameNum, seg);
        printf("Q0=<%.1lf,%.1lf,%.1lf> U0=<%.1lf,%.1lf,%.1lf>\n",
                   Q[0].x,Q[0].y,Q[0].z,U[0].x,U[0].y,U[0].z);
        printf("Q1=<%.1lf,%.1lf,%.1f> U1=<%.1lf,%.1lf,%.1lf>\n",
                   Q[1].x,Q[1].y,Q[1].z,U[1].x,U[1].y,U[1].z);
        printf("Q2=<%.1lf,%.1lf,%.1lf> U2=<%.1lf,%.1lf,%.1lf>\n",
                   Q[2].x,Q[2].y,Q[2].z,U[2].x,U[2].y,U[2].z);
        printf("Q3=<%.1lf,%.1lf,%.1lf> U3=<%.1lf,%.1lf,%.1lf>\n",
                   Q[3].x,Q[3].y,Q[3].z,U[3].x,U[3].y,U[3].z);
      #endif

      /* restrict this to the [RENDER_YMIN, RENDER_YMAX] range */
      if (min_y < RENDER_YMIN) min_y = RENDER_YMIN;
      if (max_y > RENDER_YMAX) max_y = RENDER_YMAX;
  
      for ( y=min_y ; y<=max_y ; y+=1.0 ) {
        y_value = VolumeYmin + (y/vol_yres)*(VolumeYmax - VolumeYmin);

        #if 0
          printf("y_val = %lf\n", y_value);
        #endif

        /* calculate value of t on Q+tQdir that has y=y_value */
        t_val = (y_value - Q[0].y) / Qdir[0].y;
        R[0] = add(Q[0], multiply(t_val, Qdir[0]));
        t_val = (y_value - Q[1].y) / Qdir[1].y;
        R[1] = add(Q[1], multiply(t_val, Qdir[1]));
        t_val = (y_value - Q[2].y) / Qdir[2].y;
        R[2] = add(Q[2], multiply(t_val, Qdir[2]));
        t_val = (y_value - Q[3].y) / Qdir[3].y;
        R[3] = add(Q[3], multiply(t_val, Qdir[3]));
    
        /* calculate the elevation at each vertex */
        elev[0] = mag(R[0]);
        elev[1] = mag(R[1]);
        elev[2] = mag(R[2]);
        elev[3] = mag(R[3]);

        #if 0
          printf("Elevation y=%lf 0=%lf 1=%lf 2=%lf 3=%lf\n", y_value,
                  elev[0], elev[1], elev[2], elev[3]);
        #endif

        #if 0 
          printf("R0=<%.1lf,%.1lf,%.1lf> dist=%.1lf dropoff=%.4lf \n",
                     R[0].x,R[0].y,R[0].z,mag(R[0]),drop[0]);
          printf("R1=<%.1lf,%.1lf,%.1lf> dist=%.1lf dropoff=%.4lf \n",
                     R[1].x,R[1].y,R[1].z,mag(R[1]),drop[1]);
          printf("R2=<%.1lf,%.1lf,%.1lf> dist=%.1lf dropoff=%.4lf \n",
                     R[2].x,R[2].y,R[2].z,mag(R[2]),drop[2]);
          printf("R3=<%.1lf,%.1lf,%.1lf> dist=%.1lf dropoff=%.4lf \n",
                     R[3].x,R[3].y,R[3].z,mag(R[3]),drop[3]);
          fflush(stdout);
        #endif

        T[0] = world2voxel(R[0]);
        T[1] = world2voxel(R[1]);
        T[2] = world2voxel(R[2]);
        T[3] = world2voxel(R[3]);

        #if 0
          printf("Triangle <%.1lf,%.1lf,%.1lf> ",T[0].x,T[0].y,T[0].z);
          printf("<%.1lf,%.1lf,%.1lf> <%.1lf,%.1lf,%.1lf>\n",
                 T[1].x,T[1].y,T[1].z,T[2].x,T[2].y,T[2].z);
          printf("Triangle <%.1lf,%.1lf,%.1lf> ",T[0].x,T[0].y,T[0].z);
          printf("<%.1lf,%.1lf,%.1lf> <%.1lf,%.1lf,%.1lf>\n",
                 T[3].x,T[3].y,T[3].z,T[2].x,T[2].y,T[2].z);
        #endif

        /* calculate range of V values for the corners of the triangles */
        v0 = interp_uv(c, seg1) + UVflow[c];
        v1 = interp_uv(c, seg2) + UVflow[c];

        /* find all of the volume sample points contained inside quad */
        /* y is elevation and is these are 2D in XZ plane of volume */

        /* first do triangle defined by R0 R1 R2 */
        P1.x = T[0].x;    P2.x = T[1].x;    P3.x = T[2].x;
        P1.y = T[0].y;    P2.y = T[1].y;    P3.y = T[2].y;
        P1.z = T[0].z;    P2.z = T[1].z;    P3.z = T[2].z;
        P1.e = elev[0];   P2.e = elev[1];   P3.e = elev[2];
        P1.u = 0.0;       P2.u = 1.0;       P3.u = 1.0;
        P1.v = v0;        P2.v = v0;        P3.v = v1;

        P1.d = P2.d = interp_brightness(c, seg1)*visibility*vis1;
        P3.d = interp_brightness(c, seg2)*visibility*vis2;

        scan_convert(P1, P2, P3, y, c, color_band);

        /* now do triangle defined by R0 R3 R2 */
        P1.x = T[0].x;    P2.x = T[3].x;    P3.x = T[2].x;
        P1.y = T[0].y;    P2.y = T[3].y;    P3.y = T[2].y;
        P1.z = T[0].z;    P2.z = T[3].z;    P3.z = T[2].z;
        P1.e = elev[0];   P2.e = elev[3];   P3.e = elev[2];
        P1.u = 0.0;       P2.u = 0.0;       P3.u = 1.0;
        P1.v = v0;        P2.v = v1;        P3.v = v1;

        P1.d = interp_brightness(c, seg1)*visibility*vis1;
        P2.d = P3.d = interp_brightness(c, seg2)*visibility*vis2;

        scan_convert(P1, P2, P3, y, c, color_band);

      } /* end for y */

    } /* end for seg */

  } /* end for c */

  #ifdef FIND_BOUNDS
    return;
  #endif

  #ifndef PRODUCTION_RUN
    printf("  Inserts: %d, Subs allocated: %d\n",num_inserts,num_sub_allocated);
    printf("  Subgrid overwrites: %d (%f%%)\n", num_overwrites,
           (double)num_overwrites/num_inserts*100.0);
  #endif

  {
    double subblocksize = sizeof(short)* sub_num_samples;
    double octree_size = (double)(((num_children+1)*sizeof(OCTREE))/1048576.0);
    double subblocks_size = (double)(num_sub_allocated*subblocksize/1048576.0);
/*  printf("  Size octree node: %d bytes\n", sizeof(OCTREE));*/
/*  printf("  Number of children: %d / %d = (%.02f%% Full)\n", num_children,
           super_num_samples,((double)num_children/super_num_samples)*100.0);*/
/*  printf("  Size of subblock: %d bytes\n",(int)subblocksize);*/
/*  printf("  Subblocks allocated: %d\n",num_sub_allocated);*/
    if (octree_size > 1) {
      printf("  Size of octree: %.02f MB Size of subblocks: %.02f MB\n",
           octree_size, subblocks_size);
      printf("  TOTAL MEMORY USED: %.02f MB\n", octree_size+subblocks_size);
    } else {
      printf("  Size of octree: %.02f KB Size of subblocks: %.02f MB\n",
           octree_size*1024.0, subblocks_size);
      printf("  TOTAL MEMORY USED: %.02f MB\n", octree_size+subblocks_size);
    }
  }

} /* end calc_volume() */

/*---------------------------------------------------------------------------*/

void render_image(
char *raw_file,
int   frame, 
int   color_band
)
{
  double total_vol;
  double totaltime;
  uLong  start;                  /* absolute start time of creation/rendering */
  uLong  finish;                 /* absolute end time of creation/rendering */
  double timings;                /* total rendering time */
  int    i,j,c,cur;              /* loop counters */
  float  *raw;
  int    raw_header;        /* size of the header in image file */
#ifdef DO_FISHEYE
  const double center = (s.xres-1) / 2.0;
  const double radius2 = (s.xres-1)*(s.xres-1) / 4.0;
  double theta, costheta, sintheta;
  double phi, cosphi, sinphi;
  double tx, ty;
  double cx, cy;
  double dist, max_dist;
#else
  const double wxrange = s.xmax - s.xmin;  /* window (in w.c.) in X */
  const double wyrange = s.ymax - s.ymin;  /* window (in w.c.) in Y */
  const double xpixels = s.xres - 1;       /* number of pixels in X */
  const double ypixels = s.yres - 1;       /* number of pixels in Y */
#endif
  double Tstep;
  double Tstart;
  double tmp;
  double tmpx, tmpy, tmpz;
  int    outfile;           /* fp to actual image file */
  int    header;            /* size of the header in image file */
  int    dump_it=0;         /* when 1, dumps debug info */
  char   buf[80];
  int    rindex;
  double ray_ratio;    /* how current ray differs from virtual ray */
  double length;       /* used to calculate ray_ratio */
  double tnear;        /* entrance point on global bounding volume */
  double tfar;         /* exit point on global bounding volume */
  int    t_beg, t_end; 
  short *ptr;
  double energy_000;     /* values extracted from volume */
  double energy_100;     /*             "                */
  double energy_010;     /*             "                */
  double energy_110;     /*             "                */
  double energy_001;     /*             "                */
  double energy_101;     /*             "                */
  double energy_011;     /*             "                */
  double energy_111;     /*             "                */
  double energy_x00;     /* linear interpolation in X */
  double energy_x01;     /* linear interpolation in X */
  double energy_x10;     /* linear interpolation in X */
  double energy_x11;     /* linear interpolation in X */
  double energy_xy0;     /* linear interpolation in Y */
  double energy_xy1;     /* linear interpolation in Y */
  double energy_xyz;     /* linear interpolation in Z */
  Ray    R;
  double Vx,Vy,Vz;     /* P converted to voxel coordinates */
  uLong  x0,y0,z0;      /* voxel coordinate of corner 000 */
  double fx,fy,fz;     /* percentages for interpolation */
  double next_t;
  int s0_x, s1_x;
  int s0_y, s1_y;
  int s0_z, s1_z;
#ifdef DO_EARTH
  double A, B, C, discrim;
  double t0, t1;
  u_char *hit_earth; 
#endif
  const double COS_TILT = cos(-EARTH_TILT_RAD);
  const double SIN_TILT = sin(-EARTH_TILT_RAD);
#ifdef DO_MIP_MAP
  double du, dv;
  FILE *fp;
  int xres, yres, maxval;
  int curtain_num;
  int f0, f1;
  double frac;
#endif
  int xyres = s.xres*s.yres;
  stackTop theStack;
  OCTREE *octptr;
  Point min, max;
  Point childL[8];   /* holds volume minimum */
  Point childU[8];   /* holds volume maximum */
  int did_swap;
  double new_x_max;
  double new_y_max;
  double new_z_max;
  double powcheck;
  int slab;
  double slab_tnear;        /* entrance point on global bounding volume */
  double slab_tfar;         /* exit point on global bounding volume */


#ifdef NOISE_TO_ELEVATION
  initnoise();
#endif
  total_vol = 0.0;
  start = get_time(0);


  /* set the height of each curtain */
  GreenHeight   = GreenUpper   - GreenLower;
  MagentaHeight = MagentaUpper - MagentaLower;
  RedHeight     = RedUpper     - RedLower;

  /* set distance from bottom of Magenta curtain to Green/Red curtains */
  GreenOffset = GreenLower - MagentaLower;
  RedOffset   = RedLower   - MagentaLower;

  #ifdef DO_FISHEYE
    if (s.xres != s.yres) ERROR_MSG("Fish eye view requires square raster")
  #endif

  /* set up constants used to render volume(s) */
  calc_vol_params();

  #ifdef FIND_BOUNDS
     printf("\n");
     printf("  Determining Bounds of the volume\n");
     Bxmin = Bymin = Bzmin = INF;
     Bxmax = Bymax = Bzmax = -INF;
  #else
    /* output the raw alpha values as floats */
    raw = (float *) malloc(sizeof(float)*xyres);
    if (!raw) ERROR_BAD_MALLOC("raw buffer in render_image")
    sprintf(buf, "#FRAW\n%d %d\n", s.xres, s.yres);
    raw_header = strlen(buf);

    /* set the entire raster to 1.0 (which represents energy=0.0) */
    for ( rindex=0 ; rindex<xyres ; rindex++ )
        raw[rindex] = 1.0;

    #ifdef DO_EARTH
      hit_earth = (u_char *) malloc(sizeof(u_char)*xyres);
      if (!hit_earth) ERROR_BAD_MALLOC("hit_earth in render_image")
      for ( rindex=0 ; rindex<xyres ; rindex++ ) hit_earth[rindex]=0;
    #endif

    if (super_x != super_z) {
      printf("super_x != super_z... please fix.\n");
      exit(0);
    }
  #endif

  /* set the deposition function used for this color channel */
  depo = deposition[color_band];

  #ifdef CLIP_CURTAINS
    /* calculate the viewing direction towards apex */
    apex.org.x = apex.org.y = apex.org.z = 0.0;
    apex.dir.x = apex.dir.y = 0.0;
    #ifdef DO_NEG_FISHEYE
      apex.dir.z = 1.0;
    #else
      apex.dir.z = -1.0;
    #endif

    /* apply the camera and earth tilt transforms */
    apex.org = camera_xform(apex.org);
    apex.dir = vec_camera_xform(apex.dir);
    tmpy = apex.dir.y; tmpz = apex.dir.z;
    apex.dir.y = tmpy*COS_TILT - tmpz*SIN_TILT;
    apex.dir.z = tmpy*SIN_TILT + tmpz*COS_TILT;
    tmpy = apex.org.y; tmpz = apex.org.z;
    apex.org.y = tmpy*COS_TILT - tmpz*SIN_TILT;
    apex.org.z = tmpy*SIN_TILT + tmpz*COS_TILT;

    printf("Apex = <%.2lf, %.2lf, %.2lf> + <%.2lf, %.2lf, %.2lf>t\n", apex.org.x,
           apex.org.y, apex.org.z, apex.dir.x, apex.dir.y, apex.dir.z);
  #endif

  uLong slabTimeStart = 0;
  uLong slabTimeEnd = 0;
  double slabTime = 0;
  uLong slabRenderStart = 0;
  uLong slabRenderEnd = 0;
  double slabRenderTime = 0;
  double totalVolTime = 0;
  double totalRenderTime = 0;
  
  /* for each curtain, build a volume and then render to raster */
  for ( cur=0 ; cur<num_curtains ; cur++ ) {

    printf("\n"); printf(sharps);
    printf("FRAME: %d | Start of Curtain %d: ",frame, cur);
    f0 = (int) CurtainFrameNum; f1 = f0+1;
    frac = CurtainFrameNum - f0;
    if (visible[cur][f0] < 0.00001 && visible[cur][f1] < 0.00001) {
      printf("  it's not visible!\n");
      printf(sharps);
      continue;
    }

    #ifdef DO_MIP_MAP

      /* lerp to get the texture frame number */
      itexture = texture[cur][f0] + (texture[cur][f1]-texture[cur][f0])*frac;
      printf("texture frame %lf\n", itexture);
      printf(sharps);

      /* grab the texture maps for this curtain frame number */
      get_turbulence_map(cur, itexture);

      /* set mappings for texture lookup */
      target_area = VolXrange / (sub_x*super_x);
      lower[cur].area  = CurtainWidth / lower[cur].Ures;
      higher[cur].area = CurtainWidth / higher[cur].Ures;
      if (lower[cur].Ures == higher[cur].Ures)
        mip_frac = 0.0;
      else
        mip_frac = (target_area-higher[cur].area)
                 / (lower[cur].area-higher[cur].area);
#if 0
      printf("  low[%d]=%lf high[%d]=%lf target=%lf frac=%lf\n", cur,
             lower[cur].area, cur, higher[cur].area, target_area, mip_frac);
#endif
    #else

      printf("  Processing Curtain %d\n", cur);

    #endif

    #ifdef FIND_BOUNDS
      RENDER_YMIN = 0;
      RENDER_YMAX = vol_yres-1;
      calc_volume(cur,cur,color_band);
      continue;
    #endif

    for ( slab=0 ; slab<NUM_SLABS ; slab++ ) {

      /* only generate a subset of Y grid values for each slab */
      RENDER_YMIN = (vol_yres * slab) / NUM_SLABS;
      if (RENDER_YMIN > 0) RENDER_YMIN--;
      RENDER_YMAX = (vol_yres * (slab+1)) / NUM_SLABS;
      if (RENDER_YMAX >= vol_yres) vol_yres-1;
      printf("\n");
      printf(dashes);
      printf("Frame %d | Curtain %d: ",frame, cur);
      if (color_band == GREEN_CHANNEL) printf("Green ");
      else if (color_band == RED_CHANNEL) printf("Red ");
      else printf("Magenta ");
      printf("slab %d of %d (%d to %d) Max: %d  ", slab+1, 
        NUM_SLABS, RENDER_YMIN, RENDER_YMAX, (int)vol_yres);
      totalEndTime = get_time(totalStartTime);
      totalTime = totalEndTime/get_freq();
      print_timing(" frame time:", totalTime, "\n");
      printf(dashes);
      channelTime = totalVolTime + totalRenderTime;
      print_timing("  Channel Time:", channelTime, "\n");

      octree_init(&octree,super_x);
      octree_init(&blanknode,0);
      printf("  Using octree depth of: %d\n",oct_depth);

      powcheck = pow(2,oct_depth);
      if (powcheck == super_x) {
        new_x_max = VolumeXmax;
        new_y_max = VolumeYmin+((VolumeYmax-VolumeYmin)*((double)super_x/super_y));
        new_z_max = VolumeZmax;
      } else {
        new_x_max = VolumeXmin+(VolXrange*(powcheck/(double)super_x));
        new_y_max = VolumeYmin+(VolYrange*(powcheck/(double)super_y));
        new_z_max = VolumeZmin+(VolZrange*(powcheck/(double)super_z));
      }
      printf("  Octree Bound: X:(%.1lf,%.1f) Y:(%.1f,%.1f) Z:(%.1f,%.1f)\n",
             VolumeXmin, new_x_max, VolumeYmin, new_y_max, VolumeZmin, new_z_max);

      slabTimeStart = get_time(0);
      calc_volume(cur,cur,color_band);
      slabTimeEnd = get_time(slabTimeStart);
      slabTime = slabTimeEnd / get_freq();
      totalVolTime += slabTime;

      print_timing("\n  Created in", slabTime, " ... ");
      fflush(stdout);

      /***** if no inserts, skip render !!!! *****/
      if (num_inserts == 0) {
        printf("  **** NOTHING INSERTED.  NOT RENDERING **** \n");
        octree_destroy(&octree);
        octree_destroy(&blanknode);
        oct_depth = 0;
        num_children = 0;
        num_inserts = 0;
        num_sub_allocated = 0;
        num_overwrites = 0;
        continue;
      }

      Tstep = Vmin;
      Tstart = s.near + (Tstep / 2.0);

      slabRenderStart = get_time(0);

      rindex = 0;
      for ( j=0 ; j<s.yres ; j++ ) {

        for ( i=0 ; i<s.xres ; i++,rindex++ ) {

          #ifdef DO_FISHEYE

            /* skip if outside circle */
            if (((i-center)*(i-center)+(j-center)*(j-center)) > radius2)
              continue;

            /* viewpint is at (0,0,0) */
            R.org.x = R.org.y = R.org.z = 0.0;
      
            /* determine Rdir - apex of dome is down -Z axis */
            if ((i*2)==(s.xres-1) && (j*2)==(s.yres-1)) {
      
              /* special case of exactly the apex */
              theta = 0.0; costheta = 1.0; sintheta = 0.0;
              phi = 0.0;   cosphi = 1.0;   sinphi = 0.0;

            } else {
      
              /* calculate vector from viewpoint to i,j on viewplane */
              cx = (s.xres-1) / 2.0; tx = i - cx;
              cy = (s.yres-1) / 2.0; ty = j - cy;
              dist = sqrt(tx*tx + ty*ty);
              max_dist = s.xres-1 - cx; 
        
              /* calculate angle down from the apex based on pixel[i,j] */
              #ifdef DO_90_DEGREES
                /* constant is 90 * PI / 180 */
                theta = (dist/max_dist) * M_PI_2;
              #else
                /* Hayden dome is mapped 97.3 degrees down from apex */
                /* constant is 97.3 * PI / 180 */
                theta = (dist/max_dist) * 1.69820536219;
              #endif
      
              /* calculate polar coordinates */
              costheta = cos(theta);
              sintheta = sin(theta);
              cosphi = tx / dist;
              sinphi = ty / dist;
  
            }
    
            /* create direction vector */
            R.dir.x = cosphi * sintheta;
            R.dir.y = sinphi * sintheta;
            #ifdef DO_NEG_FISHEYE
              R.dir.z = costheta;  /* down +Z axis */
            #else
              R.dir.z = -costheta; /* down -Z axis */
            #endif

            /* really shouldn't have to normalize ... */
            length = sqrt(R.dir.x*R.dir.x + R.dir.y*R.dir.y + R.dir.z*R.dir.z);
            ray_ratio = 1.0;

          #else

            /* vp at (0,0,0) looking down -Z with view plane at z=-s->near1 */
            R.org.x = R.org.y = R.org.z = 0.0;
            R.dir.x = s.xmin + (i/xpixels) * wxrange /* - R.org.x */;
            R.dir.y = s.ymin + (j/ypixels) * wyrange /* - R.org.y */;
            R.dir.z = - s.near /* - R.org.z */;
        
            /* calculate scale factor for this ray vs. the virtual ray */
            length = sqrt(R.dir.x*R.dir.x + R.dir.y*R.dir.y + R.dir.z*R.dir.z);
            ray_ratio = length / - R.dir.z;

          #endif

          /* normalize the ray direction */ 
          R.dir.x /= length; R.dir.y /= length; R.dir.z /= length;

          R.org = camera_xform(R.org);
          R.dir = vec_camera_xform(R.dir);

          /* apply the opposite of the earth's tilt to the ray */
          tmpy = R.dir.y;
          tmpz = R.dir.z;
          R.dir.y = tmpy*COS_TILT - tmpz*SIN_TILT;
          R.dir.z = tmpy*SIN_TILT + tmpz*COS_TILT;
          tmpy = R.org.y;
          tmpz = R.org.z;
          R.org.y = tmpy*COS_TILT - tmpz*SIN_TILT;
          R.org.z = tmpy*SIN_TILT + tmpz*COS_TILT;

#if 0
printf("Pixel[%d][%d] = <%.2lf,%.2lf,%.2lf>+<%.2lf,%.2lf,%.2lf>t\n",
        i, j, R.org.x, R.org.y, R.org.z, R.dir.x, R.dir.y, R.dir.z);
#endif

          #ifdef DO_EARTH
            /* A = R.dir.x*R.dir.x + R.dir.y*R.dir.y + R.dir.z*R.dir.z; */
            B = 2.0*(R.dir.x*R.org.x + R.dir.y*R.org.y + R.dir.z*R.org.z);
            C = R.org.x*R.org.x + R.org.y*R.org.y + R.org.z*R.org.z - earth_rad2;
            discrim = B*B - 4.0*C;
            if (discrim >= 0.0) {
              /* possible intersection with the earth */
              discrim = sqrt(discrim);
              t0 = (-B - discrim) * 0.5;
              t1 = (-B + discrim) * 0.5;
              if (t0 > FUDGE || t1 > FUDGE) hit_earth[rindex]=1;
            }
          #endif

          slab_up = up;
          slab_low = low;
          slab_up.y = VolumeYmin + SlabYrange*slab;
          slab_low.y = VolumeYmin + SlabYrange*(slab+1);
          /* can return if ray misses volume bound */
          if (!VolumeBoxIntersect(R.org.x, R.org.y, R.org.z,
                                  R.dir.x, R.dir.y, R.dir.z,
                                  slab_low, slab_up,
                                  &slab_tnear, &slab_tfar)) {
            continue;
          }

          /**********************************
           * OCTREE/STACK IMPLEMENTATION    *
           *  If the ray hits the volume,   * 
           *  then we push the whole volume *
           *  onto the stack                * 
           * Cube Shape:                    *
           *   ---------  ---------         *
           *   | 2 | 3 |  | 6 | 7 |         *
           *   ---------  ---------         *
           *   | 0 | 1 |  | 4 | 5 |         *
           *   ---------  ---------         *
           *     BACK       FRONT           *
           **********************************/

          stackinit(&theStack);
          octptr = &octree;
          did_swap = 0;

          min.x = VolumeXmin; min.y = VolumeYmin; min.z = VolumeZmin;
          max.x = new_x_max; max.y = new_y_max; max.z = new_z_max;
          stackpush(&theStack, octptr, min, max);
          stackData top_stack;
          while (stackpop(&theStack, &top_stack)) {
            double mid_x=((top_stack.max.x-top_stack.min.x)*0.5)+top_stack.min.x;
            double mid_y=((top_stack.max.y-top_stack.min.y)*0.5)+top_stack.min.y;
            double mid_z=((top_stack.max.z-top_stack.min.z)*0.5)+top_stack.min.z;
            childL[0].x = top_stack.min.x;   childU[0].x = mid_x;
            childL[0].y = top_stack.min.y;   childU[0].y = mid_y; 
            childL[0].z = top_stack.min.z;   childU[0].z = mid_z;
            childL[1].x = mid_x;             childU[1].x = top_stack.max.x;
            childL[1].y = top_stack.min.y;   childU[1].y = mid_y; 
            childL[1].z = top_stack.min.z;   childU[1].z = mid_z;
            childL[2].x = top_stack.min.x;   childU[2].x = mid_x;
            childL[2].y = mid_y;             childU[2].y = top_stack.max.y;
            childL[2].z = top_stack.min.z;   childU[2].z = mid_z;
            childL[3].x = mid_x;             childU[3].x = top_stack.max.x;
            childL[3].y = mid_y;             childU[3].y = top_stack.max.y;
            childL[3].z = top_stack.min.z;   childU[3].z = mid_z;
            childL[4].x = top_stack.min.x;   childU[4].x = mid_x;
            childL[4].y = top_stack.min.y;   childU[4].y = mid_y;
            childL[4].z = mid_z;             childU[4].z = top_stack.max.z;
            childL[5].x = mid_x;             childU[5].x = top_stack.max.x;
            childL[5].y = top_stack.min.y;   childU[5].y = mid_y;
            childL[5].z = mid_z;             childU[5].z = top_stack.max.z;
            childL[6].x = top_stack.min.x;   childU[6].x = mid_x;
            childL[6].y = mid_y;             childU[6].y = top_stack.max.y;
            childL[6].z = mid_z;             childU[6].z = top_stack.max.z;
            childL[7].x = mid_x;             childU[7].x = top_stack.max.x;
            childL[7].y = mid_y;             childU[7].y = top_stack.max.y;
            childL[7].z = mid_z;             childU[7].z = top_stack.max.z;

            /* For each child of the current volume */
            for (c=0; c<8; c++) {

              /* Does the volume intersect with the ray? */
              if (VolumeBoxIntersect(R.org.x, R.org.y, R.org.z,
                                     R.dir.x, R.dir.y, R.dir.z,
                                     childL[c], childU[c], &tnear, &tfar)) {

                if (tfar > slab_tfar) tfar = slab_tfar;
                if (tnear < slab_tnear) tnear = slab_tnear;

                /* Does the node have a child? If so, we push and continue */
                if (top_stack.octptr->child) {

                  stackpush(&theStack, &top_stack.octptr->child[c],
                            childL[c], childU[c]);
                  /* node does not have a child, either a leaf or data is NULL */

                } else {

                  /* If the node depth is equal to oct_depth, then it is a leaf */
                  if ((top_stack.octptr->depth == oct_depth) ||
                      (top_stack.octptr->depth == (oct_depth-1))) { 
  
                    #ifdef DO_EARTH
                      if (hit_earth[rindex]) {
                        /* modify tnear & tfar to account for the earth */
                        if (t0 > FUDGE) {
                          /* we hit the earth - stop sampling at t0 */
                          if (t0 < tfar) tfar = t0;
                          if (t0 < tnear) tnear = t0;
                        } else if (t1 > FUDGE) {
                          /* we hit the earth - stop sampling at t1 */
                          if (t1 < tfar) tfar = t1;
                          if (t1 < tnear) tnear = t1;
                        }
                      }
                    #endif

                    /* Create t values for ray sampling */
                    #ifdef DO_FISHEYE
                      t_beg = (int)ceil((tnear-Tstart) / Tstep);   
                      if (t_beg < 0) t_beg = 0;
                      t_end = (int)((tfar-Tstart) / Tstep);
                    #else
                      t_beg = (int)ceil(((tnear/ray_ratio)-Tstart) / Tstep);   
                      if (t_beg < 0) t_beg = 0;
                      t_end = (int)(((tfar/ray_ratio)-Tstart)/Tstep);
                    #endif

                    while (t_beg <= t_end) {
                      #ifdef DO_FISHEYE
                        next_t = Tstart + t_beg*Tstep;
                      #else
                        next_t = (Tstart+t_beg*Tstep) * ray_ratio;
                      #endif
                      /* convert to voxel coordinates */
                      Vx = (R.org.x + R.dir.x*next_t - VolumeXmin) * VxScale;
                      Vy = (R.org.y + R.dir.y*next_t - VolumeYmin) * VyScale;
                      Vz = (R.org.z + R.dir.z*next_t - VolumeZmin) * VzScale;

                      /*  voxel layout */
                      /*        +X      */
                      /*     010--110   */
                      /*     /|   /| +Y */
                      /* +Z /000-/100   */
                      /*   / /  / /     */
                      /* 011--111/      */
                      /*  |/   |/       */
                      /* 001--101       */

                      /* compute coordinates of the 000 corner of voxel */
                      x0 = (uLong) Vx;
                      y0 = (uLong) Vy;
                      z0 = (uLong) Vz;
                      /* percentages for interpolation in each dimension */
                      fx = Vx - x0; fy = Vy - y0; fz = Vz - z0;

                      /* grab 8 corners of voxel for tri-lerp */
                      double energy[8] = {0,0,0,0,0,0,0,0};
                      unsigned long smin[3] = {
                        x0>>shift_x, y0>>shift_y, z0>>shift_z };
                      unsigned long smax[3] = { (x0+1)>>shift_x,
                        (y0+1)>>shift_y, (z0+1)>>shift_z };
                      int data_good;
                      if (top_stack.octptr->xLoc == smin[0] && 
                          top_stack.octptr->yLoc == smin[1] && 
                          top_stack.octptr->zLoc == smin[2]) {

                        data_good = get_energy(energy, top_stack.octptr,
                                    x0, y0, z0, smin, smax);
                      } else { /* top_stack isn't the min supergrid (x0,y0,z0) */

                        OCTREE *node = octree_traverse(&octree, oct_depth,
                                       smin[0], smin[1], smin[2]);
                        data_good = get_energy(energy, node, x0,y0,z0, smin, smax);

                      }

                      if (!data_good) { t_beg++; continue; }

                      /* Tri-lerp the energy value */
                      energy_x00 = energy[0] + (energy[1] - energy[0]) * fx;
                      energy_x01 = energy[4] + (energy[5] - energy[4]) * fx;
                      energy_x10 = energy[2] + (energy[3] - energy[2]) * fx;
                      energy_x11 = energy[6] + (energy[7] - energy[6]) * fx;
                      energy_xy0 = energy_x00 + (energy_x10 - energy_x00) * fy;
                      energy_xy1 = energy_x01 + (energy_x11 - energy_x01) * fy;
                      energy_xyz = energy_xy0 + (energy_xy1 - energy_xy0) * fz;

                      raw[rindex] += energy_xyz;
                      t_beg ++;

                    } /* end while t_beg <= t_end */

                  }  /* END if, else would be node does not have children */

                } /* END else for terminal node */

              } /* END if, else would mean volume does not intersect with ray */

            } /* END for each child loop */

          } /* END Stackpop while loop */

          stackdestroy(&theStack);

        } /* end for i */

      } /* end for j */

      slabRenderEnd = get_time(slabRenderStart); 
      slabRenderTime = slabRenderEnd / get_freq(); 
      totalRenderTime += slabRenderTime;

      print_timing("  Rendered in", slabRenderTime, "\n");
      fflush(stdout);

      /* free up malloc'ed memory from this render */
      octree_destroy(&octree);
      octree_destroy(&blanknode);
      oct_depth = 0;
      num_children = 0;
      num_inserts = 0;
      num_sub_allocated = 0;
      num_overwrites = 0;

    } /* end for slab */

    #ifdef DO_MIP_MAP
      free(lower[cur].values);
      free(higher[cur].values);
    #endif

  } /* end for cur */

  #ifdef FIND_BOUNDS
    printf("\n");
/*  printf("  Voxel Bound X:(%.1lf, %.1lf) Y:(%.1lf,%.1lf) Z:(%.1lf,%.1lf)\n",*/
/*          Bxmin, Bxmax, Bymin, Bymax, Bzmin, Bzmax);*/
    printf("  World Bound X:(%.2lf, %.2lf) Y:(%.2lf,%.2lf) Z:(%.2lf,%.2lf)\n",
            VolumeXmin + (Bxmin * VolXrangeDivXres),
            VolumeXmin + (Bxmax * VolXrangeDivXres),
            VolumeYmin + (Bymin * VolYrangeDivYres),
            VolumeYmin + (Bymax * VolYrangeDivYres),
            VolumeZmin + (Bzmin * VolZrangeDivZres),
            VolumeZmin + (Bzmax * VolZrangeDivZres));
    printf("  Number of set_vol() calls = %u\n", vox_count);

    /* update global min/max */
    if (Bxmin < gBxmin) gBxmin = Bxmin;
    if (Bxmax > gBxmax) gBxmax = Bxmax;
    if (Bymin < gBymin) gBymin = Bymin;
    if (Bymax > gBymax) gBymax = Bymax;
    if (Bzmin < gBzmin) gBzmin = Bzmin;
    if (Bzmax > gBzmax) gBzmax = Bzmax;
    printf("  Global Bound X:(%.2lf, %.2lf) Y:(%.2lf,%.2lf) Z:(%.2lf,%.2lf)\n",
            VolumeXmin + (gBxmin * VolXrangeDivXres),
            VolumeXmin + (gBxmax * VolXrangeDivXres),
            VolumeYmin + (gBymin * VolYrangeDivYres),
            VolumeYmin + (gBymax * VolYrangeDivYres),
            VolumeZmin + (gBzmin * VolZrangeDivZres),
            VolumeZmin + (gBzmax * VolZrangeDivZres));

    finish = get_time(start);
    timings = (finish / get_freq()) - total_vol;
    print_timing("\n  Computed Bounds in", timings, "\n");
    return;
  #endif

  /* make energy negative when it hits the earth */
  #ifdef DO_EARTH
    for ( rindex=0 ; rindex<xyres ; rindex++ )
      if (hit_earth[rindex]) raw[rindex] = -raw[rindex];
    free(hit_earth);
  #endif

  /* output timing information */
  print_timing("\n  Rendered volume in", channelTime, "\n");
  totalEndTime = get_time(totalStartTime);
  totalTime = totalEndTime / get_freq();
  print_timing("  Total runtime:", totalTime, "\n\n");
  printf("********************\n\n");

  /* write the raw buffer to a file */
  if ((outfile = open(raw_file, O_WRONLY | O_CREAT, 0644)) < 0)
    ERROR_BAD_OPEN(raw_file);
  write(outfile, buf, raw_header);
  write(outfile, raw, (s.xres*s.yres)*sizeof(float));
  close(outfile);

  free(raw);

} /* end render_image() */

/*---------------------------------------------------------------------------*/

