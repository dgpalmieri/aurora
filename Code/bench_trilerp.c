/* bench_trilerp.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>            /* pow(),exp() */
#include <sys/times.h>       /* for timing */
#include <machine/limits.h>  /* for CLK_TCK */
#include "read_alpha.h"

#define RANDOM_POINTS 10000
double x[RANDOM_POINTS];
double y[RANDOM_POINTS];
double z[RANDOM_POINTS];

double answer[RANDOM_POINTS];

typedef unsigned char      uByte1; /* 0..255 */
typedef unsigned int       uByte4; /* 0..4,294,967,296 */
typedef unsigned long long uByte8; /* 0..billions&billions */

#define TO_FLOAT_16 0.000015258789

main(int argc, char *argv[])
{
  struct tms beg;
  struct tms end;
  clock_t    wall_beg;
  clock_t    wall_end;
  double     wall_time;
  double     cpu_time;
  double     sys_time;
  double     per_second;
  int        xres;
  int        yres;
  int        zres;
  int        zyres;
  double     min;
  double     max;
  uByte1     *voxels;

  double *energy;
  int     i,j,k;
  int     iters;
  /* for tri-lerping */
  int    x0,y0,z0;     /* voxel coordinate of corner 000 */    
  double fx,fy,fz;     /* percentages for interpolation */
  uByte1 *p000; uByte1 *p001; uByte1 *p010; uByte1 *p011;
  uByte1 *p100; uByte1 *p101; uByte1 *p110; uByte1 *p111;
  double alpha000;     /* values extracted from voxels */
  double alpha100;     /*             "                */
  double alpha010;     /*             "                */
  double alpha110;     /*             "                */
  double alpha001;     /*             "                */
  double alpha101;     /*             "                */
  double alpha011;     /*             "                */
  double alpha111;     /*             "                */
  double alphaxy0,alphax00,alphax10; /* interpolated values */
  double alphaxy1,alphax11,alphax01; /*    "                */ 
  double alphaxyz;                   /*    "                */




  if (argc != 3) {
    printf("Usage: %s alpha_filepath iterations\n", argv[0]);
    exit(1);
  }

  iters = atoi(argv[2]);
  if (iters < 1) {
    printf("Invalid number of iterations = %d\n", iters);
    exit(1);
  }

  printf("Reading file %s\n", argv[1]);
  voxels = read_alpha(argv[1], &xres, &yres, &zres, &min, &max, 1);
  if (!voxels) {
    printf("Error reading %s\n", argv[1]);
    exit(1);
  }

  /* pick random points to tri-lerp */
  for ( i=0 ; i<RANDOM_POINTS ; i++ ) {
    x[i] = (xres-1) * drand48();
    y[i] = (yres-1) * drand48();
    z[i] = (zres-1) * drand48();
#if 0
    printf("Tri-lerping [%.4lf,%.4lf,%.4lf]\n", x[i], y[i], z[i]);
#endif
  }

  zyres = zres*yres;

  /* start timing */
  wall_beg = times(&beg);
  printf("wall_beg=%u user=%u sys=%u\n",wall_beg,beg.tms_utime,beg.tms_stime);

  for ( j=0 ; j<iters ; j++ ) {
    for ( i=0 ; i<RANDOM_POINTS ; i++ ) {

      /* compute coordinates of the 000 corner of voxel */
      x0 = (int) x[i]; y0 = (int) y[i]; z0 = (int) z[i];
      /* percentages for interpolation in each dimension */
      fx = x[i] - x0; fy = y[i] - y0; fz = z[i] - z0;

      /* index into array for each corner of the voxel */
      p000 = voxels + 2*(z0 + y0*zres + x0*zyres);
      p001 = p000 + 2;
      p010 = p000 + 2*zres;
      p011 = p010 + 2;
      p100 = p000 + 2*zyres;
      p101 = p100 + 2;
      p110 = p100 + 2*zres;
      p111 = p110 + 2;

      /* unpack and convert to doubles for tri-lerp */
      alpha000 = ((((uByte8) (*p000))<<8) | ((uByte8) (*(p000+1))))*TO_FLOAT_16;
      alpha001 = ((((uByte8) (*p001))<<8) | ((uByte8) (*(p001+1))))*TO_FLOAT_16;
      alpha010 = ((((uByte8) (*p010))<<8) | ((uByte8) (*(p010+1))))*TO_FLOAT_16;
      alpha011 = ((((uByte8) (*p011))<<8) | ((uByte8) (*(p011+1))))*TO_FLOAT_16;
      alpha100 = ((((uByte8) (*p100))<<8) | ((uByte8) (*(p100+1))))*TO_FLOAT_16;
      alpha101 = ((((uByte8) (*p101))<<8) | ((uByte8) (*(p101+1))))*TO_FLOAT_16;
      alpha110 = ((((uByte8) (*p110))<<8) | ((uByte8) (*(p110+1))))*TO_FLOAT_16;
      alpha111 = ((((uByte8) (*p111))<<8) | ((uByte8) (*(p111+1))))*TO_FLOAT_16;

#if 0
printf("(%d,%d,%d) - (%d,%d,%d)\n", x0, y0, z0, x0+1, y0+1, z0+1);
printf("(%lf,%lf,%lf)\n", fx, fy, fz);
printf("000 = %12.10lf  001 = %12.10lf  ", alpha000, alpha001);
printf("010 = %12.10lf  011 = %12.10lf\n", alpha010, alpha011);
printf("100 = %12.10lf  101 = %12.10lf  ", alpha100, alpha101);
printf("110 = %12.10lf  111 = %12.10lf\n", alpha110, alpha111);
#endif

      /* Tri-lerp the Alpha value */
      alphax00 = alpha000 + (alpha100 - alpha000) * fx;
      alphax01 = alpha001 + (alpha101 - alpha001) * fx;
      alphax10 = alpha010 + (alpha110 - alpha010) * fx;
      alphax11 = alpha011 + (alpha111 - alpha011) * fx;
      alphaxy0 = alphax00 + (alphax10 - alphax00) * fy;
      alphaxy1 = alphax01 + (alphax11 - alphax01) * fy;
      alphaxyz = alphaxy0 + (alphaxy1 - alphaxy0) * fz;

answer[i] = alphaxyz;

#if 0
printf("tri-lerp = %12.10lf\n", alphaxyz);
#endif

    }
  }


  /* end timing */  
  wall_end = times(&end);
  printf("wall_end=%u user=%u sys=%u\n",wall_end,end.tms_utime,end.tms_stime);

  wall_time = (wall_end - wall_beg) / (double) CLK_TCK;
  cpu_time = (end.tms_utime - beg.tms_utime) / (double) CLK_TCK;
  sys_time = (end.tms_stime - beg.tms_stime) / (double) CLK_TCK;

  printf("wall time = %.4lf seconds\n", wall_time);
  printf("cpu time = %.4lf seconds\n", cpu_time);
  printf("sys time = %.4lf seconds\n", sys_time);

  printf("%d total tri-lerps\n", iters*RANDOM_POINTS);
  per_second = cpu_time / (iters*RANDOM_POINTS);
  printf("%.8lf      seconds per tri-lerp\n", per_second);
  printf("%.8lf milliseconds per tri-lerp\n", per_second * 1000);
  printf("%.8lf microseconds per tri-lerp\n", per_second * 1000000);


  exit(0);
}
