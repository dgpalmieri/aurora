#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "noise.h"

GLfloat wxmin=-0.5;
GLfloat wxmax= 0.5;
GLfloat wymin=-0.5;
GLfloat wymax= 0.5;
GLint xsize=1000;
GLint ysize=1000;
GLint offset=0;
//GLfloat xPeriod = 5.0;
//GLfloat yPeriod = 10.0;
//double turbPower = 5.0;
//double turbSize = 32.0;
GLfloat xPeriod = 0.0;
GLfloat yPeriod = 1.0;
double turbPower = 1.0;
double turbSize = 64.0;
GLfloat zValue = 0.0;
GLfloat zInc = 1.0;

void init(void)
{    
   int i, s;
   FILE *fp;
   int tmp1;
   float tmp2;

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   initnoise();

}

double turbulence(double x, double y, double size)
{
  double value = 0.0;
  double weight = 1.0 / size;
  double initialsize = 1.0 / (size*2.0);
  Vector p;

  while (size >= 1.0) {
    p.x = x*weight; p.y = y*weight; p.z = zValue;
    value += noise(p) * size;
    weight *= 2.0;
    size *= 0.5;
  }

  return value*initialsize;
}

#if 0
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

//printf("x=%lf y=%lf val=%lf\n", x, y, value/initialsize);

  return value/(initialsize*2.0);
}
#endif

void display(void)
{
   int i,j;
   double xyValue;
   double sineValue;
Vector p;
double x, y;

//double minval = 10e38;
//double maxval = 10e-38;
   glClear(GL_COLOR_BUFFER_BIT);

   glPointSize(1.0);

   glBegin(GL_POINTS);
     for ( i=0 ; i<xsize ; i++ ) {
       x=wxmin + ((double)i/xsize) * (wxmax-wxmin);
       for ( j=0 ; j<ysize ; j++ ) {

//         xyValue = i*xPeriod / ysize + (j+offset%xsize)*yPeriod / xsize  +
//                   turbPower * turbulence(i*0.2, j*0.2, turbSize);
//         sineValue = fabs(sin(xyValue*M_PI));


//         p.x = i*0.05; p.y = j*0.05; p.z = 0.0;
//         xyValue = noise(p);

//         xyValue = turbulence(i*0.25,j*0.25,8.0);
         y=wymin + ((double)j/ysize) * (wymax-wymin);
         xyValue = 0.4 + 0.6 * turbulence(x*75.0,y*75.0,4.0);

//if (xyValue < minval) minval = xyValue;
//if (xyValue > maxval) maxval = xyValue;

//         glColor3f(sineValue, sineValue, sineValue);
         glColor3f(xyValue, xyValue, xyValue);
         glVertex2i(i,j);
       }
     }
   glEnd();

  zValue += zInc;

//printf("Range %lf to %lf\n", minval, maxval);

   glFlush();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   xsize = w;
   ysize = h;
   glOrtho(0.0, xsize, 0.0, ysize, 0.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 'z': if (wxmax > 501.0) {
                  wxmin += 500.0; wxmax -= 500.0;
                  wymin += 500.0; wymax -= 500.0;
                }
                printf("X:(%lf,%lf) Y:(%lf,%lf)\n", wxmin, wxmax, wymin, wymax);
                glutPostRedisplay();
                break;
      case 'Z': wxmin -= 500.0; wxmax += 500.0;
                wymin -= 500.0; wymax += 500.0;
                break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }
}

void idle(void)
{
//  glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

   glutInitWindowSize(xsize, ysize);
   glutInitWindowPosition(0, 0);
   glutCreateWindow(argv[0]);

   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(idle);
   glutMainLoop();
   return 0; 
}
