#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "noise.h"

#define WHITESPACE " :,\t\n"

#define _DUMP_IT
#define EARTH_TEXTURE
#define MAX_FRAMES 1000
#define MAX_POINTS 110
#define MAX_CURTAINS 6
static int frames;
static int points;
static int f_num;          /* current frame number to display */
static int animate=0;      /* increment frame number after display() */
static int draw_earth=0;
static int view_no=2;
static int dump_frame=0;
static int test_vol=0;
static int add_detail=0;
static int spline=1;       /* draw only the splines */
static int linear=0;       /* draw only the lines between control points */
static int start_frame_dump;
static int draw_bound=0;
#define MAX_VIEWS 4
static char *filenames[MAX_CURTAINS] = {
  "../Data/Y26x8/curtainMain.data",
  "../Data/Y26x8/secondaryCurtain.data",
  "../Data/Y26x8/secondaryCurtain-1.data",
  "../Data/Y26x8/tertiaryCurtain.data",
  "../Data/Y26x8/tertiaryCurtain-1.data",
  "../Data/Y26x8/clouds.data" }; 
static char *filevisible[MAX_CURTAINS] = {
  "../Data/curtainMain.visible",
  "../Data/secondaryCurtain.visible",
  "../Data/secondaryCurtain-1.visible",
  "../Data/tertiaryCurtain.visible",
  "../Data/tertiaryCurtain.visible",
  "../Data/clouds.visible" }; 

static float cur_col[MAX_CURTAINS][3] = {
{0.0, 1.0, 0.0},
{1.0, 0.0, 0.0},
{1.0, 0.0, 0.0},
{1.0, 0.0, 0.0},
{1.0, 0.0, 1.0},
{1.0, 0.0, 1.0},
{1.0, 0.0, 1.0},
{1.0, 0.0, 1.0},
{1.0, 1.0, 0.0} };

static float colors[6][3] = {{0.5,0.0,0.0},{0.0,0.5,0.0},{0.0,0.0,0.5},
                             {0.5,0.5,0.0},{0.5,0.0,0.5},{0.0,0.5,0.5}};
static double ext = 1.0342826;
#define MAX_SUBS 64
static int n_subs[MAX_CURTAINS] = { 36, 8, 12, 28 };
static double u1[MAX_CURTAINS][MAX_SUBS];
static double u2[MAX_CURTAINS][MAX_SUBS];
static double u3[MAX_CURTAINS][MAX_SUBS];
static double w1[MAX_CURTAINS][MAX_SUBS];
static double w2[MAX_CURTAINS][MAX_SUBS];
static double w3[MAX_CURTAINS][MAX_SUBS];
static double w4[MAX_CURTAINS][MAX_SUBS];

#define EARTH_WIDTH 512
#define EARTH_HEIGHT 256
#define EARTH_SAMPLES (EARTH_WIDTH*EARTH_HEIGHT)
static unsigned char Red_image[EARTH_SAMPLES];
static unsigned char Green_image[EARTH_SAMPLES];
static unsigned char Blue_image[EARTH_SAMPLES];
static unsigned char earth_tex_map[EARTH_HEIGHT][EARTH_WIDTH][4];
static char *earth_texture = "earth.sgi";
static GLuint earth;
GLuint sphere;

/* holds number of control points for each frame */
int frame_no[MAX_CURTAINS][MAX_FRAMES];
int num_points[MAX_CURTAINS][MAX_FRAMES];

/* holds coordinates of control points */
double X[MAX_CURTAINS][MAX_FRAMES][MAX_POINTS];
double Y[MAX_CURTAINS][MAX_FRAMES][MAX_POINTS];
double Z[MAX_CURTAINS][MAX_FRAMES][MAX_POINTS];
float visible[MAX_CURTAINS][MAX_FRAMES];

double brightness[MAX_CURTAINS][MAX_POINTS];

unsigned char image[1920*1200*3];
static int width=800;
static int height=800;
#if 0
static int width=1180;
static int height=1180;
static int width=1850;
static int height=1150;
static int width=1100;
static int height=1100;
static int width=1100;
static int height=700;
#endif

typedef struct { double x, y, z; } XYZ;
XYZ iP[100];
XYZ iV[100];
XYZ iN[100];
XYZ iC[100];

static GLuint texture2D[1];

#define DET_PTS 11
static double det_pts[DET_PTS+1][2] = {
{  0.00,  0.00 }, 
{  0.00,  0.00 }, 
{  0.70,  0.20 }, 
{  1.40,  1.40 }, 
{  2.10,  1.40 }, 
{  0.00,  0.00 }, 
{ -1.90, -1.60 }, 
{ -1.00, -1.30 }, 
{ -0.70, -0.30 }, 
{  0.00,  0.00 }, 
{  0.00,  0.00 }, 
{  0.00,  0.00 } };

XYZ subtract(XYZ P1, XYZ P2)
{
  XYZ P;

  P.x = P1.x - P2.x;
  P.y = P1.y - P2.y;
  P.z = P1.z - P2.z;

  return P;
}

XYZ normalize(XYZ P)
{
  double scale;

  scale = 1.0 / sqrt(P.x*P.x + P.y*P.y + P.z*P.z);
  P.x *= scale; P.y *= scale; P.z *= scale;

  return P;
}

XYZ cross_product(XYZ P1, XYZ P2)
{
  XYZ C;

  C.x = P1.y*P2.z - P1.z*P2.y;
  C.y = P1.z*P2.x - P1.x*P2.z;
  C.z = P1.x*P2.y - P1.y*P2.x;

  return C;
}

XYZ spline_point(int spline_number, int frame_num, double u)
{
  XYZ P;
  int i;
  double u1, u2, u3;
  double w1, w2, w3, w4;

  i = (int) u;
  if (i < 1 || i>=num_points[spline_number][frame_num]-2)
    { P.x = P.y = P.z = 0.0; return P; }

  u1 = u - i;
  u2 = u1 * u1;
  u3 = u2 * u1;

  w1 = -0.5*u3 +     u2 - 0.5*u1;
  w2 =  1.5*u3 - 2.5*u2 + 1.0;
  w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
  w4 =  0.5*u3 - 0.5*u2;

  P.x = w1 * X[spline_number][frame_num][i-1]
      + w2 * X[spline_number][frame_num][i]
      + w3 * X[spline_number][frame_num][i+1]
      + w4 * X[spline_number][frame_num][i+2];
  P.y = w1 * Y[spline_number][frame_num][i-1]
      + w2 * Y[spline_number][frame_num][i]
      + w3 * Y[spline_number][frame_num][i+1]
      + w4 * Y[spline_number][frame_num][i+2];
  P.z = w1 * Z[spline_number][frame_num][i-1]
      + w2 * Z[spline_number][frame_num][i]
      + w3 * Z[spline_number][frame_num][i+1]
      + w4 * Z[spline_number][frame_num][i+2] ;

  return P;
}

void draw_3Dgrid(
int spline_number,
int frame_num,
double u1,
double u2,
int i_steps,
int j_steps,
int k_steps,
double width,
double height
)
{
  int i,j,k;
  double width_start, width_step, half_width;
  double H, C, C0, C1;
  int t_s;

  #define MAX_STEPS 100
  if (i_steps >= MAX_STEPS) return;

  /* calculate points along the curve inside grid */
  for ( i=0 ; i<=i_steps ; i++ ) {
    iP[i] = spline_point(0, frame_num, u1 + ((u2-u1) * i) / (i_steps-1));
  }

  for ( i=0 ; i<i_steps ; i++ ) {
    iV[i] = subtract(iP[i+1], iP[i]);
    iV[i] = normalize(iV[i]);
    iN[i].x = iP[i].x; iN[i].y = iP[i].y; iN[i].z = iP[i].z;
    iN[i] = normalize(iN[i]);
    iC[i] = cross_product(iV[i], iN[i]);
#if 0
if (add_detail) {
    printf("iP[%d] = (%.4lf, %.4lf, %.4lf)\n", i, iP[i].x, iP[i].y, iP[i].z);
    printf("iV[%d] = (%.4lf, %.4lf, %.4lf)\n", i, iV[i].x, iV[i].y, iV[i].z);
    printf("iN[%d] = (%.4lf, %.4lf, %.4lf)\n", i, iN[i].x, iN[i].y, iN[i].z);
    printf("iC[%d] = (%.4lf, %.4lf, %.4lf)\n", i, iC[i].x, iC[i].y, iC[i].z);
}
#endif
  }

  if (add_detail) {

  double du;
  int    di;
  double du1, du2, du3;
  double dw1, dw2, dw3, dw4;
  double delta_u, delta_v;
  XYZ iD[MAX_STEPS];

  for ( i=0 ; i<i_steps ; i++ ) {

    du = 1.0 + ((8.0 * i) / (i_steps-1));

    /* now solve this spline at u=du */
    di = (int) du;
    du1 = du - di; du2 = du1 * du1; du3 = du2 * du1;
    dw1 = -0.5*du3 +     du2 - 0.5*du1;
    dw2 =  1.5*du3 - 2.5*du2 + 1.0;
    dw3 = -1.5*du3 + 2.0*du2 + 0.5*du1;
    dw4 =  0.5*du3 - 0.5*du2;
    delta_u = (dw1*det_pts[di-1][0] + dw2*det_pts[di][0]
            + dw3*det_pts[di+1][0] + dw4*det_pts[di+2][0]) * 5.0;
    delta_v = (dw1*det_pts[di-1][1] + dw2*det_pts[di][1]
            + dw3*det_pts[di+1][1] + dw4*det_pts[di+2][1]) * 5.0;

    iD[i].x = iP[i].x + iV[i].x * delta_u + iC[i].x * delta_v;
    iD[i].y = iP[i].y + iV[i].y * delta_u + iC[i].y * delta_v;
    iD[i].z = iP[i].z + iV[i].z * delta_u + iC[i].z * delta_v;

  }

  for ( j=0 ; j<i_steps ; j++ ) {
    iV[j] = subtract(iD[j+1], iD[j]);
    iV[j] = normalize(iV[j]);
    iN[j].x = iD[j].x; iN[j].y = iD[j].y; iN[j].z = iD[j].z;
    iN[j] = normalize(iN[j]);
    iC[j] = cross_product(iV[j], iN[j]);
    iP[j] = iD[j];
#if 0
    printf(" iP[%d] = (%.4lf, %.4lf, %.4lf)\n", j, iP[j].x, iP[j].y, iP[j].z);
    printf(" iV[%d] = (%.4lf, %.4lf, %.4lf)\n", j, iV[j].x, iV[j].y, iV[j].z);
    printf(" iN[%d] = (%.4lf, %.4lf, %.4lf)\n", j, iN[j].x, iN[j].y, iN[j].z);
    printf(" iC[%d] = (%.4lf, %.4lf, %.4lf)\n", j, iC[j].x, iC[j].y, iC[j].z);
#endif
  }

  }

  width_start = - width * 0.5;
  width_step = width / (j_steps-1);

    /* draw a quadmesh at each elevation */
    glLineWidth(1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for ( k=0 ; k<k_steps ; k++ ) {
      H = (height*k) / (k_steps-1);
      for ( j=0 ; j<(j_steps-1) ; j++ ) {
        C0 = width_start + width_step*j;
        C1 = C0 + width_step;
        glBegin(GL_QUAD_STRIP);
          for ( i=0 ; i<i_steps ; i++ ) {
            glTexCoord2f(i/(double)(i_steps-1),j/(double)(j_steps-1));
            glVertex3f(iP[i].x - C0*iC[i].x + H*iN[i].x,
                       iP[i].y - C0*iC[i].y + H*iN[i].y,
                       iP[i].z - C0*iC[i].z + H*iN[i].z);
            glTexCoord2f(i/(double)(i_steps-1),(j+1)/(double)(j_steps-1));
            glVertex3f(iP[i].x - C1*iC[i].x + H*iN[i].x,
                       iP[i].y - C1*iC[i].y + H*iN[i].y,
                       iP[i].z - C1*iC[i].z + H*iN[i].z);
          } /* end for each strip */
        glEnd();
      } /* end for each width */
    } /* end for each elevation */
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

#if 0
    /* draw grid lines to the differnet elevations */
    width_start = - width * 0.5;
    width_step = width / (j_steps-1);
    for ( j=0 ; j<j_steps ; j++ ) {
      C = width_start + width_step*j;
      glBegin(GL_LINES);
        for ( i=0 ; i<i_steps ; i++ ) {
          glVertex3f(iP[i].x + C*iC[i].x,
                     iP[i].y + C*iC[i].y,
                     iP[i].z + C*iC[i].z);
          glVertex3f(iP[i].x + height*iN[i].x + C*iC[i].x,
                     iP[i].y + height*iN[i].y + C*iC[i].y,
                     iP[i].z + height*iN[i].z + C*iC[i].z);
        } 
      glEnd();
    }       
#endif

}

/*  From Paul Bourke, modified to put into a display list
   Create a sphere centered at c, with radius r, and precision n
   Draw a point for zero radius spheres
*/
void CreateSphere(XYZ c,double r,int n)
{
   int i,j;
   double theta1,theta2,theta3;
   XYZ e,p;

   sphere = glGenLists(1);

   if (r < 0) r = -r;
   if (n < 0) n = -n;
   if (n < 4 || r <= 0) {
      glNewList(sphere, GL_COMPILE);
       glBegin(GL_POINTS);
        glVertex3f(c.x,c.y,c.z);
       glEnd();
      glEndList();
     return;
   }

   glNewList(sphere, GL_COMPILE);
   for (j=0;j<n/2;j++) {
      theta1 = j * M_PI*2 / n - M_PI_2;
      theta2 = (j + 1) * M_PI*2 / n - M_PI_2;

      glBegin(GL_QUAD_STRIP);
      for (i=0;i<=n;i++) {
         theta3 = i * M_PI*2 / n;

         e.x = cos(theta2) * cos(theta3);
         e.y = sin(theta2);
         e.z = cos(theta2) * sin(theta3);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
         glVertex3f(p.x,p.y,p.z);

         e.x = cos(theta1) * cos(theta3);
         e.y = sin(theta1);
         e.z = cos(theta1) * sin(theta3);
         p.x = c.x + r * e.x;
         p.y = c.y + r * e.y;
         p.z = c.z + r * e.z;

         glNormal3f(e.x,e.y,e.z);
         glTexCoord2f(i/(double)n,2*j/(double)n);
         glVertex3f(p.x,p.y,p.z);
      }
      glEnd();
   }
   glEndList();
}

void init(void)
{    
  FILE *fp;
  char inputline[256];
  char *tokens;
  int frame;
  int i,j,c,s,t,tmp;
  XYZ center;
  int x, y;

  /* must call this to use the noise function later */
  initnoise();

  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);

  /* read in each of the curtain files */
  for ( c=0 ; c<MAX_CURTAINS ; c++ ) {

    fp = fopen(filenames[c], "r");
    if (!fp) {
      printf("Can't open %s\n", filenames[c]);
      exit(1);
    }
    printf("Reading %s ...\n", filenames[c]);

    /* I am ASSuming that all files start with the same frame */
    /* number and contain the same number of frames           */

    frame = 0;
    while (frame <= MAX_FRAMES) {

      if (fgets(inputline, 255, fp) == NULL) {
        frames = frame;
        printf("End of file reached - %d time steps read\n", frames);
        break;
      }

      tokens = strtok(inputline, WHITESPACE);
      if (tokens == NULL) continue; /* skip empty lines */

      if (strncasecmp("frame", tokens, 5) == 0) {
        /* found a new frame */
        frame ++;
        num_points[c][frame] = 0;
        strtok(NULL, WHITESPACE);
        frame_no[c][frame-1] = atoi(strtok(NULL, WHITESPACE));
      } else if (strncasecmp("visible", tokens, 7) == 0) {
        continue;
      } else if (strncasecmp("invisible", tokens, 9) == 0) {
        continue;
      } else if (strncasecmp("no", tokens, 2) == 0) {
        continue;
      } else {
        /* read the control point */
        num_points[c][frame-1] ++;
        if (num_points[c][frame-1] > points) points = num_points[c][frame-1];
        X[c][frame-1][num_points[c][frame-1]-1]=atof(tokens);
        Y[c][frame-1][num_points[c][frame-1]-1]=atof(strtok(NULL, WHITESPACE));
        Z[c][frame-1][num_points[c][frame-1]-1]=atof(strtok(NULL, WHITESPACE));
      }

    }

    fclose(fp);

    fp = fopen(filevisible[c], "r");
    if (!fp) {
      printf("Can't open %s\n", filevisible[c]);
      exit(1);
    }
    printf("Reading %s ...\n", filevisible[c]);

    /* read in visibility number for each frame */
    for ( i=0 ; i<frames ; i++ ) {
      fscanf(fp, "%d %f\n", &tmp, &(visible[c][i]));
      if ( i != tmp) { printf("Error reading visibility\n"); exit(1); }
/*    printf("curtain=%d frame=%d visibility=%f\n", c, i, visible[c][i]);*/
    }
    fclose(fp);

  }

{
  int i;
  for ( c=0 ; c<103 ; c++ ) {
    if (c < 20) brightness[0][c]=0.0;
    else if (c < 40) brightness[0][c] = (c-19) / 20.0;
    else if (c < 60) brightness[0][c] = 1.0;
    else if (c < 80) brightness[0][c] = 1.0 - ((c-59)/20.0);
    else brightness[0][c]=0.0;
  }
  for ( i=1 ; i<MAX_CURTAINS ; i++ ) {
    for ( c=0 ; c<103 ; c++ ) {
      if (c < 2) brightness[i][c]=0.0;
      else if (c < 12) brightness[i][c] = (c-1) / 10.0;
      else if (c < 90) brightness[i][c] = 1.0;
      else if (c < 100) brightness[i][c] = 1.0 - ((c-89)/10.0);
      else brightness[i][c]=0.0;
    }
  }
}

  /* read in the earth texture map */
  if ((fp=fopen(earth_texture,"rb")) == NULL)
    { printf("Can't open %s\n", earth_texture); exit(1); }
  if (fread(Red_image, 1, 512, fp) != 512)
    { printf("Error reading header\n"); exit(1); }
  if (fread(Red_image, 1, EARTH_SAMPLES, fp) != EARTH_SAMPLES)
    { printf("Error reading red channel\n"); exit(1); }
  if (fread(Green_image, 1, EARTH_SAMPLES, fp) != EARTH_SAMPLES)
    { printf("Error reading green channel\n"); exit(1); }
  if (fread(Blue_image, 1, EARTH_SAMPLES, fp) != EARTH_SAMPLES)
    { printf("Error reading blue channel\n"); exit(1); }
  fclose(fp);

  /* build the texture map */
  for ( s=0 ; s<EARTH_WIDTH ; s++ ) {
    for ( t=0 ; t<EARTH_HEIGHT ; t++ ) {
      earth_tex_map[t][EARTH_WIDTH-s][0] = Red_image[s+t*EARTH_WIDTH]>>2;
      earth_tex_map[t][EARTH_WIDTH-s][1] = Green_image[s+t*EARTH_WIDTH]>>2;
      earth_tex_map[t][EARTH_WIDTH-s][2] = Blue_image[s+t*EARTH_WIDTH]>>2;
      earth_tex_map[t][EARTH_WIDTH-s][3] = 255;
    }
  }

  /* load texture into OpenGL */
#if 0
  glGenTextures(1+NUM_POTS, texture2D);
#endif
  glGenTextures(1, texture2D);
  glBindTexture(GL_TEXTURE_2D, texture2D[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, EARTH_WIDTH, EARTH_HEIGHT, 
                0, GL_RGBA, GL_UNSIGNED_BYTE, earth_tex_map);

  center.x = center.y = center.z = 0.0;
  CreateSphere(center, 6378.1, 40);

#if 0
  for ( i=0 ; i<NUM_POTS ; i++ ) {

    sprintf(inputline, "/Genetti/Otto/04-09-10/pot%d", START_POT_NUM+i);
    read_potential(inputline);
    /* read in a potential file */
    #if 0
    if (i==0) read_potential("pot45");
    else      read_potential("pot46");
    #endif

    /* build the texture map */
#define BRIGHTNESS 75
    for ( s=0 ; s<POT_WIDTH ; s++ ) {
      x = (s*(pot_xmax-1))/(POT_WIDTH-1);
      for ( t=0 ; t<POT_HEIGHT ; t++ ) {
        y = (t*(pot_ymax-1))/(POT_HEIGHT-1);
        /* range of energy from all slices is [-8.35, 15.3] */
        if (pots[y+x*pot_xmax] <= 0.0) {
          pot_tex_map[t][s][0] = 0;
          pot_tex_map[t][s][1] = 0;  /* clamp to 0.0 */
          pot_tex_map[t][s][2] = 0;
        } else if (pots[y+x*pot_xmax] >= 9.0) {  /* clamp to 9.0 */
          pot_tex_map[t][s][0] = 0.1*BRIGHTNESS;
          pot_tex_map[t][s][1] = 0.8*BRIGHTNESS;
          pot_tex_map[t][s][2] = 0.4*BRIGHTNESS;
        } else {
          pot_tex_map[t][s][0] = (pots[y+x*pot_xmax]/9.0)*0.1*BRIGHTNESS;
          pot_tex_map[t][s][1] = (pots[y+x*pot_xmax]/9.0)*0.8*BRIGHTNESS;
          pot_tex_map[t][s][2] = (pots[y+x*pot_xmax]/9.0)*0.4*BRIGHTNESS;
        }
        pot_tex_map[t][s][3] = 255;
      }
    }

    /* load texture into OpenGL */
    glBindTexture(GL_TEXTURE_2D, texture2D[1+i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, POT_WIDTH, POT_HEIGHT, 
                  0, GL_RGBA, GL_UNSIGNED_BYTE, pot_tex_map);

    free(pots);
  }
#endif

  f_num=0;

  /* calculate U parameters for spline interpolation */
  for ( i=0 ; i<MAX_CURTAINS ; i++ ) {
    for ( j=0 ; j<n_subs[i] ; j++ ) {
      u1[i][j] = (double) j / n_subs[i];          /* calc u */
      u2[i][j] = u1[i][j]*u1[i][j];              /* calc u^2 */
      u3[i][j] = u2[i][j]*u1[i][j];              /* calc u^3 */
      w1[i][j] = -0.5*u3[i][j] +     u2[i][j] - 0.5*u1[i][j];
      w2[i][j] =  1.5*u3[i][j] - 2.5*u2[i][j] + 1.0;
      w3[i][j] = -1.5*u3[i][j] + 2.0*u2[i][j] + 0.5*u1[i][j];
      w4[i][j] =  0.5*u3[i][j] - 0.5*u2[i][j];
    }
  }

#if 0
  for ( i=0 ; i<MAX_CURTAINS ; i++ ) {
    for ( j=0 ; j<n_subs[i] ; j++ ) {
      printf("i=%d\n", j);
      printf("  u=%.4lf u^2=%.4lf u^3=%.4lf\n", u1[i][j], u2[i][j], u3[i][j]);
      printf("  w1=%.4lf w2=%.4lf w3=%.4lf w4=%.4lf\n",
              w1[i][j], w2[i][j], w3[i][j], w4[i][j]);
    }
  }
#endif

}

void keyboard(unsigned char key, int x, int y)
{ 
   switch (key) {
      case 27:  exit(0);
                break;
      case 'A':
      case 'a': if (animate) animate=0;
                else         animate=1;
                break;
      case 'B':
      case 'b': if (draw_bound) draw_bound=0;
                else            draw_bound=1;
                break;
      case 'D':
      case 'd': if (add_detail) add_detail=0;
                else            add_detail=1;
                break;
      case 'E':
      case 'e': if (draw_earth) draw_earth=0;
                else            draw_earth=1;
                break;
      case 'L':
      case 'l': if (linear) linear=0;
                else        linear=1;
                break;
      case 'N':
      case 'n': if (f_num+1 < frames)
                  f_num ++;
                break;
      case 'P':
      case 'p': if (f_num > 0)
                  f_num --;
                break;
      case 'R':
      case 'r': f_num = 0;
                break;
      case 'S':
      case 's': if (spline) spline=0;
                else        spline=1;
                break;
      case 'T':
      case 't': if (test_vol) test_vol=0;
                else          test_vol=1;
                break;
      case 'V':
      case 'v': view_no ++;
                if (view_no >= MAX_VIEWS) view_no = 0;
                break;
      case 'W':
      case 'w': if (dump_frame) dump_frame=0;
                else            dump_frame=1;
                break;
      default:  break;
   }
}   

void reshape(int w, int h)
{
   width = w; height = h;
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void draw_curtains()
{
  int i, j, c, frame;
  double col;
  double x_i, y_i, z_i;
  Vector cPt; Vector hPt; double val;
  double noise_ext;
  double last_u, next_u;
  Vector junk;

  if (view_no == 0) { /* Space viewpoint */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (GLfloat) width/(GLfloat) height, 10000.0, 25000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10643.1, 16319.42, 10643.1, 0, 5676.32, 0, 0, 1, 0); 
    glRotatef(-40.0, 0.0, 1.0, 0.0);
    start_frame_dump = 1000;
  } else if (view_no == 1) { /* Shuttle view of curtains */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (GLfloat) width/(GLfloat) height, 4000.0, 12000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(6385.86, 6385.86, 6385.86, 0, 5676.32, 0, 0, 1, 0);
    glRotatef(-40.0, 0.0, 1.0, 0.0);
    start_frame_dump = 2000;
  } else if (view_no == 2) { /* approximate Fairbanks viewpoint */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (test_vol)
      gluPerspective(140, (GLfloat) width/(GLfloat) height, 10.0, 1000.0);
    else
      gluPerspective(155, (GLfloat) width/(GLfloat) height, 10.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (test_vol)
      gluLookAt(2638.07, 5810.35, 230.88, 2638.07, 20190.8, 461.77, -1, 0, 0);
    else
      gluLookAt(2638.07, 5810.35, 230.88, 5276.14, 14190.8, 461.77, -1, 0, 0);
    start_frame_dump = 3000;
  } else { /* from above north pole */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (GLfloat) width/(GLfloat) height, 10000.0, 25000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 17738.5, 0, 0, 0, 0, 0, 0, -1);
    glRotatef(-80.0, 0.0, 1.0, 0.0);
    start_frame_dump = 4000;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* make the Z-buffer writable for drawing opaque objects */
  glDepthMask(GL_TRUE);

  if (draw_earth) {
      glPushMatrix();
      glRotatef(-10.0, 0.0, 1.0, 0.0);
      glEnable(GL_TEXTURE_2D);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glBindTexture(GL_TEXTURE_2D, texture2D[0]);
      glCallList(sphere);
      glDisable(GL_TEXTURE_2D); 
      glPopMatrix();
  }

  /* place a red square on Fairbanks */
  glPointSize(5.0);
  glColor3f(1.0,0.0,0.0);
  glPushMatrix();
  glBegin(GL_POINTS);
    glVertex3f(2638.07, 5810.35, 230.88);
  glEnd();
  glPopMatrix();

  /* make the depth buffer read only for aurora */
  glDepthMask(GL_FALSE);

  /* anything drawn now should be added to the frame buffer
     (if it isn't behind the earth) */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  if (test_vol) {

    /* the length of each segment is [0.1, 3.1] */
    c=0; i=1;
    junk.x = junk.y = 0.0; junk.z = i;
    last_u = 2.0;
    next_u = last_u + 0.1 + 2.0*noise(junk);
    junk.x = last_u;
    junk.y = next_u;
    junk.z = i;

    while (last_u < 98.0) {
      glColor3f(colors[i%6][0],colors[i%6][1],colors[i%6][2]);
/*    draw_3Dgrid(0,f_num,last_u,next_u,20,9,35,100.0,200.0);*/
/*    draw_3Dgrid(0,f_num,last_u,next_u,80,5,2,100.0,200.0);*/
      draw_3Dgrid(0,f_num,last_u,next_u,80,5,3,2.0,200.0);
      last_u = next_u;
      next_u = last_u + 0.05 + 1.0*noise(junk);
      junk.x = last_u;
      junk.y = next_u;
      junk.z = i;
      i++;
    }

#if 0
    /* 103 control points on curtain #0 */
    c=0;
    for ( i=1; i<(num_points[c][f_num]-3) ; i++ ) {
      glColor3f(colors[i%6][0],colors[i%6][1],colors[i%6][2]);
      draw_3Dgrid(0,f_num,(double)i,i+1.0,10,5,50,100.0,200.0);
    }
#endif

  } else if (linear) {

    glLineWidth(1.0);
    for ( c=0 ; c<MAX_CURTAINS ; c++ ) {
      if (num_points[c][f_num] > 0) {
        glBegin(GL_LINES);
        for ( i=0 ; i<num_points[c][f_num]-1 ; i++ ) {
          glColor3f(colors[i%6][0],colors[i%6][1],colors[i%6][2]);
          glVertex3f(X[c][f_num][i], Y[c][f_num][i], Z[c][f_num][i]);
          glVertex3f(X[c][f_num][i+1], Y[c][f_num][i+1], Z[c][f_num][i+1]);
        }
        glEnd();
      }
    }

  } else if (spline) {

printf("frame = %d\n", f_num);
    glLineWidth(1.0);
    for ( c=0 ; c<MAX_CURTAINS ; c++ ) {
      if (num_points[c][f_num] > 0) {

        glBegin(GL_LINE_STRIP);
          for ( i=1; i<(num_points[c][f_num]-2) ; i++ ) {
            glColor3f(cur_col[c][0]*brightness[c][i]*visible[c][f_num],
                      cur_col[c][1]*brightness[c][i]*visible[c][f_num],
                      cur_col[c][2]*brightness[c][i]*visible[c][f_num]);
/*          glColor3f(cur_col[c][0],cur_col[c][1],cur_col[c][2]);*/
            glVertex3f(X[c][f_num][i], Y[c][f_num][i], Z[c][f_num][i]);
            for ( j=1 ; j<n_subs[c] ; j++ ) {
              x_i = w1[c][j] * X[c][f_num][i-1]
                  + w2[c][j] * X[c][f_num][i]
                  + w3[c][j] * X[c][f_num][i+1]
                  + w4[c][j] * X[c][f_num][i+2];
              y_i = w1[c][j] * Y[c][f_num][i-1]
                  + w2[c][j] * Y[c][f_num][i]
                  + w3[c][j] * Y[c][f_num][i+1]
                  + w4[c][j] * Y[c][f_num][i+2];
              z_i = w1[c][j] * Z[c][f_num][i-1]
                  + w2[c][j] * Z[c][f_num][i]
                  + w3[c][j] * Z[c][f_num][i+1]
                  + w4[c][j] * Z[c][f_num][i+2] ;
              glVertex3f(x_i, y_i, z_i);
            }
          }
          glVertex3f(X[c][f_num][i],
                     Y[c][f_num][i],
                     Z[c][f_num][i]);
        glEnd();
      }
    }

  } else {

  for ( c=0 ; c<MAX_CURTAINS ; c++ ) {

    if (num_points[c][f_num] > 0) {

      glBegin(GL_QUAD_STRIP);
        for ( i=1; i<(num_points[c][f_num]-2) ; i++ ) {
          hPt.x = 2500.0*X[c][f_num][i];
          hPt.y = 2500.0*Y[c][f_num][i];
          hPt.z = 2500.0*Z[c][f_num][i];
          cPt.x = 75.0*X[c][f_num][i];
          cPt.y = 75.0*Y[c][f_num][i];
          cPt.z = 75.0*Z[c][f_num][i];
          glColor4f(0.0, 0.8, 0.0, 0.3+noise(cPt)*0.7);
          glVertex3f(X[c][f_num][i],
                     Y[c][f_num][i],
                     Z[c][f_num][i]);
          glColor4f(0.0, 0.8, 0.0, 0.1+noise(cPt)*0.2);
          noise_ext = 1.0 + 0.25*(ext - 1.0) * noise(hPt) + 0.75*(ext - 1.0);
          glVertex3f(X[c][f_num][i]*noise_ext,
                     Y[c][f_num][i]*noise_ext,
                     Z[c][f_num][i]*noise_ext);
          for ( j=1 ; j<n_subs[c] ; j++ ) {
            x_i = w1[c][j] * X[c][f_num][i-1]
                + w2[c][j] * X[c][f_num][i]
                + w3[c][j] * X[c][f_num][i+1]
                + w4[c][j] * X[c][f_num][i+2];
            y_i = w1[c][j] * Y[c][f_num][i-1]
                + w2[c][j] * Y[c][f_num][i]
                + w3[c][j] * Y[c][f_num][i+1]
                + w4[c][j] * Y[c][f_num][i+2];
            z_i = w1[c][j] * Z[c][f_num][i-1]
                + w2[c][j] * Z[c][f_num][i]
                + w3[c][j] * Z[c][f_num][i+1]
                + w4[c][j] * Z[c][f_num][i+2];
            hPt.x = 2500.0*x_i;
            hPt.y = 2500.0*y_i;
            hPt.z = 2500.0*z_i;
            cPt.x = 75.0*x_i;
            cPt.y = 75.0*y_i;
            cPt.z = 75.0*z_i;
            glColor4f(0.0, 0.8, 0.0, 0.3+noise(cPt)*0.7);
            glVertex3f(x_i, y_i, z_i);
            glColor4f(0.0, 0.8, 0.0, 0.1+noise(cPt)*0.2);
            noise_ext = 1.0 + 0.25*(ext - 1.0) * noise(hPt) + 0.75*(ext - 1.0);
            glVertex3f(x_i*noise_ext, y_i*noise_ext, z_i*noise_ext);
          }
        }
        hPt.x = 2500.0*X[c][f_num][i];
        hPt.y = 2500.0*Y[c][f_num][i];
        hPt.z = 2500.0*Z[c][f_num][i];
        cPt.x = 75.0*X[c][f_num][i];
        cPt.y = 75.0*Y[c][f_num][i];
        cPt.z = 75.0*Z[c][f_num][i];
        glColor4f(0.0, 0.8, 0.0, 0.3+noise(cPt)*0.7);
        glVertex3f(X[c][f_num][i],
                   Y[c][f_num][i],
                   Z[c][f_num][i]);
        glColor4f(0.0, 0.8, 0.0, 0.1+noise(cPt)*0.2);
        noise_ext = 1.0 + 0.25*(ext - 1.0) * noise(hPt) + 0.75*(ext - 1.0);
        glVertex3f(X[c][f_num][i]*noise_ext,
                   Y[c][f_num][i]*noise_ext,
                   Z[c][f_num][i]*noise_ext);
      glEnd();

    }
  }

  }

  glDisable(GL_BLEND);

  glFlush();
}

void idle(void)
{
  FILE *fp;
  int  i;
  char filename[80];

  if (dump_frame) {
    /* grab frame buffer */
    glReadPixels(0,0,width,height,GL_RGB, GL_UNSIGNED_BYTE, image);

    /* write to a file */
    sprintf(filename, "%d.ppm", start_frame_dump+f_num);
    if ((fp = fopen(filename, "w")) == 0) {
      printf("Can't open %s\n",filename);
      exit(1);
    }
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n",width,height);
    fprintf(fp, "255\n");
    for ( i=0 ; i<width*height ; i++ ) {
      putc(image[i*3],fp);
      putc(image[i*3+1],fp);
      putc(image[i*3+2],fp);
    }
    fflush(fp);
    fclose(fp);
  }

  if (animate)
    if (f_num+1 < frames) f_num ++;
    else                  f_num = 0;

  glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
   glutInitWindowSize(width, height);
   glutInitWindowPosition(0, 0);
   glutCreateWindow(argv[0]);
   init();
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutDisplayFunc(draw_curtains);
   glutIdleFunc(idle);
   glutMainLoop();
   return 0; 
}
