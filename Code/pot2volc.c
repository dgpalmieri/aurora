#include <stdio.h>
#include <stdlib.h>
#include "read_potential.h"
#include "pot_2_energy.h"
#include "energy_2_volc.h"
#include "min_max.h"

const int z_steps  =    4;
const double z_min =  90.0; /* altitude at Z=z_min */
const double z_max = 130.0; /* altitude at Z=z_max */
const double x_min =  -8.0;
const double x_max =   8.0;
const double y_min =   0.0;
const double y_max =  24.0;

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
  int     i;
  double energy_min, energy_max, energy_range;

  if (argc != 2) {
    printf("Usage: %s potential_slice_number\n", argv[0]);
    exit(1);
  }

  sprintf(buf,"/Users/genetti/Documents/Aurora/Otto/03-06-02/pot%02d",
/*sprintf(buf,"/Users/genetti/Documents/Aurora/Otto/03-02-11/pot%02d",*/
          atoi(argv[1]));
  printf("Reading file %s\n", buf);
  if (!read_potential(buf, &poxmax, &poymax, &potmax, &time_step, &xpo,
       &ypo, &zpo, &pot, &pot_min, &pot_max, 1, 0)) {
     printf("Error reading %s\n", buf);
     exit(1);
  }
  printf("Range of potential values = [%.2e, %.2e]\n", pot_min, pot_max);  

  /* calculate energy volume */
  if (!(energy = pot_2_energy(pot, poxmax, poymax, pot_min,
                              z_min, z_max, z_steps, 1))) {
    printf("Error creating 3D energy volume\n");
    exit(1);
  }

  /* dump to volc file */
  find_min_max(energy, potmax*z_steps, &energy_min, &energy_max);
  energy_range = energy_max - energy_min;
  if (!energy_2_volc("/Users/genetti/Documents/Aurora/Otto/aurora.volc",  
                     energy, poxmax, poymax, z_steps,
                     energy_min + energy_range*0.02,
                     energy_range*0.98, 1)) {
    printf("Error writing %s\n", buf);
    exit(1);
  }

  exit(0);
}
