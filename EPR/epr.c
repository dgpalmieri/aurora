#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "render.h"
#include "rays.h"
#include "raster.h"
#include "err_msg.h"
#include "view.h"
#include "timing.h"

#define WHITESPACE " :,\t\n"

/* HACK */
#define NUM_POINTS 103

static char *asterisks = "********************************************************************************\n";

/***** read in from the configuration file and passed to render.c *****/

/* bounding box (world coordinates) of aurora at any time */
double VolumeXmin, VolumeXmax;
double VolumeYmin, VolumeYmax;
double VolumeZmin, VolumeZmax;

/* resolution of the grid to sample the curtain to */
int super_x = 10;
int super_y = 10;
int super_z = 10;
int sub_x   = 16;
int sub_y   = 16;
int sub_z   = 16;

/* properties of the curtain */
double CurtainWidth           =   25.0;
double OrigCurtainWidth       =   25.0;

/* texture path to apply to the bottom of the curtain */
/* path to directory of the form [director]/%03d/snap%04d.pbm */
char TextureTemp[1025];
char *CurtainTexturePath = NULL; /* pointer set if a path is read in */

/* how many curtains are there? */
int num_curtains = 0;


/***** generated in epr.c and passed to render.c *****/

/* what frame number to use */
double CurtainFrameNum = 0.0;

/* sampling rate for each curtain */
double seg_step_size[MAX_CURTAINS];

/* low and high resolution needed for the texture maps */
int LowerRes[MAX_CURTAINS];
int HigherRes[MAX_CURTAINS];

/* how much the offset the texture maps should have (simulate flow along curtain) */
static double baseUVflow[MAX_CURTAINS];
double UVflow[MAX_CURTAINS];

/***** read in from configuration and used to calculate data for render.c *****/

/* range of curtains to render (code will interpolate if necessary) */
/* these map to camera file as cam(i)=frame_begin + i*inc */
static double CurtainFrameBegin = 0.0;
static double CurtainFrameInc   = 1.0;

/* sampling rate for a spline/curtain */
static double CurtainStepSize = 1.0;

/* range of multi-resolution texture maps available */
static int CurtainResLowest  = 0;
static int CurtainResHighest = 0;


/***** read in from camera file and passed to render.c *****/

/* holds raster/window information */
Screen s;


int num_frames[MAX_CURTAINS];
int frame_no[MAX_CURTAINS][MAX_FRAMES];
int num_points[MAX_CURTAINS][MAX_FRAMES];
double spX[MAX_CURTAINS][MAX_FRAMES][MAX_POINTS];
double spY[MAX_CURTAINS][MAX_FRAMES][MAX_POINTS];
double spZ[MAX_CURTAINS][MAX_FRAMES][MAX_POINTS];
float visible[MAX_CURTAINS][MAX_FRAMES];
float texture[MAX_CURTAINS][MAX_FRAMES];
float brightness[MAX_CURTAINS][MAX_POINTS];
float uv[MAX_CURTAINS][MAX_POINTS];

      
/* if supplied, this will hold the inclination angles based on lat/long */
float **inclination = NULL;

/* timing information */
uLong totalStartTime = 0;
uLong totalEndTime = 0;
double totalTime = 0;

void mat_mult(double res[4][4], double m1[4][4], double m2[4][4])
{
  int i, j, k;

  for ( i=0 ; i<4 ; i++ )
    for ( j=0 ; j<4 ; j++ ) {  
      res[i][j]=0.0;
      for ( k=0 ; k<4 ; k++ )
        res[i][j] += m1[i][k]*m2[k][j];
    }
}

int read_curtain(
FILE *fp
)
{
  int fr=0;  /* current frame number */
  char inputline[256];
  char *tokens;

  /* I am ASSuming that all files start with the same frame */
  /* number and contain the same number of frames           */

  while (fr < MAX_FRAMES) {
        
    if (fgets(inputline, 255, fp) == NULL) break;
      
    tokens = strtok(inputline, WHITESPACE);
    if (tokens == NULL) continue; /* skip empty lines */
    
    if (strncasecmp("frame", tokens, 5) == 0) { 
      /* found a new frame */
      fr ++;
      num_points[num_curtains][fr] = 0;
      strtok(NULL, WHITESPACE);
      frame_no[num_curtains][fr-1] = atoi(strtok(NULL, WHITESPACE));
    } else if (strncasecmp("visible", tokens, 7) == 0) {
      continue;
    } else if (strncasecmp("invisible", tokens, 9) == 0) {
      continue;
    } else if (strncasecmp("no", tokens, 2) == 0) {
      continue;
    } else {
      /* read the control point */
      if (num_points[num_curtains][fr-1] < MAX_POINTS) {
        spX[num_curtains][fr-1][num_points[num_curtains][fr-1]] =
                        atof(tokens);
        spY[num_curtains][fr-1][num_points[num_curtains][fr-1]] =
                        atof(strtok(NULL, WHITESPACE));
        spZ[num_curtains][fr-1][num_points[num_curtains][fr-1]] =
                        atof(strtok(NULL, WHITESPACE));
        num_points[num_curtains][fr-1] ++;
      } else {
        printf("Number of points exceeds MAX_POINTS - change in render.h\n");
      }
    }

#if 0
printf("fr=%d num_points[0][0] = %d\n", fr, num_points[0][0]);
#endif

  } 

  if (fr == MAX_FRAMES) {
    printf("Number of frames exceeds MAX_FRAMES - change in render.h\n");
    printf("Ignoring the rest of the frames\n");
  }

  num_frames[num_curtains] = fr;

  return 1;
}

int read_visible(
FILE *fp
)
{
  int i,c,tmp;

  /* this is visibilty for the last curtain read in */
  c = num_curtains-1;

  /* read in visibility number for each frame */
  for ( i=0 ; i<num_frames[c] ; i++ ) {
    if ( fscanf(fp, "%d %f\n", &tmp, &(visible[c][i])) != 2 ) {
      printf("Error reading visibility - format\n");
      fclose(fp);
      exit(1);
    }
    if ( i != tmp) {
      printf("Error reading visibility - frames\n");
      fclose(fp);
      exit(1);
    }

#if 0
printf("curtain=%d frame=%d visibility=%f\n", c, i, visible[c][i]);
#endif

  }

  return 1;

}

int read_brightness(
FILE *fp,
int c
)
{
  int i,tmp;

  /* read in brightness number for each control point */
  for ( i=0 ; i<NUM_POINTS ; i++ ) {
    if ( fscanf(fp, "%d %f\n", &tmp, &(brightness[c][i])) != 2 ) {
       printf("Error reading brightness - format\n");
       fclose(fp);
       exit(1);
    }
    if ( i != tmp) {
      printf("Error reading brightness - frames\n");
      fclose(fp);
      exit(1);
    }

#if 0
printf("curtain=%d frame=%d brightness=%f\n", c, i, brightness[c][i]);
#endif

  }

  return 1;

}

int read_uv(
FILE *fp,
int c
)
{
  int i,tmp;

  /* read in uv mapping value for each control point */
  for ( i=0 ; i<NUM_POINTS ; i++ ) {
    if ( fscanf(fp, "%d %f\n", &tmp, &(uv[c][i])) != 2 ) {
      printf("Error reading UV mapping - format\n");
      fclose(fp);
      exit(1);
    }
    if ( i != tmp) {
      printf("Error reading UV mapping - frames\n");
      fclose(fp);
      exit(1);
    }

#if 0
printf("curtain=%d frame=%d uv=%f\n", c, i, uv[c][i]);
#endif

  }

  return 1;

}

int read_texture(
FILE *fp
)
{
  int i,c,tmp;

  /* this is texture for the last curtain read in */
  c = num_curtains-1;

  /* read in texture number for each frame */
  for ( i=0 ; i<num_frames[c] ; i++ ) {
    if ( fscanf(fp, "%d %f\n", &tmp, &(texture[c][i])) != 2 ) {
       printf("Error reading texture - format\n");
       fclose(fp);
       exit(1);
    }
    if ( i != tmp) {
      printf("Error reading texture - frames\n");
      fclose(fp);
      exit(1);
    }

#if 0
printf("curtain=%d frame=%d texture=%f\n", c, i, texture[c][i]);
#endif

  }

  return 1;

}

int read_inclination(
FILE *fp
)
{
  int i,j;      /* loop counters */
  int lat, lon; /* lat/long read in */
  float inc;    /* inclination read in */

  /* malloc array of pointers to the angles for each latitude */
  inclination = (float **) malloc(sizeof(float *) * NUM_LATS);
  if (!inclination) {
    printf("Can't malloc array of pointers\n");
    return 0;
  }

  /* each latitude holds floats for long = [-180, 179] */
  for ( i=0 ; i<NUM_LATS ; i++ ) {
    inclination[i] = (float *) malloc(sizeof(float)*360);
    if (!inclination[i]) {
      printf("Can't malloc latitude %d\n", 90-i);
      return 0;
    }
  }

  /* set to an out-of-bounds value */
  for ( i=0 ; i<NUM_LATS ; i++ )
    for ( j=0 ; j<360 ; j++ )
      inclination[i][j] = -100.0;

  /* read the inclinations from the file */
  while (fscanf(fp, "%d %d %f\n", &lat, &lon, &inc) == 3) {
    inclination[90-lat][lon+180] = inc;
  }
  fclose(fp);

  /* verify we have a value for each inclination */
  for ( i=0 ; i<NUM_LATS ; i++ ) {
    for ( j=0 ; j<360 ; j++ ) {
      if (inclination[i][j] < -90.0) {
        printf("Didn't get an inclination for [%d][%d]\n", 90-i, j-180);
        return 0;
      }
    }
  }

  return 1;
}

int main(int argc, char *argv[])
{
  FILE   *parms;            /* pointer to parameter file */
  char    inputline[256];   /* holds line read in from file */
  char   *tokens;           /* use to break inputline into tokens */
  int     lineno;           /* line number being read in file */
  int     it1, it2, it3;    /* temp holders for ints read in */
  FILE   *camera_matrix;    /* file pointer to camera matrix file */
  int     next_arg;         /* used to step thru arg list */
  int     cam_start = 0;    /* first camera (in camera file) to render */
  int     cam_end   = 0;    /* last camera (in camera file) to render */
  int     cam_inc   = 1;    /* increment between first and last camera */
  int     xres;             /* input width of image(s) to render */
  int     yres;             /* input height of image(s) to render */
  int     i,j,k;            /* loop counters */
  int     cam_read;         /* # of cameras read so far from camera file */
  char    raw_temp[1025];   /* template used to build a filename */
  char    raw_file[1025];   /* filename of output image */
  char    tex_file[1025];   /* filename of curtain texture file */
  double  left, right;      /* left & right vertical clip planes */
  double  bottom, top;      /* bottom & top horizontal clip plane */
  double  near;             /* near clipping plane (positive) */
  double  far;              /* far clipping plane (positive) */
  double  xorg, yorg, zorg;
  double  xrot, yrot, zrot;
  double  xtr, ytr, ztr;
  double  cs, sn;
  MATRIX  m1, m2, m3, m4;
  MATRIX  t1, t2;
  MATRIX  cam;              /* camera transformation matrix */
  FILE   *fp;               /* file pointer for curtain or inclination files */
  Ray R;
  int    csize;
  double target_area;
  double area;
  double low_area, high_area;
  int lower, higher;
  char *base_name;

  if (argc != 4 && argc != 7 && argc != 8 && argc != 11)
    ERROR_MSG("USAGE: epr parmfile camerafile [-range s e i] imagebase [-res w h]")

  printf("Reading Parameter File ...\n\n");
  if ((parms = fopen(argv[1], "r")) == 0) ERROR_BAD_OPEN(argv[1])

  /* read and process parm file one line at a time */
  lineno=0;
  while (fgets(inputline, 255, parms) != NULL) {

    lineno++;

    if (inputline[0] == '#') continue;  /* ignore comments */

    /* find the first token in this input line */
    tokens = strtok(inputline, WHITESPACE);
    if (tokens == NULL) continue;  /* skip this empty line */

    if (strncasecmp("CurtainSplineFile",tokens,17) == 0) {

      sprintf(raw_temp, "%s", strtok(NULL, WHITESPACE));
      fp = fopen(raw_temp, "r");
      if (!fp) {
        printf("Can't open %s -- IGNORING\n", raw_temp);
      } else {
        if (read_curtain(fp)) {
          printf("  Curtain %d <%s>\n", num_curtains, raw_temp);
          seg_step_size[num_curtains] = CurtainStepSize;
          num_curtains ++;
        } else {
          printf("Error reading %s -- IGNORING\n", raw_temp);
        }
        fclose(fp);
      }

    } else if (strncasecmp("CurtainVisibleFile",tokens,18) == 0) {

      if (num_curtains == 0) {
        printf("Must define a curtain before visibility --- IGNORING\n");
      } else {
        sprintf(raw_temp, "%s", strtok(NULL, WHITESPACE));
        fp = fopen(raw_temp, "r");
        if (!fp) {
          printf("Can't open %s -- IGNORING\n", raw_temp);
        } else {
          if (read_visible(fp)) {
            printf("  Curtain Visibility %d <%s>\n", num_curtains-1, raw_temp);
          } else {
            printf("Error reading %s -- IGNORING\n", raw_temp);
          }
          fclose(fp);
        }
      }

    } else if (strncasecmp("CurtainTextureFile",tokens,18) == 0) {

      if (num_curtains == 0) {
        printf("Must define a curtain before texture --- IGNORING\n");
      } else {
        sprintf(raw_temp, "%s", strtok(NULL, WHITESPACE));
        fp = fopen(raw_temp, "r");
        if (!fp) {
          printf("Can't open %s -- IGNORING\n", raw_temp);
        } else {
          if (read_texture(fp)) {
            printf("  Curtain Texture  %d <%s>\n", num_curtains-1, raw_temp);
          } else {
            printf("Error reading %s -- IGNORING\n", raw_temp);
          }
          fclose(fp);
        }
      }

    } else if (strncasecmp("CurtainBrightnessFile",tokens,21) == 0) {

      if (num_curtains == 0) {
        printf("Must define a curtain before brightness --- IGNORING\n");
      } else {
        sprintf(raw_temp, "%s", strtok(NULL, WHITESPACE));
        fp = fopen(raw_temp, "r");
        if (!fp) {
          printf("Can't open %s -- IGNORING\n", raw_temp);
        } else {
          if (read_brightness(fp, num_curtains-1)) {
            printf("  Curtain Brightness %d <%s>\n", num_curtains-1, raw_temp);
          } else {
            printf("Error reading %s -- IGNORING\n", raw_temp);
          }
          fclose(fp);
        }
      }

    } else if (strncasecmp("CurtainUVFile",tokens,13) == 0) {

      if (num_curtains == 0) {
        printf("Must define a curtain before uv --- IGNORING\n");
      } else {
        sprintf(raw_temp, "%s", strtok(NULL, WHITESPACE));
        fp = fopen(raw_temp, "r");
        if (!fp) {
          printf("Can't open %s -- IGNORING\n", raw_temp);
        } else {
          if (read_uv(fp, num_curtains-1)) {
            printf("  Curtain UV %d <%s>\n", num_curtains-1, raw_temp);
          } else {
            printf("Error reading %s -- IGNORING\n", raw_temp);
          }
          fclose(fp);
        }
      }

    } else if (strncasecmp("CurtainFrameBegin",tokens,17) == 0) {

      CurtainFrameBegin = atof(strtok(NULL, WHITESPACE));
      printf("  CurtainFrameBegin: %.2lf\n", CurtainFrameBegin);

    } else if (strncasecmp("CurtainFrameInc",tokens,15) == 0) {

      CurtainFrameInc = atof(strtok(NULL, WHITESPACE));
      printf("  CurtainFrameInc: %.2lf\n", CurtainFrameInc);

    } else if (strncasecmp("CurtainWidth",tokens,12) == 0) {

      OrigCurtainWidth = CurtainWidth = atof(strtok(NULL, WHITESPACE));
      printf("  CurtainWidth: %lf\n", CurtainWidth);

    } else if (strncasecmp("CurtainStepSize",tokens,15) == 0) {

      CurtainStepSize = atof(strtok(NULL, WHITESPACE));
      printf("  CurtainStepSize: %lf\n", CurtainStepSize);

    } else if (strncasecmp("CurtainResLowest",tokens,16) == 0) {

      CurtainResLowest = atoi(strtok(NULL, WHITESPACE));
      printf("  CurtainResLowest: %d\n", CurtainResLowest);

    } else if (strncasecmp("CurtainResHighest",tokens,17) == 0) {

      CurtainResHighest = atoi(strtok(NULL, WHITESPACE));
      printf("  CurtainResHighest: %d\n", CurtainResHighest);

    } else if (strncasecmp("CurtainTexturePath",tokens,18) == 0) {

      sprintf(TextureTemp, "%s", strtok(NULL, WHITESPACE));
      printf("  CurtainTexturePath: %s\n", TextureTemp);
      CurtainTexturePath = TextureTemp;

    } else if (strncasecmp("VolumeXmin",tokens,10) == 0) {

      VolumeXmin = atof(strtok(NULL, WHITESPACE));
      printf("  VolumeXmin: %lf\n", VolumeXmin);

    } else if (strncasecmp("VolumeXmax",tokens,10) == 0) {

      VolumeXmax = atof(strtok(NULL, WHITESPACE));
      printf("  VolumeXmax: %lf\n", VolumeXmax);

    } else if (strncasecmp("VolumeYmin",tokens,10) == 0) {

      VolumeYmin = atof(strtok(NULL, WHITESPACE));
      printf("  VolumeYmin: %lf\n", VolumeYmin);

    } else if (strncasecmp("VolumeYmax",tokens,10) == 0) {

      VolumeYmax = atof(strtok(NULL, WHITESPACE));
      printf("  VolumeYmax: %lf\n", VolumeYmax);

    } else if (strncasecmp("VolumeZmin",tokens,10) == 0) {

      VolumeZmin = atof(strtok(NULL, WHITESPACE));
      printf("  VolumeZmin: %lf\n", VolumeZmin);

    } else if (strncasecmp("VolumeZmax",tokens,10) == 0) {

      VolumeZmax = atof(strtok(NULL, WHITESPACE));
      printf("  VolumeZmax: %lf\n", VolumeZmax);

    } else if (strncasecmp("InclinationFile",tokens,15) == 0) {

      sprintf(raw_temp, "%s", strtok(NULL, WHITESPACE));
      fp = fopen(raw_temp, "r");
      if (!fp) {
        printf("Can't open %s -- IGNORING\n", raw_temp);
      } else {
        if (read_inclination(fp)) {
          printf("  InclinationFile: <%s>\n", raw_temp);
        } else {
          printf("Error reading %s -- IGNORING\n", raw_temp);
        }
      }

    } else if (strncasecmp("SubGridSize",tokens,11) == 0) {

      it1 = atoi(strtok(NULL, WHITESPACE));
      it2 = atoi(strtok(NULL, WHITESPACE));
      tokens = strtok(NULL, WHITESPACE);
      if (tokens) {
        it3 = atoi(tokens);
        if ((it1 > 1) && (it2 > 1) && (it3 > 1)) {
          sub_x = it1; sub_y = it2; sub_z = it3;
          printf("  SubGridSize: %dx%dx%d\n", sub_x, sub_y, sub_z);
        } else {
          printf("  SubGridSize must be greater than 1\n");
        }
      } else {
        printf("  Invalid SubGridSize on line %d\n", lineno);
      }

    } else if (strncasecmp("SuperGridSize",tokens,13) == 0) {

      it1 = atoi(strtok(NULL, WHITESPACE));
      it2 = atoi(strtok(NULL, WHITESPACE));
      tokens = strtok(NULL, WHITESPACE);
      if (tokens) {
        it3 = atoi(tokens);
        super_x = it1; super_y = it2; super_z = it3;
        printf("  SuperGridSize: %dx%dx%d\n", super_x, super_y, super_z);
      } else {
        printf("  Invalid SuperGridSize on line %d\n", lineno);
      }

    } else {

      printf("***** Unrecognized token on line %d\n", lineno);

    }

  }

  fclose(parms);

  printf("\nEnd of Parameter File\n\n");

  /* calculate size of texture maps to use */
  /* use just X for now (mapped to U) */
  target_area = (VolumeXmax - VolumeXmin) / (sub_x*super_x);

  if (CurtainResLowest == CurtainResHighest) {
    lower = higher = CurtainResLowest;
  } else if (target_area > (CurtainWidth/CurtainResLowest)) {
    /* use lowest res texture map available */
    lower = higher = CurtainResLowest;
  } else if (target_area < (CurtainWidth/CurtainResHighest)) {
    /* use highest res texture map available */
    lower = higher = CurtainResHighest;
  } else {
    lower = CurtainResLowest;
    higher = CurtainResLowest*2;
    while (higher<CurtainResHighest) {
      area = CurtainWidth / higher;

#if 0
printf("Area(%d) = %lf\n", higher, area);
#endif

      if (area < target_area) break;
      lower *= 2;
      higher *= 2;
    }
  }
  low_area = CurtainWidth / lower;
  high_area = CurtainWidth / higher;
  printf("Texture target=%lf Low(%d)=%lf High(%d)=%lf\n",
         target_area, lower, low_area, higher, high_area);

  for ( k=0 ; k<num_curtains ; k++ ) {
    LowerRes[k] = lower;
    HigherRes[k] = higher;
    baseUVflow[k] = 0.0005;
  }

  printf("Grid size is %d x %d x %d\n", sub_x*super_x,
          sub_y*super_y, sub_z*super_z);
  printf("Bound is X:(%.1lf,%.1lf) Y:(%.1lf,%.1lf) Z:(%.1lf,%.1lf)\n",
          VolumeXmin, VolumeXmax, VolumeYmin,
          VolumeYmax, VolumeZmin, VolumeZmax);
  printf("Grid Resolution (km) X:%.4lf Y:%.4lf Z:%.4lf\n",
          (VolumeXmax-VolumeXmin)/(sub_x*super_x),
          (VolumeYmax-VolumeYmin)/(sub_y*super_y),
          (VolumeZmax-VolumeZmin)/(sub_z*super_z));

  /* set defaults for Screen data structure */
  s.xmin = -0.045;
  s.xmax =  0.045;
  s.ymin = -0.045;
  s.ymax =  0.045;
  s.near =  0.1;
  s.far  =  5.0;
  s.xres =  400;
  s.yres =  400;

  if ((camera_matrix = fopen(argv[2], "r")) == 0) ERROR_BAD_OPEN(argv[2])

  /* arg 3 could be -range */
  next_arg = 3;
  if (strcasecmp("-range",argv[next_arg]) == 0) {
    next_arg ++;
    cam_start = atoi(argv[next_arg++]);
    cam_end = atoi(argv[next_arg++]);
    cam_inc = atoi(argv[next_arg++]);
    if (cam_start<0 || cam_inc<1 || cam_start>cam_end)
      ERROR_MSG("Invalid camera range\n")
  }

  base_name = argv[next_arg++];

  /* grab image resolution (if given) */
  if (next_arg < argc) {
    if (strcasecmp("-res",argv[next_arg])) ERROR_MSG(" -res expected\n")
    next_arg++;
    xres = atoi(argv[next_arg++]);
    yres = atoi(argv[next_arg++]);
    if (xres < 1 || yres < 1) ERROR_MSG("Bad image resolution\n")
    s.xres = xres;
    s.yres = yres;
  }

  cam_read = -1;
  for ( cam_read=-1,i=cam_start ; i<=cam_end ; i+=cam_inc) {
    totalStartTime = get_time(0);
    /* find the next camera */
    lineno=0;
    while (cam_read < i) {

      CurtainFrameNum = CurtainFrameBegin + i*CurtainFrameInc;
      for ( k=0 ; k<num_curtains ; k++ ) 
        UVflow[k] = baseUVflow[k]*i;

      if (fgets(inputline, 255, camera_matrix) == NULL)
        ERROR_MSG("End of file reached in camera file")
      lineno++;
      if (inputline[0] == '#') continue;  /* ignore comments */

      /* find first token from input line */
      tokens = strtok(inputline, WHITESPACE);
      if (tokens == NULL) continue;  /* skip this empty line */

      if (strncasecmp("CameraClipping",tokens,14) == 0) {

         /* get the clipping planes */
         near = atof(strtok(NULL, WHITESPACE));
         far = atof(strtok(NULL, WHITESPACE));
         if (near<=0.0 || far < near) {
           printf("  Ignoring invalid clipping plane on line %d\n",lineno);
           continue;
         }
         s.near = near; s.far = far;

      } else if (strncasecmp("CameraWindow",tokens,12) == 0) {

        /* get the window values */
        left   = atof(strtok(NULL, WHITESPACE));
        right  = atof(strtok(NULL, WHITESPACE));
        top    = atof(strtok(NULL, WHITESPACE));
        bottom = atof(strtok(NULL, WHITESPACE));
        if (right <= left || top <= bottom) {
          printf("  Ignoring invalid window on line %d\n",lineno);
          continue;
        }
        s.xmin = left; s.xmax = right; s.ymin = bottom; s.ymax = top;

      } else if (strncasecmp("CameraMatrix",tokens,12) == 0) {

        cam[0][0] = atof(strtok(NULL, WHITESPACE));
        cam[0][1] = atof(strtok(NULL, WHITESPACE));
        cam[0][2] = atof(strtok(NULL, WHITESPACE));
        cam[0][3] = atof(strtok(NULL, WHITESPACE));
        cam[1][0] = atof(strtok(NULL, WHITESPACE));
        cam[1][1] = atof(strtok(NULL, WHITESPACE));
        cam[1][2] = atof(strtok(NULL, WHITESPACE));
        cam[1][3] = atof(strtok(NULL, WHITESPACE));
        cam[2][0] = atof(strtok(NULL, WHITESPACE));
        cam[2][1] = atof(strtok(NULL, WHITESPACE));
        cam[2][2] = atof(strtok(NULL, WHITESPACE));
        cam[2][3] = atof(strtok(NULL, WHITESPACE));
        cam[3][0] = atof(strtok(NULL, WHITESPACE));
        cam[3][1] = atof(strtok(NULL, WHITESPACE));
        cam[3][2] = atof(strtok(NULL, WHITESPACE));
        cam[3][3] = atof(strtok(NULL, WHITESPACE));
        cam_read ++;
        if (cam_read == i) break;  /* found it */

      } else if (strncasecmp("HaydenCamera",tokens,12) == 0) {

        xtr  = atof(strtok(NULL, WHITESPACE));
        ytr  = atof(strtok(NULL, WHITESPACE));
        ztr  = atof(strtok(NULL, WHITESPACE));
        xrot = atof(strtok(NULL, WHITESPACE));
        yrot = atof(strtok(NULL, WHITESPACE));
        zrot = atof(strtok(NULL, WHITESPACE));

        /* convert from 1000km to km */
        xorg=xtr*1000.0; yorg=ytr*1000.0; zorg=ztr*1000.0;

        m1[0][0]=1.0; m1[0][1]=0.0; m1[0][2]=0.0; m1[0][3]=0.0;
        m1[1][0]=0.0; m1[1][1]=1.0; m1[1][2]=0.0; m1[1][3]=0.0;
        m1[2][0]=0.0; m1[2][1]=0.0; m1[2][2]=1.0; m1[2][3]=0.0;
        m1[3][0]=xorg; m1[3][1]=yorg; m1[3][2]=zorg; m1[3][3]=1.0;

        cs = cos(zrot*3.14159/180.0);
        sn = sin(zrot*3.14159/180.0);
        m2[0][0]=cs;  m2[0][1]= sn; m2[0][2]=0.0; m2[0][3]=0.0;
        m2[1][0]=-sn; m2[1][1]=cs;  m2[1][2]=0.0; m2[1][3]=0.0;
        m2[2][0]=0.0; m2[2][1]=0.0; m2[2][2]=1.0; m2[2][3]=0.0;
        m2[3][0]=0.0; m2[3][1]=0.0; m2[3][2]=0.0; m2[3][3]=1.0;

        cs = cos(yrot*3.14159/180.0);
        sn = sin(yrot*3.14159/180.0);
        m3[0][0]=cs;  m3[0][1]=0.0; m3[0][2]=-sn; m3[0][3]=0.0;
        m3[1][0]=0.0; m3[1][1]=1.0; m3[1][2]=0.0; m3[1][3]=0.0;
        m3[2][0]= sn; m3[2][1]=0.0; m3[2][2]=cs;  m3[2][3]=0.0;
        m3[3][0]=0.0; m3[3][1]=0.0; m3[3][2]=0.0; m3[3][3]=1.0;

        cs = cos(xrot*3.14159/180.0);
        sn = sin(xrot*3.14159/180.0);
        m4[0][0]=1.0; m4[0][1]=0.0; m4[0][2]=0.0; m4[0][3]=0.0;
        m4[1][0]=0.0; m4[1][1]=cs;  m4[1][2]= sn; m4[1][3]=0.0;
        m4[2][0]=0.0; m4[2][1]=-sn; m4[2][2]=cs;  m4[2][3]=0.0;
        m4[3][0]=0.0; m4[3][1]=0.0; m4[3][2]=0.0; m4[3][3]=1.0;

        mat_mult(t1, m4, m3);
        mat_mult(t2, t1, m2);
        mat_mult(cam, t2, m1);

        cam_read ++;
        if (cam_read == i) break;  /* found it */

      }

    }
    set_view_matrix((double *)cam);

#if DO_GREEN
    printf("\n"); printf("%s", asterisks);
    sprintf(raw_file, "%s_%05d.green", base_name, i);
    printf("  Rendering %s at %d x %d ...\n", raw_file, s.xres, s.yres);
    printf("  Curtain Frame=%lf Camera=%i\n", CurtainFrameNum, cam_read);
    printf("%s", asterisks);
    render_image(raw_file, i, GREEN_CHANNEL);
#endif

#if DO_MAGENTA
    printf("\n"); printf("%s", asterisks);
    sprintf(raw_file, "%s_%05d.magenta", base_name, i);
    printf("  Rendering %s at %d x %d ...\n", raw_file, s.xres, s.yres);
    printf("  Curtain Frame=%lf Camera=%i\n", CurtainFrameNum, cam_read);
    printf("%s", asterisks);
    render_image(raw_file, i, MAGENTA_CHANNEL);
#endif

#ifdef DO_RED
    printf("\n"); printf("%s", asterisks);
    sprintf(raw_file, "%s_%05d.red", base_name, i);
    printf("  Rendering %s at %d x %d ...\n", raw_file, s.xres, s.yres);
    printf("  Curtain Frame=%lf Camera=%i\n", CurtainFrameNum, cam_read);
    printf("%s", asterisks);
    render_image(raw_file, i, RED_CHANNEL);
#endif

  }

  fclose(camera_matrix);

}

/*---------------------------------------------------------------------------*/

