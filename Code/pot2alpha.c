#include <stdio.h>
#include <stdlib.h>
#include "read_potential.h"
#include "pot_2_energy.h"
#include "energy_2_alpha.h"
#include "min_max.h"

const int z_steps  =    51;
const double z_min =  70.0; /* altitude at Z=z_min */
const double z_max = 150.0; /* altitude at Z=z_max */

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
#ifdef CROP_IT
int xmin=200;
int xmax=650;
int ymin=200;
int ymax=1200;
#endif


  if (argc != 2) {
    printf("Usage: %s potential_slice_number\n", argv[0]);
    exit(1);
  }

  sprintf(buf,"/Users/genetti/Documents/Aurora/Otto/03-06-08/pot%02d",
/*sprintf(buf,"/Users/genetti/Documents/Aurora/Otto/03-02-11/pot%02d",*/
          atoi(argv[1]));
  printf("Reading file %s\n", buf);
  if (!read_potential(buf, &poxmax, &poymax, &potmax, &time_step, &xpo,
       &ypo, &zpo,  &pot, &pot_min, &pot_max, 1, 0)) {
     printf("Error reading %s\n", buf);
     exit(1);
  }
  printf("Range of potential values = [%.2e, %.2e]\n", pot_min, pot_max);  
#ifdef CROP_IT
/* blow away anything outside bounding box */
for ( j=0 ; j<poymax; j++ ) {
  for ( i=0 ; i<poxmax; i++ ) {
    if ( i<xmin || i>xmax || j<ymin || j>ymax)
      pot[i+j*poxmax] = pot_min;
  }
}
#endif
  /* calculate energy volume */
  if (!(energy = pot_2_energy(pot, poxmax, poymax, pot_min,
                              z_min, z_max, z_steps, 1))) {
    printf("Error creating 3D energy volume\n");
    exit(1);
  }

  /* dump to alpha file */
  find_min_max(energy, potmax*z_steps, &energy_min, &energy_max);
  energy_range = energy_max - energy_min;
  if (!energy_2_alpha("/Users/genetti/Documents/Aurora/Otto/aurora.alpha",  
                     energy, poxmax, poymax, z_steps,
                     energy_min + energy_range*0.02,
                     energy_range*0.98, 1)) {
    printf("Error writing %s\n", buf);
    exit(1);
  }

  exit(0);
}
