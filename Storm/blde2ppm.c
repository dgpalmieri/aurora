#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NUM_PTS 14
#define N_PTS_1 13.0
#define MAX_DRAW 11.0

#define GREEN_DEP   0
#define RED_DEP     1
#define MAGENTA_DEP 2

int c_p = 0;
GLfloat sp_u[NUM_PTS];
GLfloat sp_y[NUM_PTS];

static GLint ImageWidth = 1280;
static GLint ImageHeight = 1024;
GLubyte *frame = NULL;
float *values = NULL;
float min, max, map_range;
double max_green[3] = { 0.29, 0.92, 0.61 };


void read_energy(
char *infilename
)
{
  FILE *fp;
  int tmpxres;
  int tmpyres;
  int i;

  if ((fp=fopen(infilename, "rb")) == NULL)
    { printf("%s not found\n", infilename); exit(1); }

  printf("Reading file %s\n", infilename);

  if (fscanf(fp, "#FRAW\n%d %d\n", &tmpxres, &tmpyres) != 2) {
    printf("Error reading header\n");
    exit(1);
  }

  /* verify these are the same */
  if (ImageWidth != tmpxres || ImageHeight != tmpyres) {
    printf("Raster resolutions do not match\n");
    exit(1);
  }

  /* need to malloc memory to hold input energy */
  values = (float *) malloc(ImageWidth*ImageHeight*sizeof(float));
  if (!values) {
    printf("Can't malloc array to hold values\n");
    exit(1);
  }

  if (fread(values, sizeof(float), ImageWidth*ImageHeight, fp) != ImageWidth*ImageHeight) {
    printf("Error reading data\n");
    free(values);
    exit(1);
  }

  fclose(fp);

  printf("%d bytes (data) read.\n", ImageWidth*ImageHeight*sizeof(float));

  min = max = fabs(values[0]) - 1.0;
  for ( i=1 ; i<ImageWidth*ImageHeight ; i++ ) {
    if ((fabs(values[i])-1.0) > max) max = fabs(values[i])-1.0;
    if ((fabs(values[i])-1.0) < min) min = fabs(values[i])-1.0;
  }
  printf("%s: Range of values for mapping [%lf, %lf]\n", infilename, min, max);
  map_range = max - min;

}


void energy2frame(
)
{
  int i, u_i;
  double alpha;
  double u;
  double u1, u2, u3;
  double w1, w2, w3, w4;
  double u_value;
  double color;

  /* add this energy to the raster */
  for ( i=0 ; i<ImageWidth*ImageHeight ; i++ ) {

    if ((fabs(values[i])-1.0) <= min) alpha = 0.0;
    else if ((fabs(values[i])-1.0) >= max) alpha = 1.0;
    else alpha = (fabs(values[i])-1.0-min) / map_range;

    u_value = 1.0 + alpha * 10.0;

    u_i = (int) u_value;
    u1 = u_value - u_i; u2 = u1 * u1; u3 = u2 * u1;
    w1 = -0.5*u3 +     u2 - 0.5*u1;
    w2 =  1.5*u3 - 2.5*u2 + 1.0;
    w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
    w4 =  0.5*u3 - 0.5*u2;

    color = w1*sp_y[u_i-1] + w2*sp_y[u_i] + w3*sp_y[u_i+1] + w4*sp_y[u_i+2];

if (color < 0.0) color = 0.0;
if (color > 1.0) color = 1.0;

//    frame[3*i]   = 0;
//    frame[3*i+1] = (GLubyte) (color * 255);
//    frame[3*i+2] = 0;

    frame[3*i]   = (GLubyte) ((color * max_green[0]) * 255);
    frame[3*i+1] = (GLubyte) ((color * max_green[1]) * 255);
    frame[3*i+2] = (GLubyte) ((color * max_green[2]) * 255);

  }

}


void print_splines()
{
  int i, c;

  for ( i=0 ; i<NUM_PTS ; i++ ) 
    printf("%d %f %f\n", i, sp_u[i], sp_y[i]);
}

void init(void)
{    
   int i, s;
   FILE *fp;
   int tmp1;
   float tmp2;
   double tmp3;

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);

   /* see if initial data is already there */
   fp = fopen("curves.txt", "r");
   if (!fp) {
     printf("Using default values\n");
     /* this is the original value of each spline point */
     for ( i=0 ; i<NUM_PTS ; i++ ) {
       sp_u[i] = i;
       sp_y[i] = -0.1 + (i / (MAX_DRAW-1.0));
     }
   } else {
     for ( i=0 ; i<NUM_PTS ; i++ )
       fscanf(fp, "%d %f %f\n", &tmp1, &(sp_u[i]), &(sp_y[i]));
     fclose(fp);
   }

   /* read in the energy file */
   read_energy("a_06090.green");

   /* allocate memory to hold raster to display */
   frame = (GLubyte *) malloc( ImageWidth*ImageHeight*3*sizeof(GLubyte) );
   if (!frame) {
     printf("Can't malloc memory for image\n");
     exit(1);
   }

/*
   for ( i=0 ; i<ImageWidth*ImageHeight ; i++ )
     frame[i] = i % 256;
*/

   energy2frame();

}

void display2(void)
{
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   glViewport(0, 0, (GLsizei) ImageWidth, (GLsizei) ImageHeight);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLdouble) ImageWidth, 0.0, (GLdouble) ImageHeight);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glClear(GL_COLOR_BUFFER_BIT);
   glRasterPos2i(0, 0);
   glDrawPixels(ImageWidth, ImageHeight, GL_RGB, GL_UNSIGNED_BYTE, frame);
   glFlush();

printf("Display2\n");

}

void display(void)
{
   int i;
   double u;
   double u1, u2, u3;
   double w1, w2, w3, w4;
   int c;

   glClear(GL_COLOR_BUFFER_BIT);

   for ( c=GREEN_DEP ; c<=GREEN_DEP; c++ ) {

     /* select the color for the curve */
     if (c==GREEN_DEP) glColor3f(0.0, 1.0, 0.0);
     else if (c==RED_DEP) glColor3f(1.0, 0.0, 0.0);
     else glColor3f(1.0, 0.0, 1.0);

     /* draw the spline */
     glBegin(GL_LINE_STRIP);
       for ( u=1.0; u<=MAX_DRAW ; u+=0.1 ) {
         i = (int) u;
         u1 = u - i; u2 = u1 * u1; u3 = u2 * u1;
         w1 = -0.5*u3 +     u2 - 0.5*u1;
         w2 =  1.5*u3 - 2.5*u2 + 1.0;
         w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
         w4 =  0.5*u3 - 0.5*u2;
         glVertex3f(-0.1+u/10.0, w1*sp_y[i-1] + w2*sp_y[i] + w3*sp_y[i+1] + w4*sp_y[i+2], 0.0);
       }
     glEnd();

     /* display control points for active spline */
     glPointSize(4.0);
     glBegin(GL_POINTS);
       for ( i=0 ; i<NUM_PTS ; i++ ) {
         if (i==c_p) glColor3f(1.0, 1.0, 0.0);
         else        glColor3f(0.0, 0.0, 1.0);
         glVertex3f(-0.1+sp_u[i]/10.0, sp_y[i], 0.0);
       }
     glEnd();

   }

   glColor3f(1.0, 1.0, 1.0);
   glBegin(GL_LINE_STRIP);
     glVertex3f(0.0, 0.0, 0.0);
     glVertex3f(0.0, 1.0, 0.0);
     glVertex3f(1.0, 1.0, 0.0);
     glVertex3f(1.0, 0.0, 0.0);
     glVertex3f(0.0, 0.0, 0.0);
   glEnd();

   glFlush();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
     glOrtho(-0.21, 1.21, -0.21*((GLfloat)h/w), 1.21*((GLfloat)h/w), -0.21, 1.21);
   else
     glOrtho(-0.21*((GLfloat)w/h), 1.21*((GLfloat)w/h), -0.21, 1.21, -0.21, 1.21);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 'p': c_p ++;
                if (c_p >= NUM_PTS) c_p = 0;
                break;
      case 'P': c_p --;
                if (c_p < 0) c_p = NUM_PTS - 1;
                break;
      case 'j': /* down arrow */
                sp_y[c_p] -= 0.01;
                break;
      case 'k': /* up arrow */
                sp_y[c_p] += 0.01;
                break;
      case 'u': /* update the raster with the new curve */
                energy2frame();
                break;
      case 'Z': print_splines();
                break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }
   glutPostRedisplay();
}

void idle(void)
{
//  glutPostRedisplay();
}

int win1, win2;

int main(int argc, char** argv)
{
   glutInit(&argc, argv);

   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize(800, 800);
   glutInitWindowPosition(0, 0);
   win1 = glutCreateWindow(argv[0]);

   init();

   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(idle);

   glutInitWindowSize(ImageWidth, ImageHeight);
   glutInitWindowPosition(200, 0);
   win2 = glutCreateWindow("image");
   glutDisplayFunc(display2);

   glutMainLoop();
   return 0; 
}
