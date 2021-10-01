#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define WHITESPACE " :,\t\n"

#define MAX_CURTAINS 4
static char *filenames[MAX_CURTAINS] = { "curtainMain.data",
  "secondaryCurtain.data", "tertiaryCurtain.data", "clouds.data" }; 

main()
{    
  FILE *fp;
  char inputline[256];
  char *tokens;
  int frame;
  int i,j,c,s,t;
  double X, Y, Z;

  /* read in each of the curtain files */
  for ( c=0 ; c<MAX_CURTAINS ; c++ ) {

    fp = fopen(filenames[c], "r");
    if (!fp) { printf("Can't open %s\n", filenames[c]); exit(1); }

    printf("************* Reading %s ...\n", filenames[c]);

    while (fgets(inputline, 255, fp) != NULL) {

      if ((inputline[0] >= '0' && inputline[0] <= '9')  ||
          (inputline[0] == '-')) {

        /* convert this coordinate to km */
        tokens = strtok(inputline, WHITESPACE);
        if (tokens == NULL) continue; /* skip empty lines */
        X = atof(tokens);
        Y = atof(strtok(NULL, WHITESPACE));
        Z = atof(strtok(NULL, WHITESPACE));

        /* datasets: +Y is the north pole, earth is centered at 0,0,0 */
        /* average distance to bottom of curtain is 9.13 in original data */
        /* earth R=6378.1km + 100km to bottom of curtain */
        /* multiply each point by 6478.1/9.13 = 709.54 to convert to km */

        printf("%.6lf %.6lf %.6lf\n", X*709.54, Y*709.54, Z*709.54);

      } else {

        /* just write it out unchanged */
        printf("%s",inputline);

      }

    }

    printf("************* End of file reached\n");
    fclose(fp);

  }

}
