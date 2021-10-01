#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static unsigned char magenta_table[256][3] = {
{   0,   0,   0},
{   0,   1,   0},
{   0,   2,   0},
{   1,   3,   2},
{   1,   4,   2},
{   1,   5,   2},
{   2,   6,   4},
{   2,   7,   4},
{   2,   8,   4},
{   3,   9,   6},
{   3,  10,   6},
{   3,  11,   6},
{   4,  12,   8},
{   4,  13,   8},
{   4,  14,   8},
{   5,  15,  10},
{   5,  16,  10},
{   5,  17,  10},
{   6,  18,  12},
{   6,  19,  12},
{   6,  20,  12},
{   7,  21,  14},
{   7,  22,  14},
{   7,  23,  14},
{   8,  24,  16},
{   8,  25,  16},
{   8,  26,  16},
{   9,  27,  18},
{   9,  28,  18},
{   9,  29,  18},
{  10,  30,  20},
{  10,  31,  20},
{  10,  32,  20},
{  11,  33,  22},
{  11,  34,  22},
{  11,  35,  22},
{  12,  36,  24},
{  12,  37,  24},
{  12,  38,  24},
{  13,  39,  26},
{  13,  40,  26},
{  13,  41,  26},
{  14,  42,  28},
{  14,  43,  28},
{  14,  44,  28},
{  15,  45,  30},
{  15,  46,  30},
{  15,  47,  30},
{  16,  48,  32},
{  16,  49,  32},
{  16,  50,  32},
{  17,  51,  34},
{  17,  52,  34},
{  17,  53,  34},
{  18,  54,  36},
{  18,  55,  36},
{  18,  56,  36},
{  19,  57,  38},
{  19,  58,  38},
{  19,  59,  38},
{  20,  60,  40},
{  20,  61,  40},
{  20,  62,  40},
{  21,  63,  42},
{  21,  64,  42},
{  21,  65,  42},
{  22,  66,  44},
{  22,  67,  44},
{  22,  68,  44},
{  23,  69,  46},
{  23,  70,  46},
{  23,  71,  46},
{  24,  72,  48},
{  24,  73,  48},
{  24,  74,  48},
{  25,  75,  50},
{  25,  76,  50},
{  25,  77,  50},
{  26,  78,  52},
{  26,  79,  52},
{  26,  80,  52},
{  27,  81,  54},
{  27,  82,  54},
{  27,  83,  54},
{  28,  84,  56},
{  28,  85,  56},
{  28,  86,  56},
{  29,  87,  58},
{  29,  88,  58},
{  29,  89,  58},
{  30,  90,  60},
{  30,  91,  60},
{  30,  92,  60},
{  31,  93,  62},
{  31,  94,  62},
{  31,  95,  62},
{  32,  96,  64},
{  32,  97,  64},
{  32,  98,  64},
{  33,  99,  66},
{  33, 100,  66},
{  33, 101,  66},
{  34, 102,  68},
{  34, 103,  68},
{  34, 104,  68},
{  35, 105,  70},
{  35, 106,  70},
{  35, 107,  70},
{  36, 108,  72},
{  36, 109,  72},
{  36, 110,  72},
{  37, 111,  74},
{  37, 112,  74},
{  37, 113,  74},
{  38, 114,  76},
{  38, 115,  76},
{  38, 116,  76},
{  39, 117,  78},
{  39, 118,  78},
{  39, 119,  78},
{  40, 120,  80},
{  40, 121,  80},
{  40, 122,  80},
{  41, 123,  82},
{  41, 124,  82},
{  41, 125,  82},
{  42, 126,  84},
{  42, 127,  84},
{  42, 128,  84},
{  43, 129,  86},
{  43, 130,  86},
{  43, 131,  86},
{  44, 132,  88},
{  44, 133,  88},
{  44, 134,  88},
{  45, 135,  90},
{  45, 136,  90},
{  45, 137,  90},
{  46, 138,  92},
{  46, 139,  92},
{  46, 140,  92},
{  47, 141,  94},
{  47, 142,  94},
{  47, 143,  94},
{  48, 144,  96},
{  48, 145,  96},
{  48, 146,  96},
{  49, 147,  98},
{  49, 148,  98},
{  49, 149,  98},
{  50, 150, 100},
{  50, 151, 100},
{  50, 152, 100},
{  51, 153, 102},
{  51, 154, 102},
{  51, 155, 102},
{  52, 156, 104},
{  52, 157, 104},
{  52, 158, 104},
{  53, 159, 106},
{  53, 160, 106},
{  53, 161, 106},
{  54, 162, 108},
{  54, 163, 108},
{  54, 164, 108},
{  55, 165, 110},
{  55, 166, 110},
{  55, 167, 110},
{  56, 168, 112},
{  56, 169, 112},
{  56, 170, 112},
{  57, 171, 114},
{  57, 172, 114},
{  57, 173, 114},
{  58, 174, 116},
{  58, 175, 116},
{  58, 176, 116},
{  59, 177, 118},
{  59, 178, 118},
{  59, 179, 118},
{  60, 180, 120},
{  60, 181, 120},
{  60, 182, 120},
{  61, 183, 122},
{  61, 184, 122},
{  61, 185, 122},
{  62, 186, 124},
{  62, 187, 124},
{  62, 188, 124},
{  63, 189, 126},
{  63, 190, 126},
{  63, 191, 126},
{  64, 192, 128},
{  64, 193, 128},
{  64, 194, 128},
{  65, 195, 130},
{  65, 196, 130},
{  65, 197, 130},
{  66, 198, 132},
{  66, 199, 132},
{  66, 200, 132},
{  67, 201, 134},
{  67, 202, 134},
{  67, 203, 134},
{  68, 204, 136},
{  68, 205, 136},
{  68, 206, 136},
{  69, 207, 138},
{  69, 208, 138},
{  69, 209, 138},
{  70, 210, 140},
{  70, 211, 140},
{  70, 212, 140},
{  71, 213, 142},
{  71, 214, 142},
{  71, 215, 142},
{  72, 216, 144},
{  72, 217, 144},
{  72, 218, 144},
{  73, 219, 146},
{  73, 220, 146},
{  73, 221, 146},
{  74, 222, 148},
{  74, 223, 148},
{  74, 224, 148},
{  75, 225, 150},
{  75, 226, 150},
{  75, 227, 150},
{  76, 228, 152},
{  76, 229, 152},
{  76, 230, 152},
{  77, 231, 154},
{  77, 232, 154},
{  77, 233, 154},
{  78, 234, 156},
{  78, 235, 156},
{  78, 236, 156},
{  79, 237, 158},
{  79, 238, 158},
{  79, 239, 158},
{  80, 240, 160},
{ 241, 241, 241},
{ 241, 241, 241},
{ 242, 242, 242},
{ 242, 242, 242},
{ 243, 243, 243},
{ 243, 243, 243},
{ 244, 244, 244},
{ 244, 244, 244},
{ 245, 245, 245},
{ 245, 245, 245},
{ 246, 246, 246},
{ 246, 246, 246},
{ 247, 247, 247},
{ 247, 247, 247},
{ 248, 248, 248} };
static unsigned char green_table[256][3] = {
{   0,   0,   0},
{   0,   1,   0},
{   0,   2,   0},
{   1,   3,   2},
{   1,   4,   2},
{   1,   5,   2},
{   2,   6,   4},
{   2,   7,   4},
{   2,   8,   4},
{   3,   9,   6},
{   3,  10,   6},
{   3,  11,   6},
{   4,  12,   8},
{   4,  13,   8},
{   4,  14,   8},
{   5,  15,  10},
{   5,  16,  10},
{   5,  17,  10},
{   6,  18,  12},
{   6,  19,  12},
{   6,  20,  12},
{   7,  21,  14},
{   7,  22,  14},
{   7,  23,  14},
{   8,  24,  16},
{   8,  25,  16},
{   8,  26,  16},
{   9,  27,  18},
{   9,  28,  18},
{   9,  29,  18},
{  10,  30,  20},
{  10,  31,  20},
{  10,  32,  20},
{  11,  33,  22},
{  11,  34,  22},
{  11,  35,  22},
{  12,  36,  24},
{  12,  37,  24},
{  12,  38,  24},
{  13,  39,  26},
{  13,  40,  26},
{  13,  41,  26},
{  14,  42,  28},
{  14,  43,  28},
{  14,  44,  28},
{  15,  45,  30},
{  15,  46,  30},
{  15,  47,  30},
{  16,  48,  32},
{  16,  49,  32},
{  16,  50,  32},
{  17,  51,  34},
{  17,  52,  34},
{  17,  53,  34},
{  18,  54,  36},
{  18,  55,  36},
{  18,  56,  36},
{  19,  57,  38},
{  19,  58,  38},
{  19,  59,  38},
{  20,  60,  40},
{  20,  61,  40},
{  20,  62,  40},
{  21,  63,  42},
{  21,  64,  42},
{  21,  65,  42},
{  22,  66,  44},
{  22,  67,  44},
{  22,  68,  44},
{  23,  69,  46},
{  23,  70,  46},
{  23,  71,  46},
{  24,  72,  48},
{  24,  73,  48},
{  24,  74,  48},
{  25,  75,  50},
{  25,  76,  50},
{  25,  77,  50},
{  26,  78,  52},
{  26,  79,  52},
{  26,  80,  52},
{  27,  81,  54},
{  27,  82,  54},
{  27,  83,  54},
{  28,  84,  56},
{  28,  85,  56},
{  28,  86,  56},
{  29,  87,  58},
{  29,  88,  58},
{  29,  89,  58},
{  30,  90,  60},
{  30,  91,  60},
{  30,  92,  60},
{  31,  93,  62},
{  31,  94,  62},
{  31,  95,  62},
{  32,  96,  64},
{  32,  97,  64},
{  32,  98,  64},
{  33,  99,  66},
{  33, 100,  66},
{  33, 101,  66},
{  34, 102,  68},
{  34, 103,  68},
{  34, 104,  68},
{  35, 105,  70},
{  35, 106,  70},
{  35, 107,  70},
{  36, 108,  72},
{  36, 109,  72},
{  36, 110,  72},
{  37, 111,  74},
{  37, 112,  74},
{  37, 113,  74},
{  38, 114,  76},
{  38, 115,  76},
{  38, 116,  76},
{  39, 117,  78},
{  39, 118,  78},
{  39, 119,  78},
{  40, 120,  80},
{  40, 121,  80},
{  40, 122,  80},
{  41, 123,  82},
{  41, 124,  82},
{  41, 125,  82},
{  42, 126,  84},
{  42, 127,  84},
{  42, 128,  84},
{  43, 129,  86},
{  43, 130,  86},
{  43, 131,  86},
{  44, 132,  88},
{  44, 133,  88},
{  44, 134,  88},
{  45, 135,  90},
{  45, 136,  90},
{  45, 137,  90},
{  46, 138,  92},
{  46, 139,  92},
{  46, 140,  92},
{  47, 141,  94},
{  47, 142,  94},
{  47, 143,  94},
{  48, 144,  96},
{  48, 145,  96},
{  48, 146,  96},
{  49, 147,  98},
{  49, 148,  98},
{  49, 149,  98},
{  50, 150, 100},
{  50, 151, 100},
{  50, 152, 100},
{  51, 153, 102},
{  51, 154, 102},
{  51, 155, 102},
{  52, 156, 104},
{  52, 157, 104},
{  52, 158, 104},
{  53, 159, 106},
{  53, 160, 106},
{  53, 161, 106},
{  54, 162, 108},
{  54, 163, 108},
{  54, 164, 108},
{  55, 165, 110},
{  55, 166, 110},
{  55, 167, 110},
{  56, 168, 112},
{  56, 169, 112},
{  56, 170, 112},
{  57, 171, 114},
{  57, 172, 114},
{  57, 173, 114},
{  58, 174, 116},
{  58, 175, 116},
{  58, 176, 116},
{  59, 177, 118},
{  59, 178, 118},
{  59, 179, 118},
{  60, 180, 120},
{  60, 181, 120},
{  60, 182, 120},
{  61, 183, 122},
{  61, 184, 122},
{  61, 185, 122},
{  62, 186, 124},
{  62, 187, 124},
{  62, 188, 124},
{  63, 189, 126},
{  63, 190, 126},
{  63, 191, 126},
{  64, 192, 128},
{  64, 193, 128},
{  64, 194, 128},
{  65, 195, 130},
{  65, 196, 130},
{  65, 197, 130},
{  66, 198, 132},
{  66, 199, 132},
{  66, 200, 132},
{  67, 201, 134},
{  67, 202, 134},
{  67, 203, 134},
{  68, 204, 136},
{  68, 205, 136},
{  68, 206, 136},
{  69, 207, 138},
{  69, 208, 138},
{  69, 209, 138},
{  70, 210, 140},
{  70, 211, 140},
{  70, 212, 140},
{  71, 213, 142},
{  71, 214, 142},
{  71, 215, 142},
{  72, 216, 144},
{  72, 217, 144},
{  72, 218, 144},
{  73, 219, 146},
{  73, 220, 146},
{  73, 221, 146},
{  74, 222, 148},
{  74, 223, 148},
{  74, 224, 148},
{  75, 225, 150},
{  75, 226, 150},
{  75, 227, 150},
{  76, 228, 152},
{  76, 229, 152},
{  76, 230, 152},
{  77, 231, 154},
{  77, 232, 154},
{  77, 233, 154},
{  78, 234, 156},
{  78, 235, 156},
{  78, 236, 156},
{  79, 237, 158},
{  79, 238, 158},
{  79, 239, 158},
{  80, 240, 160},
{ 241, 241, 241},
{ 241, 241, 241},
{ 242, 242, 242},
{ 242, 242, 242},
{ 243, 243, 243},
{ 243, 243, 243},
{ 244, 244, 244},
{ 244, 244, 244},
{ 245, 245, 245},
{ 245, 245, 245},
{ 246, 246, 246},
{ 246, 246, 246},
{ 247, 247, 247},
{ 247, 247, 247},
{ 248, 248, 248} };

main(int argc, char *argv[])
{
  FILE *fp;
  int xres, yres;
  int be_verbose=0;
  int i,x,y,ptr;
  float  min, max;
  float *values;
  float map_min, map_range;
  float alpha;
  unsigned char r, g, b;
  int tmp;
  int index;
  int tmpc;

  if (argc != 3 && argc != 5) {
    printf("Usage: %s filename image.ppm [min_map max_map]\n", argv[0]);
    exit(1);
  }

  if ((fp=fopen(argv[1], "rb")) == NULL) {
    printf("Can't open %s\n", argv[1]);
    exit(1);
  }

  if (be_verbose)
    printf("Reading file %s\n", argv[1]);

  if (fscanf(fp, "#FRAW\n%d %d\n", &xres, &yres) != 2) {
    printf("Error reading header\n");
    exit(1);
  }

  if (xres<0 || yres<0) {
    printf("Invalid values in header\n");
    printf("Read xres=%d yres=%d\n", xres, yres);
    exit(1);
  }

  if (be_verbose)
    printf("Resolution = %dx%d\n", xres, yres);

  values = (float *) malloc(xres*yres*sizeof(float));
  if (!values) {
    printf("Can't malloc array to hold values\n");
    exit(1);
  }

  tmp = fread(values, sizeof(float), xres*yres, fp);
  if (tmp != xres*yres) {
/*if (fread(values, sizeof(float), xres*yres, fp) != xres*yres) {*/
    printf("Error reading data\n");
    free(values);
    exit(1);
  }

  if (be_verbose)
    printf("%d bytes (data) read.\n", xres*yres*sizeof(float));

  fclose(fp);

  if (argc == 5) {
    /* use supplied min/max values */
    min = atof(argv[3]);
    max = atof(argv[4]);
    if (min >= max) {
       printf("Invalid range [%lf, %lf]\n", min, max);
       exit(1);
    }
  } else {
    /* find max value */
    min = max = fabs(values[0]) - 1.0;
    for ( i=1 ; i<xres*yres ; i++ ) {
      if ((fabs(values[i])-1.0) > max) max = fabs(values[i])-1.0;
      if ((fabs(values[i])-1.0) < min) min = fabs(values[i])-1.0;
    }
  }
  printf("%s: Range of values for mapping [%lf, %lf]\n", argv[1], min, max);
  map_range = max - min;

  /* write out a ppm file */
  if ((fp = fopen(argv[2], "w")) == 0) {
    printf("Can't open %s\n",argv[2]);
    exit(1);
  }

  fprintf(fp, "P6\n");
  fprintf(fp, "%d %d\n",xres,yres);
  fprintf(fp, "255\n");
  /* flip in y as PPM have 0,0 at upper right */
  for ( y=yres-1 ; y>=0 ; y-- ) {
    for ( x=0 ; x<xres ; x++ ) {
      i = y*xres + x;
      if ((fabs(values[i])-1.0) <= min) alpha = 0.0;
      else if ((fabs(values[i])-1.0) >= max) alpha = 1.0;
      else alpha = (fabs(values[i])-1.0-min) / map_range;
      index = (int) (alpha * 255);
      r = green_table[index][0];
      g = green_table[index][1];
/*
      if (values[i] < 0.0) b = 255;
      else                 b = green_table[index][2];
*/
      if (values[i] < 0.0) {
      tmpc = green_table[index][2]  + 40;
      if (tmpc > 255) b = 255; else b= tmpc;
      } else {
        b = green_table[index][2];
      }
      putc(r,fp);
      putc(g,fp);
      putc(b,fp);
    }
  }

  fflush(fp);
  fclose(fp);
 
  free(values);
}
