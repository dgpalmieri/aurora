#include <stdio.h>
#include "pot_2_ppm_file.h"

void pot_2_ppm_file(char *filename,
                    int xres,
                    int yres,
                    float *values,
                    float min,
                    float range)
{
  FILE *img;
  int i;
  unsigned char g;
  int xyres = xres*yres;

  if ((img = fopen(filename, "w")) == 0) {
    printf("Can't open %s\n",filename);
    exit(1);
  }
  fprintf(img, "P6\n");
  fprintf(img, "%d %d\n",xres,yres);
  fprintf(img, "255\n");
  for ( i=0 ; i<xyres ; i++ ) {
    if (values[i] > min)
      g = (unsigned char) (255*((values[i]-min)/range));
    else
      g = 0;
    putc(0,img);
    putc(g,img);
    putc(0,img);
  }
  fflush( img );
  fclose( img );

}

