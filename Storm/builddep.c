#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_PTS 81
#define N_PTS_1 80.0
#define MAX_DRAW 79.0

#define GREEN_DEP   0
#define RED_DEP     1
#define MAGENTA_DEP 2

int c_p = 0;
int c_c = GREEN_DEP;

GLfloat pts[NUM_PTS][3];

void print_splines()
{
  int i, c;

#if 1
  for ( i=0 ; i<NUM_PTS ; i++ ) {
    printf("%d %f %f %f %f\n", i, 6370.0 + (508 * i / N_PTS_1),
           pts[i][0], pts[i][1], pts[i][2]);
  }
#endif

  for ( c=0 ; c<3 ; c++ ) {
    printf("Color channel %d:\n", c);
    for ( i=0 ; i<NUM_PTS ; i++ ) {
      printf("%f,", pts[i][c]);
      if (i%8 == 7) printf("\n");
    }
    printf("\n");
  }

}

void init(void)
{    
   int i, s;
   FILE *fp;
   int tmp1;
   float tmp2;

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);

   /* see if initial data is already there */
   fp = fopen("depositions.txt", "r");
   if (!fp) {
     printf("Using default values\n");

     /* this is the original value of each spline point */
     for ( i=0 ; i<NUM_PTS ; i++ ) {
       pts[i][0] = pts[i][1] = pts[i][2]  = 0.0;
     }

   } else {

     for ( i=0 ; i<NUM_PTS ; i++ ) {
       fscanf(fp, "%d %f %f %f %f\n", &tmp1, &tmp2, 
              &(pts[i][0]), &(pts[i][1]), &(pts[i][2]));
     }
     fclose(fp);
   }
}

void display(void)
{
   int i;
   double u;
   double u1, u2, u3;
   double w1, w2, w3, w4;
   int c;

   glClear(GL_COLOR_BUFFER_BIT);

   for ( c=GREEN_DEP ; c<=MAGENTA_DEP; c++ ) {

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
         glVertex3f( w1*pts[i-1][c] + w2*pts[i][c]
                   + w3*pts[i+1][c] + w4*pts[i+2][c],
                     u / N_PTS_1, 0.0);
       }
     glEnd();

     /* display control points for active spline */
     if (c==c_c) {
       glPointSize(4.0);
       glBegin(GL_POINTS);
         for ( i=0 ; i<NUM_PTS ; i++ ) {
           if (i==c_p) glColor3f(1.0, 1.0, 0.0);
           else        glColor3f(0.0, 0.0, 1.0);
           glVertex3f(pts[i][c], i / N_PTS_1, 0.0);
         }
       glEnd();
     }

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
     glOrtho(-0.1, 1.1, -0.1*((GLfloat)h/w), 1.1*((GLfloat)h/w), -0.1, 1.1);
   else
     glOrtho(-0.1*((GLfloat)w/h), 1.1*((GLfloat)w/h), -0.1, 1.1, -0.1, 1.1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 'c': c_c ++;
                if (c_c > MAGENTA_DEP) c_c = 0;
                c_p = 0;
                break;
      case 'C': c_c --;
                if (c_c < 0) c_c = MAGENTA_DEP;
                c_p = 0;
                break;
      case 'p': c_p ++;
                if (c_p >= NUM_PTS) c_p = 0;
                break;
      case 'P': c_p --;
                if (c_p < 0) c_p = NUM_PTS - 1;
                break;
      case 'h': /* left arrow */
                pts[c_p][c_c] -= 0.0025;
                break;
      case 'l': /* right arrow */
                pts[c_p][c_c] += 0.0025;
                break;
      case 'Z': print_splines();
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

   glutInitWindowSize(1000, 1000);
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
