#include <math.h>
#include "pot_2_energy.h"
#include "min_max.h"

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

const double z_base = 100.0;

double *pot_2_energy(
float *pot,
int    xmax,
int    ymax,
float  pot_min,
double z_min,
double z_max,
int    z_steps,
int be_verbose
)
{
  double *energy;
  double *rho;
  double *Ren;
  double *Emax;
  int     i;
  int     slice;
  double  tmp1, tmp2;
  double  min_energy, max_energy;
  double  z_range;
  double  z_value;
  double  mu,A1,A2;
  double *slice_ptr;
  double final_range;
  const double lam = (mo*g)/(kb*to);
  const int xymax = xmax*ymax;

  energy = (double *) malloc( sizeof(double)*xymax*z_steps );
  if (!energy) {
    printf("Malloc error for energy\n");
    return 0;
  }

  rho = (double *) malloc( sizeof(double)*z_steps );
  if (!rho) {
    printf("Malloc error for rho\n");
    free(energy); return 0;
  }

  Ren = (double *) malloc( sizeof(double)*z_steps );
  if (!Ren) {
    printf("Malloc error for Ren\n");
    free(rho); free(energy); return 0;
  }

  Emax = (double *) malloc( sizeof(double)*xymax );
  if (!Emax) {
    printf("Malloc error for Emax\n");
    free(Ren); free(rho); free(energy); return 0;
  }

  /* scale potential data (taken from moddep Matlab code) */
  for ( i=0 ; i<xymax ; i++ ) {
    energy[i] = (pot[i] - pot_min) + 1.0;
    energy[i] = pow(energy[i],2.42);
    energy[i] = energy[i]/1.602e-9;
  }
 
  if (be_verbose) {
    printf("Energy after scale from moddep (slice 0):\n");
    double_array_overview(energy, xmax, ymax);
    printf("Range = [%e, %e]\n",
           find_min(energy, xymax), find_max(energy, xymax));
  }


  z_range = z_max-z_min;
  for ( i=0 ; i<z_steps ; i++ ) {
    z_value = z_min + (i*z_range) / (z_steps-1);
    rho[i] = (mo*no) * exp( -lam * ((z_value-z_base)*1e5));
    Ren[i] = rho[i] / lam;
  }
  for ( i=0 ; i<xymax ; i++ ) {
    Emax[i] = sqrt(energy[i])*2.0013e-5;
  }

  if (be_verbose) {
    printf("lam = %e\n",lam);
    printf("rho[0]=%e ... rho[%d]=%e\n",rho[0],z_steps-1,rho[z_steps-1]);
    printf("Ren[0]=%e ... Ren[%d]=%e\n",Ren[0],z_steps-1,Ren[z_steps-1]);
    printf("Emax[%d] (values same for all slices):\n", xymax);
    double_array_overview(Emax, xmax, ymax);
  }
  
  /* calculate energy volume */
  /* NOTE: energy[0] has the values needed for all slices */
  /* calculate z_steps-1 ... 1 before 0 so we don't overwrite them */
  
  for ( slice=z_steps-1 ; slice>=0 ; slice-- ) {
    for ( i=0 ; i<xymax ; i++ ) {
      mu = pow( (Ren[slice] / 4.6e-6), (1.0/1.65) ) / Emax[i];
      A1 = c1*pow(mu,c2) * exp(-c3*pow(mu,c4));
      A2 = c5*pow(mu,c6) * exp(-c7*pow(mu,c8));
      energy[i+slice*xymax] = ((energy[i] * rho[slice]) /
                              (2.0*Ren[slice])) * (A1 + A2);
    }
  }

  if (be_verbose) {
    min_energy = max_energy = energy[0];
    for ( slice=0 ; slice<z_steps ; slice++ ) {
      printf("energy (slice %d):\n", slice);
      double_array_overview(energy+slice*xymax,xmax,ymax);
      tmp1 = find_min(energy+slice*xymax,xymax);
      if (tmp1 < min_energy) min_energy=tmp1;
      tmp2 = find_max(energy+slice*xymax,xymax);
      if (tmp2 > max_energy) max_energy=tmp2;
      printf("Range of slice %d=[%e, %e]\n", slice, tmp1, tmp2);
    }
    printf("Range of all slices [%e, %e]\n", min_energy, max_energy);
  }
  
  free(Emax); free(Ren); free(rho);
  
  return energy;
}

