
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "noise.h"

/* info to read the texture map */
#define TEXT_URES 512
#define TEXT_VRES 4096
static unsigned char texture[TEXT_URES*TEXT_VRES];
//static char *texture_name = "/Genetti/Hayden/05-10-31/512/snap0150.pbm";
static char *texture_base = "/Genetti/Hayden/05-10-31/512/snap%04d.pbm";
static char *output_base = "turb%04d.pbm";
static char texture_file[256];
static char output_file[256];

#define xsize 512
#define ysize 4096

int offset=4096;
double xPeriod = 0.0;
double yPeriod = 1.0;
double turbPower = 50.0;
double turbSize = 64.0;
double zValue = 0.0;
double zInc = 0.01;

static unsigned char raster[xsize*ysize];

double turbulence(double x, double y, double size)
{
  double value = 0.0;
  double initialsize = size;
  Vector p;

  while (size >= 1.0) {
    p.x = x/size; p.y = y/size; p.z = zValue;
    value += noise(p) * size;
    size /= 2.0;
  }

  return value/(initialsize*2.0);
}

void get_texture(int num)
{
  FILE *fp;
  int xres, yres, maxval, u, v;
        
  sprintf(texture_file, texture_base, num);

  if ((fp = fopen(texture_file, "r")) == 0)
    { printf("Can't open %s\n", texture_file); exit(1); }
          
  fscanf(fp, "P5\n");
  if (fscanf(fp, "%d %d\n%d\n", &xres, &yres, &maxval) != 3)
    { printf("Error reading header\n"); exit(1); }
        
  if (xres != TEXT_URES || yres != TEXT_VRES)
    { printf("Invalid size for texture map\n"); exit(1); }
          
  if (fread(texture, sizeof(unsigned char), xres*yres, fp) != xres*yres)
    { printf("Error reading %s\n", texture_file); exit(1); }

  fclose(fp);

  printf("Read texture %s (%dx%d)\n", texture_file, TEXT_URES, TEXT_VRES);
}

double texture_lookup(double i, double j)
{
  double value;

  if (i<0 || i>=xsize) return 1.0;

  if (i > xsize/2) {
    value = (i-xsize/2) / (xsize/2 - 1.0);
  } else {
    value = (xsize/2-i) / (xsize/2 - 1.0);
  }

  return value;
}

int main()
{
  int i,j;
  double xyValue;
  double sineValue;
  Vector p;
  int rindex;
  FILE *fp;
  double new_i, new_j;
  double fl_u, fl_v;
  double u_frac, v_frac;
  short l00, l01, l10, l11;
  short lx0, lx1, lxx;
  int u, v;
  int frames;

  initnoise();

  for ( frames=0 ; frames<211 ; frames++ ) {

    get_texture(frames);

    rindex = 0;
    for ( j=0 ; j<ysize ; j++ ) {

      for ( i=0 ; i<xsize ; i++,rindex++ ) {

#if 0
         // Just make a gradient along Y axis
//         xyValue = texture_lookup(i, j);

         // Try adding some turbulence before looking up
         xyValue = texture_lookup(i - turbPower/2.0
                                  + turbPower*turbulence(i*10.0,j*10.0,turbSize),
                                  j);

         if (xyValue < 0.0) raster[rindex] = 0;
         else if (xyValue > 1.0) raster[rindex] = 255;
         else raster[rindex] = (unsigned char) (xyValue * 255);
#else
         // add turbulence to i,j lookup coordinates
         new_i = i - turbPower/2.0 + turbPower*turbulence(i*10.0,j*10.0,turbSize);
         new_j = j;

         if (new_i < 0.0) { raster[i+j*xsize] = 0; continue; }
         if (new_i > xsize-1) { raster[i+j*xsize] = 0; continue; }

         /* bi-lerp the texture map */
         u = (int) new_i; u_frac = new_i - u;
         v = (int) new_j; v_frac = new_j - v;
//printf("new_i=%lf new_j=%lf u=%d v=%d u_f=%lf v_f=%lf\n",
// new_i, new_j,u, v,u_frac,v_frac);

//         raster[i+j*xsize] = texture[u+v*xsize];

           l00 = texture[u   + v*TEXT_URES];
           l10 = texture[u+1 + v*TEXT_URES];
//           l10 = texture[u   + (v+1)*TEXT_URES];
//           l11 = texture[u+1 + (v+1)*TEXT_URES];
           lx0 = l00 + (short)((l10 - l00)*u_frac);
//           lx1 = l01 + (short)((l11 - l01)*u_frac);
//           lxx = lx0 + (short)((lx1 - lx0)*v_frac);

           raster[i+j*xsize] = (unsigned char) lx0;

//         raster[i+j*xsize] = texture[new_i+new_j*xsize];
#endif

       }

    }

    sprintf(output_file, output_base, frames);
    fp = fopen(output_file, "wb");
    if (!fp) { printf("Can't open %s\n", output_file); exit(1); }
    fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n%d\n", xsize, ysize, 255);
    fwrite(raster, sizeof(unsigned char), xsize*ysize, fp);
    fclose(fp);
  }

}
