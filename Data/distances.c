#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define WHITESPACE " :,\t\n"

#define MAX_CURTAINS 6
static char *filenames[MAX_CURTAINS] = { "n5p20/1.data", "n5p20/2.data",
  "n5p20/2-1.data", "n5p20/3.data", "n5p20/3-1.data", "n5p20/4.data" }; 
static double CurLength[MAX_CURTAINS] =
  { 20303.0, 2709.0, 1952.0, 4145.0, 3186.0, 9219.0};

main()
{    
  FILE *fp;
  char inputline[256];
  char *tokens;
  int frame;
  int i,j,c,s,t;
  double lastX, lastY, lastZ;
  double X, Y, Z;
  double dist;
  double curve_dist;
  double tot_dist;
  int    tot_count;
  int num_frames;
  int num_points;

  /* read in each of the curtain files */
  for ( c=0 ; c<MAX_CURTAINS ; c++ ) {

    fp = fopen(filenames[c], "r");
    if (!fp) { printf("Can't open %s\n", filenames[c]); exit(1); }

    printf("************* Reading %s ...\n", filenames[c]);

    num_frames = 0;
    tot_dist = 0.0;
    tot_count = 0;

    while (fgets(inputline, 255, fp) != NULL) {

      if (inputline[0] == 'f' && inputline[1] == 'r') {

        /* found a new frame */
        if (num_frames > 0 && num_points > 0) {
          /* output distance of last spline */
          printf("Frame %d distance %lf\n", num_frames, curve_dist);
          tot_dist += curve_dist;
          tot_count ++;
        }
        num_frames ++;
        num_points = 0;
        curve_dist = 0.0;

      } else if ((inputline[0] >= '0' && inputline[0] <= '9')  ||
          (inputline[0] == '-')) {

        lastX = X; lastY = Y; lastZ = Z;

        /* convert this coordinate to km */
        tokens = strtok(inputline, WHITESPACE);
        if (tokens == NULL) continue; /* skip empty lines */
        X = atof(tokens);
        Y = atof(strtok(NULL, WHITESPACE));
        Z = atof(strtok(NULL, WHITESPACE));

        if (num_points > 0) {
           /* compute distance from last point to this point */
          dist = sqrt((X - lastX) * (X - lastX) +
                      (Y - lastY) * (Y - lastY) +
                      (Z - lastZ) * (Z - lastZ));
          curve_dist += dist;

printf("%d %lf\n", num_points, curve_dist / 1000.0);

        }
        num_points ++;

/*        printf("Last = %lf, %lf, %lf\n", lastX, lastY, lastZ);*/
/*        printf("New = %lf, %lf, %lf\n", X, Y, Z);*/
/*      if (dist > 0.001 && dist < 50.0)*/
/*        printf("Dist=%lf\n", dist);*/

      }

    }

    printf("Avg curve distance = %lf\n", tot_dist / tot_count);
    printf("************* End of file reached\n");
    fclose(fp);

  }

}
