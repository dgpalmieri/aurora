#include <stdio.h>
#include "read_potential.h"

int read_potential(char *filename,
                   int *xmax,
                   int *ymax,
                   int *xymax,
                   float *time_step,
                   float **xunits,
                   float **yunits,
                   float *zpo,
                   float **pot,
                   float *min,
                   float *max,
                   int be_verbose,
                   int dump_record_markers)
{
  FILE *fp;
  char  rec1[5], rec2[5];
  int   i;

  if ((fp = fopen(filename, "r")) == 0) {
    printf("Can't open %s\n", filename);
    return 0;
  }

  /* this should be the first record marker */
  fread(&rec1, 1, 4, fp);
  if (dump_record_markers)
    printf("start of first record marker = <%d %d %d %d>\n",
           rec1[0], rec1[1], rec1[2], rec1[3]);

  /* read the dimensions of the grid */
  fread(xmax, 1, 4, fp);
  fread(ymax, 1, 4, fp);
  fread(time_step, 1, 4, fp);
  if (be_verbose)
    printf("x steps=%d y steps=%d time_step=%f\n", *xmax, *ymax, *time_step);
  *xunits = (float *) malloc (sizeof(float) * *xmax);
  if (!(*xunits)) {
    printf("Unable to malloc xunits[%d]\n", *xmax);
    return 0;
  }
  *yunits = (float *) malloc (sizeof(float) * *ymax);
  if (!(*yunits)) {
    printf("Unable to malloc yunits[%d]\n", *ymax);
    free(*xunits); return 0;
  }

  /* this should be the first record marker */
  fread(&rec2, 1, 4, fp);
  if (dump_record_markers)
    printf("end of first record marker = <%d %d %d %d>\n",
           rec2[0], rec2[1], rec2[2], rec2[3]);

  /* verify */    
  if ((rec1[0] != rec2[0]) || (rec1[1] != rec2[1]) ||
      (rec1[2] != rec2[2]) || (rec1[3] != rec2[3])) {
    printf("First record markers do not match\n");
    free(*yunits); free(*xunits); return 0;
  }

  /* this should be the second record marker */
  fread(&rec1, 1, 4, fp);
  if (dump_record_markers)
    printf("start of second record marker = <%d %d %d %d>\n",
           rec1[0], rec1[1], rec1[2], rec1[3]);

  /* read the grid coordinates */
  if (fread(*xunits, sizeof(float), *xmax, fp) != *xmax) {
    printf("Error reading X grid coordinates\n");
    free(*yunits); free(*xunits); return 0;
  }
  if (fread(*yunits, sizeof(float), *ymax, fp) != *ymax) {
    printf("Error reading Y grid coordinates\n");
    free(*yunits); free(*xunits); return 0;
  }
  if (fread(zpo, 1, 4, fp) != 4) {
    printf("Error reading Z potential\n");
    free(*yunits); free(*xunits); return 0;
  }

  if (be_verbose)
    printf("xunits=[%f,%f] yunits=[%f,%f] zpo = %f\n", (*xunits)[0],
           (*xunits)[*xmax-1], (*yunits)[0], (*yunits)[*ymax-1], *zpo);

  /* this should be the second record marker */
  fread(&rec2, 1, 4, fp);
  if (dump_record_markers)
    printf("end of second record marker = <%d %d %d %d>\n",
           rec2[0], rec2[1], rec2[2], rec2[3]);    
  if ((rec1[0] != rec2[0]) || (rec1[1] != rec2[1]) ||
      (rec1[2] != rec2[2]) || (rec1[3] != rec2[3])) {
    printf("Second record markers do not match\n");  
    free(*yunits); free(*xunits); return 0;      
  }

  /* this should be the third record marker */
  fread(&rec1, 1, 4, fp);
  if (dump_record_markers)
    printf("start of third record marker = <%d %d %d %d>\n",
           rec1[0], rec1[1], rec1[2], rec1[3]);

  *xymax = *xmax * *ymax;
  *pot = (float *) malloc (sizeof(float) * *xymax);  
  if (!(*pot)) {
    printf("Unable to malloc pot[%d elements]\n", *xymax);
    free(*yunits); free(*xunits); return 0;
  }

  if (fread(*pot, sizeof(float), *xymax, fp) != *xymax) {
    printf("Error reading potential values\n");
    free(*pot); free(*yunits); free(*xunits); return 0;
  }

  *min = *max = (*pot)[0];
  for ( i=1 ; i<*xymax ; i++ ) { 
    if ((*pot)[i] < *min) *min = (*pot)[i];
    if ((*pot)[i] > *max) *max = (*pot)[i];      
  }
  if (be_verbose) {
    float_array_overview(*pot, *xmax, *ymax);
    printf("Range = [%.2e, %.2e]\n\n", *min, *max);
  }

  /* this should be the third record marker */ 
  fread(&rec2, 1, 4, fp);
  if (dump_record_markers)
    printf("end of third record marker = <%d %d %d %d>\n", 
           rec2[0], rec2[1], rec2[2], rec2[3]);    
  if ((rec1[0] != rec2[0]) || (rec1[1] != rec2[1]) ||
      (rec1[2] != rec2[2]) || (rec1[3] != rec2[3])) {
    printf("Third record markers do not match\n");   
    free(*pot); free(*yunits); free(*xunits); return 0;      
  }

  fclose(fp);

  return 1;
}

