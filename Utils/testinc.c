#include <stdio.h>
#include <math.h>

#define DEG2RAD (M_PI/180.0)
#define RAD2DEG (180.0/M_PI)

static char *filename = "Binc_200km_03oct31.txt";
float inclination[31][360];

static double val_x[17] = { 0.0, 0.25, 0.5, 0.75, 1.0, 0.75, 0.5, 0.25,
                           0.0,-0.25,-0.5,-0.75,-1.0,-0.75,-0.5,-0.25, 0.0 };
static double val_z[17] = { 1.0, 0.75, 0.5, 0.25, 0.0, -0.25, -0.5, -0.75,
                           -1.0,-0.75,-0.5,-0.25, 0.0,  0.25,  0.5,  0.75, 1.0 };
static double val_y[11] = { 0.0, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.0 };

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
  double sphi;
  double ctheta, stheta;
double lo_frac, la_frac;
double i00, i01, i10, i11, ix0, ix1, ixx;
double tlo, tla;

  printf("Opening %s\n", filename);
  fp = fopen(filename, "r");
  if (fp == 0) { printf("Can't open %s\n", filename); exit(1); }

  /* get the inclinations from the file */
  while (fscanf(fp, "%d %d %f\n", &lat, &lon, &inc) == 3) {
    inclination[90-lat][lon+180] = inc;
  }
  fclose(fp);

#if 0
  /* print out for include file */
  printf("static float inclination[31][360] = {\n");
  for ( lat=0 ; lat<=30 ; lat++ ) {
    printf("{ ");
    for ( lon=0 ; lon<=359 ; lon++ ) {
      printf("%6.3f, ", inclination[lat][lon]);
      if ((lon+1) % 9 == 0) printf("\n  ");
    }
    printf("},\n");
  }
  printf("};\n");
  for ( lat=0 ; lat<=30 ; lat++ ) {
    printf("latitude = %d", 90-lat);
    for ( lon=0 ; lon<=359 ; lon++ ) {
      if (lon % 10 == 0) printf("\n");
      printf("%6.3f ", inclination[lat][lon]);
    }
    printf("\n");
  }
#endif

  /* convert vector to lat/long */
  x = 0.0;
  y = 0.0;
  z = 0.0;

#if 0
  for ( j=0 ; j<11 ; j++ ) {
    for ( i=0 ; i<17 ; i++ ) {

    x = val_x[i];
    y = val_y[j];
    z = val_z[i];
#endif

x=0.003751;
y=0.915517;
z=0.402261;
    /* normalize (if needed) */
    len = sqrt(x*x + y*y + z*z);
    x /= len; y /= len; z /= len;

    printf("<%.5lf,%.5lf,%.5lf> ", x, y, z);

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
    la = 90.0 - (phi * RAD2DEG); /* map to degrees w/equater=0.0 */;
    lo = (theta * RAD2DEG); /* long=0 is down -z axis */
    printf("lat = %.1lf long = %.1lf ", la, lo);

    if (la < 60.0) {

      printf("-> out of range\n");

    } else {

      tlo = lo + 180.0;
      ilo = (int) tlo;
      lo_frac = tlo - ilo;

      tla = 90.0 - la;
      ila = (int) tla;
      la_frac = tla - ila;

      i00 = inclination[ila][ilo];
      i10 = inclination[ila][ilo+1];
      i01 = inclination[ila+1][ilo];
      i11 = inclination[ila+1][ilo+1];

      ix0 = i00 + (i10 - i00)*lo_frac;
      ix1 = i01 + (i11 - i01)*lo_frac;
      ixx = ix0 + (ix1 - ix0)*la_frac;
      phi = 90.0 - ixx;

#if 0
printf("\n");
printf("lo=%lf la=%lf\n", tlo, tla);
printf("lo_frac=%lf la_frac=%lf\n", lo_frac, la_frac);
printf("00=%d,%d 10=%d,%d 01=%d,%d 11=%d,%d\n",ilo,ila,ilo+1,ila,ilo,ila+1,ilo+1,ila+1);
printf("i00=%lf i10=%lf i01=%lf i11=%lf\n", i00, i10, i01, i11);
printf("ix0=%lf ix1=%lf ixx=%lf\n", ix0, ix1, ixx);
#endif

#if 0
      phi = 90.0 - inclination[ila][ilo];
      phi = 90.0 - la;
#endif

      printf("-> %.1lf D ", 90.0 - phi);
      phi *= DEG2RAD;
      y = cos(phi);

      sphi = sin(phi);
      ctheta = cos(theta);
      stheta = sin(theta);
      x = sphi * stheta;
      z = - sphi * ctheta;
      len = sqrt(x*x + y*y + z*z);
      x /= len; y /= len; z /= len;
      printf("-> <%.5lf,%.5lf,%.5lf>\n", x, y, z);

    }

#if 0
   }
   printf("\n");
  }
#endif

}
