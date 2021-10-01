#include "min_max.h"

double find_min(double *values,
                int num)
{
  int i;
  double min;
  
  min = values[0];
  for ( i=1 ; i<num ; i++ )
    if (values[i]<min) min=values[i];
  return min;
}

double find_max(double *values,
                int num)
{
  int i;
  double max;
  
  max = values[0];
  for ( i=1 ; i<num ; i++ )
    if (values[i]>max) max=values[i];
  return max;
}

void find_min_max(double *values,
                  int num,
                  double *min,
                  double *max)
{
  int i;
  double mn, mx;
  
  mn = mx = values[0];
  for ( i=1 ; i<num ; i++ ) {
    if (values[i]<mn) mn=values[i];
    if (values[i]>mx) mx=values[i];
  }

  *min = mn;
  *max = mx;
}

