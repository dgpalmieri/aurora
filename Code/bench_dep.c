/* bench_dep.c */

#include <stdio.h>
#include <stdlib.h>          /* +drand48() */
#include <math.h>            /* pow(),exp() */
#include <sys/times.h>       /* for timing */
#include <machine/limits.h>  /* for CLK_TCK */
#include "read_potential.h"  /* for read_potential() */

#define RANDOM_POINTS 5
int x[RANDOM_POINTS];
int y[RANDOM_POINTS];

#define Z_STEPS 250
double answers[Z_STEPS];

const double z_min  =  50.0; /* altitude at Z=z_min */
const double z_max  = 250.0; /* altitude at Z=z_max */
const double z_base = 100.0;


/* values for deposition function */
const double no = 1.57e12;
const double mo = 16*1.672e-24;
const int    to = 300;
const int     g = 981;
const double kb = 1.38e-16;
const double c1 = 3.233;
const double c2 = 2.56588;
const double c3 = 2.2541;
const double c4 = 0.7297198;
const double c5 = 1.106907;
const double c6 = 1.71349;
const double c7 = 1.8835444;
const double c8 = 0.86472135;
const double e1 = 1.0 / 1.65;

const int be_verbose=1;

main(int argc, char *argv[])
{
  int        poxmax;
  int        poymax;
  int        potmax;
  float      pot_min;
  float      pot_max;
  float      time_step;
  float      *xpo;
  float      *ypo;
  float      zpo;
  float      *pot;
  struct tms beg;
  struct tms end;
  clock_t    wall_beg;
  clock_t    wall_end;
  double     wall_time;
  double     cpu_time;
  double     sys_time;
  double     per_second;
  int        i,j,k;
  int        index;
  int        iters;
  const double lam = (mo*g)/(kb*to);
  const double mo_x_no = mo*no;
  double       mu, A1, A2;
  double     z_value;
  double     *energy;
  double     *Emax;
  double     rho;
  double     Ren;
  const double z_range = z_max-z_min;


  if (argc != 3) {
    printf("Usage: %s potential_filepath iterations\n", argv[0]);
    exit(1);
  }

  iters = atoi(argv[2]);
  if (iters < 1) {
    printf("Invalid number of iterations = %d\n", iters);
    exit(1);
  }

  /* read in a potential file to do benchmarking of deposition function */
  printf("Reading file %s\n", argv[1]);
  if (!read_potential(argv[1], &poxmax, &poymax, &potmax, &time_step, &xpo,
       &ypo, &zpo, &pot, &pot_min, &pot_max, 1, 0)) {
     printf("Error reading %s\n", argv[1]);
     exit(1);
  }
  printf("Range of potential values = [%.2e, %.2e]\n", pot_min, pot_max);  

  /* pick random potential values to test */
  for ( i=0 ; i<RANDOM_POINTS ; i++ ) {
    x[i] = poxmax * drand48();
    y[i] = poymax * drand48();
    printf("Testing pot[%d,%d] = %f\n", x[i], y[i], pot[x[i]+y[i]*poxmax]);
  }

  /* malloc energy array and fill with values */
  energy = (double *) malloc( sizeof(double)*poxmax*poymax );
  if (!energy) { printf("Malloc error for energy slice\n"); return 0; }
  Emax = (double *) malloc( sizeof(double)*poxmax*poymax );
  if (!Emax) { printf("Malloc error for Emax\n"); return 0; }
  for ( i=0 ; i<poxmax*poymax ; i++ ) {
    energy[i] = (pot[i] - pot_min) + 1.0;
    energy[i] = pow(energy[i], 2.42) / 1.602e-9;
    Emax[i] = sqrt(energy[i])*2.0013e-5;
  }

  /* start timing */
  wall_beg = times(&beg);
  printf("wall_beg=%u user=%u sys=%u\n",wall_beg,beg.tms_utime,beg.tms_stime);

  /* compute depositions */
  for ( k=0 ; k<iters ; k++ ) {
    for ( j=0 ; j<RANDOM_POINTS ; j++ ) {
      index = x[j] + y[j]*poxmax;
      for ( i=0 ; i<Z_STEPS ; i++ ) {

        z_value = z_min + (i*z_range) / (Z_STEPS-1);

        rho = mo_x_no * exp( -lam * ((z_value-z_base)*1e5));

        Ren = rho / lam;

        mu = pow( (Ren / 4.6e-6), e1 ) / Emax[index];

        A1 = c1*pow(mu,c2) * exp(-c3*pow(mu,c4));

        A2 = c5*pow(mu,c6) * exp(-c7*pow(mu,c8));

        answers[i] = ((energy[index] * rho) / (2.0*Ren)) * (A1 + A2);

      }
    }
  }


  /* end timing */
  wall_end = times(&end);
  printf("wall_end=%u user=%u sys=%u\n",wall_end,end.tms_utime,end.tms_stime);

  for ( i=0 ; i<Z_STEPS ; i++ )
    printf("%4d - %10.5lf at z=%8.2lf\n", i, answers[i],
            z_min+(i*z_range) / (Z_STEPS-1));

  wall_time = (wall_end - wall_beg) / (double) CLK_TCK;
  cpu_time = (end.tms_utime - beg.tms_utime) / (double) CLK_TCK;
  sys_time = (end.tms_stime - beg.tms_stime) / (double) CLK_TCK;

  printf("wall time = %.4lf seconds\n", wall_time);
  printf("cpu time = %.4lf seconds\n", cpu_time);
  printf("sys time = %.4lf seconds\n", sys_time);

  printf("%d total depositions\n", iters*RANDOM_POINTS*Z_STEPS);
  per_second = cpu_time / (iters*RANDOM_POINTS*Z_STEPS);
  printf("%.8lf      seconds per deposition\n", per_second);
  printf("%.8lf milliseconds per deposition\n", per_second * 1000);
  printf("%.8lf microseconds per deposition\n", per_second * 1000000);

  exit(0);
}
