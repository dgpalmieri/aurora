#include "double_array_overview.h"

void double_array_overview(double *values, int xsize, int ysize)
{
  printf("[%4d,%4d]=%.2e ... [%4d,%4d]=%.2e ... [%4d,%4d]=%.2e\n",
         0,      0,values[0],
         xsize/2,0,values[xsize/2],
         xsize-1,0,values[xsize-1]);
  printf("[%4d,%4d]=%.2e ... [%4d,%4d]=%.2e ... [%4d,%4d]=%.2e\n",
         0,      ysize/4,values[        (ysize/4)*xsize],
         xsize/2,ysize/4,values[xsize/2+(ysize/4)*xsize],
         xsize-1,ysize/4,values[xsize-1+(ysize/4)*xsize]);
  printf("[%4d,%4d]=%.2e ... [%4d,%4d]=%.2e ... [%4d,%4d]=%.2e\n",
         0,      ysize/2,values[        (ysize/2)*xsize],
         xsize/2,ysize/2,values[xsize/2+(ysize/2)*xsize],
         xsize-1,ysize/2,values[xsize-1+(ysize/2)*xsize]);
  printf("[%4d,%4d]=%.2e ... [%4d,%4d]=%.2e ... [%4d,%4d]=%.2e\n",
         0,      3*ysize/4,values[        (3*ysize/4)*xsize],
         xsize/2,3*ysize/4,values[xsize/2+(3*ysize/4)*xsize],
         xsize-1,3*ysize/4,values[xsize-1+(3*ysize/4)*xsize]);
  printf("[%4d,%4d]=%.2e ... [%4d,%4d]=%.2e ... [%4d,%4d]=%.2e\n",
         0,      ysize-1,values[        (ysize-1)*xsize],
         xsize/2,ysize-1,values[xsize/2+(ysize-1)*xsize],
         xsize-1,ysize-1,values[xsize-1+(ysize-1)*xsize]);
}

