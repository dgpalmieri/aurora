#include <stdio.h>
#include <stdlib.h>
#include "pot_2_ppm_file.h"
#include "float_array_overview.h"
#include "read_potential.h"

main(int argc, char *argv[])
{
  int    poxmax;
  int    poymax;
  int    potmax;
  float  time_step;
  float *xpo;
  float *ypo;
  float  zpo;
  float *pot;
  float  min_val;
  float  max_val;
  double global_min;
  double global_max;
  char   buf[80];
  int    i,j;
  int    start, stop, step;

  if (argc != 6) {
    printf("Usage: %s filebase start stop step outbase\n", argv[0]);
    exit(1);
  }

  start = atoi(argv[2]);
  stop  = atoi(argv[3]);
  step  = atoi(argv[4]);

  /* determine global min/max from potential slices */
  for ( i=start ; i<=stop ; i+=step ) {
    sprintf(buf, argv[1], i);
    printf("Reading file %s\n", buf);
    if (!read_potential(buf, &poxmax, &poymax, &potmax, &time_step, &xpo,
         &ypo, &zpo, &pot, &min_val, &max_val, 0, 1)) {
       printf("Error reading %s\n", buf);
       exit(1);
    }
    free(pot); free(ypo); free(xpo);
    printf("Range = [%.2e, %.2e]\n", min_val, max_val);  
    if (i==start) {
      global_min = min_val;
      global_max = max_val;
    } else {
      if (min_val < global_min) global_min = min_val;
      if (max_val > global_max) global_max = max_val;
    }
  }               
  printf("Global Range = [%.2e, %.2e]\n", global_min, global_max);  

  /* dump out the raw potential slices mapped to global min/max */
  for ( i=start ; i<=stop ; i+=step ) {
    sprintf(buf, argv[1], i);
    printf("Reading file %s\n", buf);
    if (!read_potential(buf, &poxmax, &poymax, &potmax, &time_step, &xpo,
         &ypo, &zpo, &pot, &min_val, &max_val, 1, 0)) {
      printf("Error reading %s\n", buf);
      exit(1);
    }
    sprintf(buf, argv[5], i);
    printf("Writing file %s\n", buf);
    pot_2_ppm_file(buf, poxmax, poymax, pot, global_min, global_max-global_min);
    free(pot); free(ypo); free(xpo);
  }               

}
