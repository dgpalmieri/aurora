#include <stdio.h>
#include <math.h>

static char *filename = "Binc_200km_03oct31.txt";
float inclination[31][360];

static double val_x[9] = { 0.0, 0.5, 1.0, 0.5, 0.0, -0.5, -1.0, -0.5, 0.0 };
static double val_z[9] = { 1.0, 0.5, 0.0, -0.5, -1.0, -0.5, 0.0, 0.5, 1.0 };
static double val_y[11] = { 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 7.7, 8.8, 9.9, 10.0 };

main()
{
  FILE *fp;
  int lon, lat;
  float inc;
  double lo, la;
  double x, y, z;
  double len;
  int ilo, ila;
  int i,j;
  double S;
  double phi, theta;

  printf("Opening %s\n", filename);
  fp = fopen(filename, "r");
  if (fp == 0) { printf("Can't open %s\n", filename); exit(1); }

  /* get the inclinations from the file */
  while (fscanf(fp, "%d %d %f\n", &lat, &lon, &inc) == 3) {
    inclination[lat-60][lon+180] = inc;
  }
  fclose(fp);

  for ( lat=0 ; lat<=30 ; lat++ ) {
    printf("latitude = %d", lat+60);
    for ( lon=0 ; lon<=359 ; lon++ ) {
      if (lon % 8 == 0) printf("\n");
      printf("%f ", inclination[lat][lon]);
    }
    printf("\n");
  }

  /* convert vector to lat/long */
  x = 0.0;
  y = 0.0;
  z = 0.0;

  for ( j=0 ; j<11 ; j++ ) {
    for ( i=0 ; i<9 ; i++ ) {

    x = val_x[i];
    y = val_y[j];
    z = val_z[i];

/*  printf("Vec = <%.1lf,%.1lf,%.1lf> -> ", x, y, z);*/

    /* normalize (if needed) */
    len = sqrt(x*x + y*y + z*z);
    x /= len; y /= len; z /= len;

    printf("<%.1lf,%.1lf,%.1lf> ", x, y, z);

    S = x*x + z*z;
    if (S < 0.000001) {
      phi = 0.0;
      theta = 0.0;
    } else {
      S = sqrt(S);
      phi = acos(y);
      if (x>=0.0) theta = M_PI - acos(z/S);
      else        theta = acos(z/S) - M_PI;
    }
    la = 90.0 - (phi * 180.0 / M_PI); /* map to degrees w/equater=0.0 */;
    lo = (theta * 180.0 / M_PI); /* long=0 is down -z axis */
    printf("lat = %.1lf long = %.1lf ", la, lo);
    fflush(stdout);

    if (la < 60.0) {

      printf("-> out of range\n");

    } else {

      ilo = (int) lo;
      ila = (int) la - 60;
      printf("-> %.1lf\n", inclination[ila][ilo]);

/*
      printf("inclination[%d][%d] = %f\n", ila, ilo, inclination[ila][ilo]);
*/

      /* convert back to a vector using new inclination */
      phi = 90.0 - inclination[ila][ilo];
      x = 
      z = 
      y = cos(phi);


    }

   }
   printf("\n");
  }

}
