/* reads in a curtain file and creates a new one */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define WHITESPACE " :,\t\n"
#define DEG2RAD (M_PI/180.0)
#define RAD2DEG (180.0/M_PI)
#define LAT_SHIFT -5.0
#define LON_SHIFT  15.0

/* secondary <orig> <0.6, 0.2, -1.2>, <0.4, 0.3, -2.1> */
/* tertiary <.7,.1,1> <orig> <0.9, 0.05, -0.8> <0.7,0.15,-1.6> */
/* 0.8, .10, -1.2 */
/* new 2 0.5, 0.15, -1.6 */
#if 1
#define SHRINKAGE 0.5
#define SHRINK_OFFSET 0.15
#define LAT_OFFSET -1.6
#endif
/* new 3 0.7, 0.1, -1.4 */
#if 0
#define SHRINKAGE 0.7
#define SHRINK_OFFSET 0.1
#define LAT_OFFSET -1.4
#endif


#if 0
#define SHRINKAGE 0.7
#define SHRINK_OFFSET 0.1
#define LAT_OFFSET 1.0
#define SHRINKAGE 0.7
#define SHRINK_OFFSET 0.15
#define LAT_OFFSET -1.6
#define SHRINKAGE 0.9
#define SHRINK_OFFSET 0.05
#define LAT_OFFSET -0.8
#endif

static char *filename = "secondaryCurtain.data";
/*
static char *filename = "curtainMain.data";
static char *filename = "tertiaryCurtain.data";
static char *filename = "clouds.data";
*/

#define MAX_FRAMES 825
#define MAX_POINTS 110

int num_points[MAX_FRAMES];
int frame_no[MAX_FRAMES];
double X[MAX_FRAMES][MAX_POINTS];
double Y[MAX_FRAMES][MAX_POINTS];
double Z[MAX_FRAMES][MAX_POINTS];

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

int read_curtain(FILE *fp)
{
  int fr=0;  /* current frame number */
  char inputline[256]; 
  char *tokens;

  /* I am ASSuming that all files start with the same frame */
  /* number and contain the same number of frames           */

  while (fr <= MAX_FRAMES) {
        
    if (fgets(inputline, 255, fp) == NULL) break;
      
    tokens = strtok(inputline, WHITESPACE);
    if (tokens == NULL) continue; /* skip empty lines */
    
    if (strncasecmp("frame", tokens, 5) == 0) {
      /* found a new frame */
      fr ++;
      num_points[fr] = 0;
      strtok(NULL, WHITESPACE);
      frame_no[fr-1] = atoi(strtok(NULL, WHITESPACE));
    } else if (strncasecmp("visible", tokens, 7) == 0) {
      continue;
    } else if (strncasecmp("invisible", tokens, 9) == 0) {
      continue;
    } else if (strncasecmp("no", tokens, 2) == 0) {
      continue;
    } else {
      /* read the control point */
      X[fr-1][num_points[fr-1]] = atof(tokens);
      Y[fr-1][num_points[fr-1]] = atof(strtok(NULL, WHITESPACE));       
      Z[fr-1][num_points[fr-1]] = atof(strtok(NULL, WHITESPACE));
      num_points[fr-1] ++;
    }

  } 
    
  return fr;
}   


main()
{    
  FILE *fp;
  char inputline[256];
  char *tokens;
  int frame;
  int i,j,c,s,t;
  double mag;
  Point cart;
  Polar newp, p;
  int num_frames;
  double min_lon, max_lon;
  double map_range;
  double start_map, end_map;


  fp = fopen(filename, "r");
  if (!fp) { printf("Can't open %s\n", filename); exit(1); }

  printf("************* Reading %s ...\n", filename);
  num_frames = read_curtain(fp);

  if (num_frames) {

    for ( i=0 ; i<num_frames ; i++ ) {

      if (num_points[i]) {

        for ( j=0 ; j<num_points[i] ; j++ ) {

          /* normalize the vector and save in cart */
          mag = sqrt(X[i][j]*X[i][j] + Y[i][j]*Y[i][j] + Z[i][j]*Z[i][j]);
          cart.x = X[i][j]/mag; cart.y = Y[i][j]/mag; cart.z = Z[i][j]/mag;
          p = cartesian2polar(cart);
#if 0
          printf("%d: (%.1lf,%.1lf,%.1lf) ", j, X[i][j], Y[i][j], Z[i][j]);
          printf("la=%lf lo=%lf\n", p.lat, p.lon);
#endif

          if (p.lon < 0.0) p.lon = 360.0 + p.lon;

          /* mapping is */
          /* long=0  lon=0 */
          /* long=90 lon=90 */
          /* long=180 lon=180 */
          /* long=-180 lon=180 */
          /* long=-90  lon=270 */
          /* long=-0   lon=360 */

          if ( j==0 ) min_lon = p.lon;
          else if ( j==num_points[i]-1 ) max_lon = p.lon;

        }

#if 0
        printf("Fr %d - ", i);
        printf("orig: [%.2lf, %.2lf] ", min_lon, max_lon);
#endif

        /* now shrink the range in longitude by input percentage */
        map_range = (max_lon-min_lon)*SHRINKAGE;
        start_map = min_lon + (max_lon-min_lon)*SHRINK_OFFSET;
        end_map = start_map + map_range;

#if 0
        printf("new: [%.2lf, %.2lf] ", start_map, end_map);
#endif

        if (end_map > 180.0) end_map = end_map - 360.0;

#if 0
        printf("lo=%.2lf\n", end_map);
#endif

        /* re-map points for this frame */
        printf("visible at\n");
        printf("frame = %d\n", frame_no[i]);

        for ( j=0 ; j<num_points[i] ; j++ ) {

          /* convert point to polar coordinates */
          mag = sqrt(X[i][j]*X[i][j] + Y[i][j]*Y[i][j] + Z[i][j]*Z[i][j]);
          cart.x = X[i][j]/mag; cart.y = Y[i][j]/mag; cart.z = Z[i][j]/mag;
          p = cartesian2polar(cart);
          if (p.lon < 0.0) p.lon = 360.0 + p.lon;

          /* re-map */
          newp.lat = p.lat + LAT_OFFSET;
          newp.lon = start_map + map_range *((p.lon - min_lon)/(max_lon - min_lon));

          /* convert point back to cartesian space */
          cart = polar2cartesian(newp);
          cart.x *= mag; cart.y *= mag; cart.z *= mag;
          printf("%.6lf %.6lf %.6lf\n", cart.x, cart.y, cart.z);

        }

        printf("\n");

      } else {

        printf("invisible at\n");
        printf("frame = %d\n", frame_no[i]);
        printf("no points output while invisible\n");
        printf("\n");

      }

    }

  }

  printf("************* End of file reached\n");
  fclose(fp);

}
