#include <stdio.h>
#include "read_potential.h"
#include "write_potential.h"

main(int argc, char *argv[])
{
  int   xmax1, xmax2;
  int   ymax1, ymax2;
  int   xymax1, xymax2;
  float time1, time2, itime;
  float *xpo1, *xpo2;
  float *ypo1, *ypo2;
  float  zpo1, zpo2;
  float *pot1, *pot2, *ipot;
  float  min1, min2;
  float  max1, max2;
  int    num_interps;
  float  time_step;
  float  fx;
  int    i,j;
  char   buf[80];
  int    out_num;

  if (argc != 6) {
    printf("Usage: %s pot1 pot2 interps out_base out_num_start\n", argv[0]);
    exit(1);
  }

  num_interps = atoi(argv[3]);
  if (num_interps <= 0) {
    printf("Number of interpolations must be > 0\n");
    exit(2);
  }

  out_num = atoi(argv[5]);
  if (out_num <= 0) {
    printf("Output numbering sequence must be positive\n");
    exit(2);
  }

  /* read in first potential slice */
  printf("Reading file %s\n", argv[1]);
  if (!read_potential(argv[1], &xmax1, &ymax1, &xymax1, &time1, &xpo1,
      &ypo1, &zpo1, &pot1, &min1, &max1, 1, 0)) {
    printf("Error reading %s\n", argv[1]);
    exit(3);
  }

  /* read in second potential slice */
  printf("Reading file %s\n", argv[2]);
  if (!read_potential(argv[2], &xmax2, &ymax2, &xymax2, &time2, &xpo2,
      &ypo2, &zpo2, &pot2, &min2, &max2, 1, 0)) {
    printf("Error reading %s\n", argv[2]);
    exit(4);
  }

  /* basic sanity checks */
  if (xmax1 != xmax2 || ymax1 != ymax2 || xymax1 != xymax2) {
    printf("Potential slice resolutions do not match\n");
    exit(5);
  }
  if (xpo1[0] != xpo2[0] || ypo1[0] != ypo2[0] || zpo1 != zpo2) {
    printf("Potential grids do not match\n");
    exit(6);
  }

  /* malloc array to hold interpolated potential values */
  ipot = (float *) malloc (sizeof(float) * xymax1);
  if (!ipot) {
    printf("Unable to malloc ipot[%d elements]\n", xymax1);
    exit(7);
  }

  /* compute the interpolated potential slices */
  time_step = (time2 - time1) / (num_interps + 1);
  for ( i=1 ; i<=num_interps ; i++ ) {

    printf("Computing interpolation %d\n", i);

    itime = time1 + time_step * i;
    fx = (float) i / (num_interps + 1);
    for ( j=0 ; j<xymax1 ; j++ )
      ipot[j] = pot1[j] + (pot2[j]-pot1[j])*fx;

/*  sprintf(buf,"%s.%03d",argv[1], i);*/
    sprintf(buf, argv[4], out_num+i-1);
    printf("Writing file %s\n", buf);
    write_potential(buf, xmax1, ymax1, itime, xpo1, ypo1, zpo1, ipot, 1);

  }

  exit(0);
}
