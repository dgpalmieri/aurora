#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

int ImageWidth=0;
int ImageHeight=0;
int num_frames=0;
GLubyte **frames;

static GLdouble zoomFactor = 1.0;
static GLint raster_x = 0;
static GLint raster_y = 0;
static GLint delay = 0;
static GLint pause = 0;
static GLint step = 0;
static double dummy[16];

static int redraw_count = 0;
static clock_t t_start, t_last, t_now;
static double fps, avg_fps;

void read_frames(int argc, char* argv[])
{
  int frame, beg_frame, end_frame, inc_frame;
  char fname[80];
  FILE *fp;
  int  resolution_set=0;
  int  xres, yres, maxval;
  int  tmp;
  int  total_frames;
  int x, y, half;
  GLubyte tmpb;
  GLubyte *ptr1, *ptr2;
  GLubyte *img;

  beg_frame = atoi(argv[2]);
  end_frame = atoi(argv[3]);
  inc_frame = atoi(argv[4]);
  if (beg_frame < 0 || inc_frame <= 0 || beg_frame > end_frame) {
    printf("Invalid frame range\n");
    exit(1);
  }

  /* allocate pointers to hold frame sequence */
  total_frames = (end_frame - beg_frame) / inc_frame + 1;
  frames = (GLubyte **) malloc( total_frames*sizeof(GLubyte *) );
  if (!frames) {
    printf("Can't malloc array of pointers for frames\n");
    exit(1); 
  }

  num_frames = 0;
  for ( frame=beg_frame ; frame<=end_frame ; frame+=inc_frame, num_frames++ ) {

    sprintf(fname, argv[1], frame);
    printf("Loading %s\n", fname); fflush(stdout);

    /* open file */
    if ((fp=fopen(fname, "rb")) == NULL) {
      printf("Can't open %s\n", fname);
      exit(1);
    }

    /* read header */
    if (fscanf(fp, "P6\n%d %d\n%d\n", &xres, &yres, &maxval) != 3)
      { printf("Error reading header\n"); exit(1); }
    if (resolution_set) {
      if (xres!=ImageWidth || yres!=ImageHeight)
        { printf("Image resolutions do not match\n"); exit(1); }
    } else {
      ImageWidth = xres;
      ImageHeight = yres;
      resolution_set = 1;
    }

    /* malloc memory and read frame */
    frames[num_frames] = (GLubyte *) malloc( xres*yres*3*sizeof(GLubyte) );
    if (!frames[num_frames]) {
      printf("Can't malloc memory for frame %d\n", frame);
      exit(1);
    }
    tmp = fread(frames[num_frames], 1, xres*yres*3, fp);
    if (tmp != xres*yres*3) {
      printf("Bad read\n");
      exit(1);
    }
    fclose(fp);

    /* flip the image as PPM has 0,0 at top left */
    half = ImageHeight/2;
    img = frames[num_frames];
    for ( y=0 ; y<half ; y++ ) {
      /* exchange row y with row ImageHeight-1-y */
#if 0
printf("%d: Exchange row %d with row %d\n", num_frames, y, ImageHeight-1-y);
#endif
      ptr1 = img + y*ImageWidth*3;
      ptr2 = img + (ImageHeight-1-y)*ImageWidth*3;
      for ( x=0 ; x<ImageWidth*3 ; x++ ) {
        tmpb = *ptr1;
        *ptr1 = *ptr2;
        *ptr2 = tmpb;
        ptr1++; ptr2++;
      }
    }

  }

  if (total_frames != num_frames) {
    printf("Only read %d of %d frames\n", num_frames, total_frames);
    exit(1);
  }

}

void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   t_start = t_now = clock();
}

void display(void)
{
   int i;

   glClear(GL_COLOR_BUFFER_BIT);
   glRasterPos2i(raster_x, raster_y);
   glPixelZoom(zoomFactor, zoomFactor);
   glDrawPixels(ImageWidth, ImageHeight, GL_RGB, GL_UNSIGNED_BYTE,
                frames[redraw_count % num_frames]);
   glFlush();
   glutSwapBuffers();
   if (step) step=0; else redraw_count++;
   t_last = t_now;
   t_now = clock();
   fps = 1.0 / ((double)(t_now - t_last) / CLOCKS_PER_SEC);
   avg_fps = redraw_count / ((double)(t_now - t_start) / CLOCKS_PER_SEC);
   printf("frame # %d - %.1lf fps %.1lf avg fps\n",
          redraw_count, fps, avg_fps);

   for ( i=0 ; i<delay ; i++ ) {
     dummy[i%16] = sqrt((double)i) / t_now;
   }
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   switch (key) {
      case ' ':
         pause = (pause + 1) % 2;
         break;
      case 'n':
      case 'N':
         redraw_count ++;
         step=1;
         break;
      case 'p':
      case 'P':
         redraw_count --;
         if (redraw_count < 0) redraw_count += num_frames;
         step=1;
         break;
      case 'r':
      case 'R':
         zoomFactor = 1.0;
         raster_x = raster_y = 0;
         glutPostRedisplay();
         printf ("zoomFactor reset to 1.0\n");
         break;
      case 'd':
         delay += 25000;
printf("delay = %d\n", delay);
         break;
      case 'D':
         if (delay >= 25000) delay -= 25000;
         break;
      case 'z':
         zoomFactor += 0.1;
         if (zoomFactor >= 4.0) zoomFactor = 4.0;
         printf ("zoomFactor is now %4.1f\n", zoomFactor);
         break;
      case 'Z':
         zoomFactor -= 0.1;
         if (zoomFactor <= 0.2) zoomFactor = 0.2;
         printf ("zoomFactor is now %4.1f\n", zoomFactor);
         break;
      case 'j':
      case 'J':
         raster_y -= 10;
         if (raster_y < 0) raster_y = 0;
         break;
      case 'k':
      case 'K':
         raster_y += 10;
         break;
      case 'h':
      case 'H':
         raster_x -= 10;
         if (raster_x < 0) raster_x = 0;
         break;
      case 'l':
      case 'L':
         raster_x += 10;
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
  if (step || !pause)
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
   if (argc != 5) {
     printf("USAGE: %s file_pattern start_frame end_frame frame_inc\n",argv[0]);
     exit(1);
   }

   read_frames(argc, argv);

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

   glutInitWindowSize(ImageWidth, ImageHeight);
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
