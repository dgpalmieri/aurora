#include <stdio.h>
#include <stdlib.h>
#include "read_potential.h"
#include "pot_2_energy.h"
#include "energy_2_alpha.h"
#include "min_max.h"

main(int argc, char *argv[])
{
  char   buf[80];
  int    poxmax;
  int    poymax;
  int    potmax;
  float  time_step;
  float  pot_min;
  float  pot_max;
  float *xpo;
  float *ypo;
  float  zpo;
  float *pot;
  double *energy;
  int     i,j;
  double energy_min, energy_max, energy_range;
  FILE *fp;
  int    start, stop, step;
  int    xymax;

  if (argc != 6) {
    printf("Usage: %s path_to_pot height start stop step\n", argv[0]);
    exit(1);
  }

  start = atoi(argv[3]);
  stop  = atoi(argv[4]);
  step  = atoi(argv[5]);

  for ( i=start ; i<=stop ; i+=step ) {

    sprintf(buf, argv[1], i);
    printf("Reading file %s\n", buf);
    if (!read_potential(buf, &poxmax, &poymax, &potmax, &time_step, &xpo,
         &ypo, &zpo,  &pot, &pot_min, &pot_max, 1, 0)) {
       printf("Error reading %s\n", buf);
       exit(1);
    }
    printf("Range of potential values = [%.2e, %.2e]\n", pot_min, pot_max);  

    xymax = poxmax*poymax;
    energy = (double *) malloc( sizeof(double)*xymax );
    if (!energy) {
      printf("Malloc error for energy\n");
      return 0;
    }

    /* scale potential data (taken from moddep Matlab code) */
    for ( i=0 ; i<xymax ; i++ ) {
      energy[i] = (pot[i] - pot_min) + 1.0;
      energy[i] = pow(energy[i],2.42) / 1.602e-9;
    }
 
#if 1
    printf("Energy after scale from moddep:\n");
    double_array_overview(energy, poxmax, poymax);
    printf("Range = [%e, %e]\n",
           find_min(energy, xymax), find_max(energy, xymax));
#endif

    /* write out to an energy file */
    sprintf(buf, "energy.%d", i);
    if ((fp=fopen(buf, "wb")) == 0) {
      printf("Can't open file %s\n", buf);
      exit(1);
    }
    if (fwrite(energy, sizeof(double), poxmax*poymax, fp) != poxmax*poymax) {
      printf("Bad write to %s\n", buf);
      exit(1);
    }
    fclose(fp);

    /* write out to a ppm file */
    find_min_max(energy, potmax*2, &energy_min, &energy_max);
    energy_range = energy_max - energy_min;
    sprintf(buf, "energy.%02d.ppm", i);
    energy_2_ppm_file(buf, poxmax, poymax, energy, energy_min, energy_range);

    free(energy);
    free(pot);
    free(ypo);
    free(xpo);
  }

  exit(0);
}

