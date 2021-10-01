#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define NUM_PTS 11
#define N_PTS_1 10.0
#define NUM_STEPS 10

int c_p = 0;
int c_s = 0;

/* test overlay spline */
/*
0 { -5.000000,  0.000000 },
1 { -4.000000,  0.000000 },
2 { -2.300001,  0.200000 },
3 { -0.600000,  1.400000 },
4 {  1.100000,  1.400000 },
5 {  0.000000,  0.000000 },
6 { -0.900000, -1.600000 },
7 {  1.000000, -1.300000 },
8 {  2.300001, -0.300000 },
9 {  4.000000,  0.000000 },
10{  5.000000,  0.000000 }
*/

GLfloat pts[NUM_STEPS][NUM_PTS][3];

void init(void)
{    
   int i, s;
   double step_size;

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);

   /* set all control points in all steps to initial values */
   step_size = 10.0 / N_PTS_1;
   for ( s=0 ; s<NUM_STEPS ; s++ ) {
     for ( i=0 ; i<NUM_PTS ; i++ ) {
       pts[s][i][0] = -5.0 + i*step_size;
       pts[s][i][1] = 0.0;
       pts[s][i][2] = 0.0;
     }
   }
}

void display(void)
{
   int i;
   double u;
   double u1, u2, u3;
   double w1, w2, w3, w4;

   glClear(GL_COLOR_BUFFER_BIT);

   glColor3f(0.0, 1.0, 0.0);
   glBegin(GL_LINE_STRIP);
     for ( u=1.0; u<=9.0 ; u+=0.1 ) {
       i = (int) u;
       u1 = u - i; u2 = u1 * u1; u3 = u2 * u1;
       w1 = -0.5*u3 +     u2 - 0.5*u1;
       w2 =  1.5*u3 - 2.5*u2 + 1.0;
       w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
       w4 =  0.5*u3 - 0.5*u2;
       glVertex3f( w1*pts[c_s][i-1][0] + w2*pts[c_s][i][0]
                 + w3*pts[c_s][i+1][0] + w4*pts[c_s][i+2][0],
                   w1*pts[c_s][i-1][1] + w2*pts[c_s][i][1]
                 + w3*pts[c_s][i+1][1] + w4*pts[c_s][i+2][1],
                   w1*pts[c_s][i-1][2] + w2*pts[c_s][i][2]
                 + w3*pts[c_s][i+1][2] + w4*pts[c_s][i+2][2] );
     }
   glEnd();

   glPointSize(4.0);
   glBegin(GL_POINTS);
     for ( i=0 ; i<NUM_PTS ; i++ ) {
       if (i==c_p) glColor3f(1.0, 0.0, 0.0);
       else        glColor3f(0.0, 0.0, 1.0);
       glVertex3f(pts[c_s][i][0], pts[c_s][i][1], pts[c_s][i][2]);
     }
   glEnd();

   glFlush();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
     glOrtho(-5.1, 5.1, -5.1*((GLfloat)h/w), 5.1*((GLfloat)h/w), -5.1, 5.1);
   else
     glOrtho(-5.1*((GLfloat)w/h), 5.1*((GLfloat)w/h), -5.1, 5.1, -5.1, 5.1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 's': c_s ++;
                if (c_s >= NUM_STEPS) c_s = 0;
                break;
      case 'S': c_s --;
                if (c_s < 0) c_s = NUM_STEPS - 1;
                break;
      case 'p': c_p ++;
                if (c_p >= NUM_PTS) c_p = 0;
                break;
      case 'P': c_p --;
                if (c_p < 0) c_p = NUM_PTS - 1;
                break;
                pts[c_s][c_p][1] += 0.1;
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
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

   glutInitWindowSize(1100, 1100);
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
