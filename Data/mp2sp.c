
/* reads in the curtain data files and moves each control point */
/* by the latitude change and divide by 1000 to get to 10^3 km  */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define WHITESPACE " :,\t\n"
#define DEG2RAD (M_PI/180.0)
#define RAD2DEG (180.0/M_PI)

#define MAX_CURTAINS 6
static char *filenames[MAX_CURTAINS] = { "curtainMain.data",
                                         "secondaryCurtain.data",
                                         "secondaryCurtain-1.data",
                                         "tertiaryCurtain.data",
                                         "tertiaryCurtain-1.data",
                                         "clouds.data" }; 

typedef struct { double x, y, z; } Point;
typedef struct { double lat, lon; } Polar;

Point polar2cartesian(Polar p)   
{   
  /* assumes that long=0 is at <-1, 0, 0> */
  /* assumes that lat=90.0 is <0, 1, 0> */
  Point out;
  double phi, theta;
  double sphi; 
  double mag;

  phi = (90.0 - p.lat) * DEG2RAD;
  theta = (p.lon - 90.0) * DEG2RAD;
  sphi = sin(phi);
  out.y = cos(phi);
  out.x = sphi * sin(theta); 
  out.z = -sphi * -cos(theta);
  mag = sqrt(out.x*out.x + out.y*out.y + out.z*out.z);
  out.x /= mag;
  out.y /= mag;
  out.z /= mag;

  return out;
}

Polar cartesian2polar(Point c)
{   
  /* assumes that long=0 is at <-1, 0, 0> */
     /* add 180deg to angle so 0deg=<1,0,0> 90deg=<0,0,-1>, 180deg=<-1,0,0> */
  /* assumes that lat=90.0 is <0, 1, 0> */
  Polar p;
  double dist;
  double phi, theta;

  dist = c.x*c.x + c.z*c.z;
  if (dist < 0.000001) {
    /* north pole */ phi = 0.0; theta = 0.0;
  } else {
    dist = sqrt(dist);
    phi = acos(c.y);
    if (c.z>=0.0) theta = M_PI - acos(c.x/dist);
    else          theta = acos(c.x/dist) - M_PI;
  }
  p.lat = 90.0 - (phi * RAD2DEG);  /* invert as north pole is lat=90 */;
  p.lon = (theta * RAD2DEG);       /* long=0 is down -x axis */

  return p;
}


main()
{    
  FILE *fp;
  char inputline[256];
  char *tokens;
  int frame;
  int i,j,c,s,t;
  double X, Y, Z;
  double mag;
  Point cart;
  Polar p;
  double theta, cos_theta, sin_theta;
  double tmpx, tmpy, tmpz;

#if 0
  cart.x = 0.0;
  cart.y = 1.0;
  cart.z = 0.0;
  p = cartesian2polar(cart);
  printf("<%.2lf,%.2lf,%.2lf> -> ", cart.x, cart.y, cart.z);
  printf("la=%.2lf lo=%.2lf\n ", p.lat, p.lon);
#endif

  /* read in each of the curtain files */
  for ( c=0 ; c<MAX_CURTAINS ; c++ ) {

    fp = fopen(filenames[c], "r");
    if (!fp) { printf("Can't open %s\n", filenames[c]); exit(1); }

    printf("************* Reading %s ...\n", filenames[c]);

    while (fgets(inputline, 255, fp) != NULL) {

      if ((inputline[0] >= '0' && inputline[0] <= '9')  ||
          (inputline[0] == '-')) {

        /* get the XYZ values of this point */
        tokens = strtok(inputline, WHITESPACE);
        if (tokens == NULL) continue; /* skip empty lines */
        X = atof(tokens);
        Y = atof(strtok(NULL, WHITESPACE));
        Z = atof(strtok(NULL, WHITESPACE));

        cart.x = X;
        cart.y = Y;
        cart.z = Z;

        /* rotate 24 degrees around y */
        theta = 24.0 * DEG2RAD;
        cos_theta = cos(theta);
        sin_theta = sin(theta);

        tmpx = cart.x;
        tmpz = cart.z;
        cart.x =  tmpx*cos_theta + tmpz*sin_theta;
        cart.z = -tmpx*sin_theta + tmpz*cos_theta;

        /* rotate -12 degrees around x */
        theta = -12.0 * DEG2RAD;
        cos_theta = cos(theta);
        sin_theta = sin(theta);

        tmpy = cart.y;
        tmpz = cart.z;
        cart.y = tmpy*cos_theta - tmpz*sin_theta;
        cart.z = tmpy*sin_theta + tmpz*cos_theta;

        /* rotate -24 degrees around y */
        theta = -24.0 * DEG2RAD;
        cos_theta = cos(theta);
        sin_theta = sin(theta);

        tmpx = cart.x;
        tmpz = cart.z;
        cart.x =  tmpx*cos_theta + tmpz*sin_theta;
        cart.z = -tmpx*sin_theta + tmpz*cos_theta;

        printf("%.6lf %.6lf %.6lf\n", cart.x, cart.y, cart.z);

      } else {

        /* just write it out unchanged */
        printf("%s",inputline);

      }

    }

    printf("************* End of file reached\n");
    fclose(fp);

  }

}
