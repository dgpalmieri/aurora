#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "noise.h"

#define WHITESPACE " :,\t\n"

#define _DUMP_IT
#define EARTH_TEXTURE

static int frames;
static int points;
static int draw_earth=0;
static int view_no=2;

#define NUM_POTS 0
#define MAX_VIEWS 4
static GLuint texture2D[1+NUM_POTS];

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

static int width=1024;
static int height=1024;

 #define VOL_XRES  500
 #define VOL_YRES  200
 #define VOL_ZRES  500
 float vol[VOL_XRES*VOL_YRES*VOL_ZRES];
 int vol_samples;
 double vol_xmin = -3500.0; 
 double vol_xmax =  3500.0;
 double vol_ymin =  5700.0;
 double vol_ymax =  6800.0;
 double vol_zmin = -3500.0;
 double vol_zmax =  3500.0;


typedef struct { double x, y, z; } XYZ;
XYZ iP[100];
XYZ iV[100];
XYZ iN[100];
XYZ iC[100];

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
  int i,j,c,s,t;
  XYZ center;
  int x, y;
  int tmp;

  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);

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
      earth_tex_map[t][EARTH_WIDTH-s][0] = Red_image[s+t*EARTH_WIDTH];
      earth_tex_map[t][EARTH_WIDTH-s][1] = Green_image[s+t*EARTH_WIDTH];
      earth_tex_map[t][EARTH_WIDTH-s][2] = Blue_image[s+t*EARTH_WIDTH];
      earth_tex_map[t][EARTH_WIDTH-s][3] = 255;
    }
  }

  /* load texture into OpenGL */
  glGenTextures(1+NUM_POTS, texture2D);
  glBindTexture(GL_TEXTURE_2D, texture2D[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, EARTH_WIDTH, EARTH_HEIGHT, 
                0, GL_RGBA, GL_UNSIGNED_BYTE, earth_tex_map);

  center.x = center.y = center.z = 0.0;
  CreateSphere(center, 6378.1, 40);

  vol_samples = VOL_XRES * VOL_YRES * VOL_ZRES;
 
  fp = fopen("energy_volume", "rb");
  if (!fp) { printf("Can't open energy_volume\n"); exit(1); }
  tmp = fread(vol, sizeof(float), vol_samples, fp);
  printf("%d samples read\n", tmp);
  if (tmp != vol_samples) {
    printf("Bad read of volume.energy\n");
    exit(1);
  }
  fclose(fp);

}

void keyboard(unsigned char key, int x, int y)
{ 
   switch (key) {
      case 27:  exit(0);
                break;
      case 'E':
      case 'e': if (draw_earth) draw_earth=0;
                else            draw_earth=1;
                break;
      case 'V':
      case 'v': view_no ++;
                if (view_no >= MAX_VIEWS) view_no = 0;
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
  int x, y, z;
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
  } else if (view_no == 1) { /* Shuttle view of curtains */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (GLfloat) width/(GLfloat) height, 4000.0, 12000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(6385.86, 6385.86, 6385.86, 0, 5676.32, 0, 0, 1, 0);
    glRotatef(-40.0, 0.0, 1.0, 0.0);
  } else if (view_no == 2) { /* approximate Fairbanks viewpoint */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(155, (GLfloat) width/(GLfloat) height, 10.0, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(2638.07, 5810.35, 230.88, 5276.14, 14190.8, 461.77, -1, 0, 0);
  } else { /* from above north pole */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (GLfloat) width/(GLfloat) height, 10000.0, 25000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 17738.5, 0, 0, 0, 0, 0, 0, -1);
    glRotatef(-80.0, 0.0, 1.0, 0.0);
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* make the Z-buffer writable for drawing opaque objects */
  glDepthMask(GL_TRUE);

  if (draw_earth) {
      glPushMatrix();
      glRotatef(-40.0, 0.0, 1.0, 0.0);
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

  glBegin(GL_POINTS);
   for ( z=0 ; z<VOL_ZRES ; z++ ) {
    for ( y=0 ; y<VOL_YRES ; y++ ) {
     for ( x=0 ; x<VOL_XRES ; x++ ) {
      if (vol[z*VOL_XRES*VOL_YRES + y*VOL_XRES + x] > 0.0) {
       glVertex3f(vol_xmin + ((double)x/VOL_XRES) * (vol_xmax - vol_xmin),
                  vol_ymin + ((double)y/VOL_YRES) * (vol_ymax - vol_ymin),
                  vol_zmin + ((double)z/VOL_ZRES) * (vol_zmax - vol_zmin));
      }
     }
    }
   }
  glEnd();

  glDisable(GL_BLEND);

  glFlush();
}

void idle(void)
{
  FILE *fp;
  int  i;
  char filename[80];

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
