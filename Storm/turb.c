#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#define MAX_PARTS 10000

typedef struct {
  int alive;
  GLfloat pos_x, pos_y;
  GLfloat dir_x, dir_y;
} Particle;

Particle parts[MAX_PARTS];
int NumParticles;

#define NUMX 100
#define NUMY 100

void init(void)
{
  int i, j;

  /* create a grid of points */
  NumParticles = 0;
  for ( i=0 ; i<100 ; i++ ) {
    for ( j=0 ; j<100 ; j++ ) {
      parts[NumParticles].alive =  1;
      parts[NumParticles].pos_x = -1.0 + 2.0 * ((double)i / (NUMX-1));
      parts[NumParticles].pos_y = -1.0 + 2.0 * ((double)j / (NUMY-1));
      parts[NumParticles].dir_x = -0.001;
      parts[NumParticles].dir_y =  0.0;
      NumParticles ++;
    }
  }
}

void time_step(void)
{
  int i;

  for ( i=0 ; i<NumParticles ; i++ ) {
    if (parts[i].alive) {
      if (parts[i].pos_x < 0.0) {
        parts[i].pos_x += parts[i].dir_x;
        parts[i].pos_y += parts[i].dir_y - 0.001;
      } else {
        parts[i].pos_x += parts[i].dir_x;
        parts[i].pos_y += parts[i].dir_y;
      }
    }
  }
}

void draw_timestep(void)
{
  int i;

  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glPointSize(1.0);

  glBegin(GL_POINTS);
    for ( i=0 ; i<NumParticles ; i++ )
      if (parts[i].alive)
        glVertex2f(parts[i].pos_x, parts[i].pos_y);
  glEnd();

  glFlush();
  glutSwapBuffers();
}

void display(void)
{
  draw_timestep();
  time_step();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
     glOrtho(-2.0, 2.0, -2.0*((GLfloat)h/w), 2.0*((GLfloat)h/w), -2.0, 2.0);
   else
     glOrtho(-2.0*((GLfloat)w/h), 2.0*((GLfloat)w/h), -2.0, 2.0, -2.0, 2.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case 'l': /* right arrow */
                break;
      case 27: exit(0);
               break;
      default: break;
   }
}

void idle(void)
{
  glutPostRedisplay();
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

   glutInitWindowSize(800, 800);
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
