#include <stdio.h>
#include <math.h>

#define MAX_ENTRIES 256
#define MAX_POINTS 10

static double table[MAX_ENTRIES][3];

#if 0
/* Green Channel */
static int ctl_pts = 3;
static int frame_num[MAX_POINTS] = {  0,  240,  255};
static float red[MAX_POINTS]     = {0.0, 0.29, 0.97};
static float green[MAX_POINTS]   = {0.0, 0.92, 0.97};
static float blue[MAX_POINTS]    = {0.0, 0.61, 0.97};
#endif

#if 1
/* Magenta Channel */
static int ctl_pts = 3;
static int frame_num[MAX_POINTS] = {  0, 240,  255};
static float red[MAX_POINTS]     = {0.0, 0.95, 0.97};
static float green[MAX_POINTS]   = {0.0, 0.71, 0.97};
static float blue[MAX_POINTS]    = {0.0, 0.91, 0.97};
#endif

#if 0
/* Red Channel */
static int ctl_pts = 3;
static int frame_num[MAX_POINTS] = {  0,  240,   255};
static float red[MAX_POINTS]     = {0.0,  0.92, 0.97};
static float green[MAX_POINTS]   = {0.0,  0.18, 0.97};
static float blue[MAX_POINTS]    = {0.0, 0.078, 0.97};
#endif

void interp_points(int a, int b)
{
  int i;
  float low_r, low_g, low_b, inc_r, inc_g, inc_b;

/*printf("frame %d to %d\n", frame_num[a], frame_num[b]-1);*/
  
  low_r = red[a]; low_g = green[a]; low_b = blue[a];
  inc_r = (red[b]-red[a]) / (frame_num[b]-frame_num[a]);
  inc_g = (green[b]-green[a]) / (frame_num[b]-frame_num[a]);
  inc_b = (blue[b]-blue[a]) / (frame_num[b]-frame_num[a]);
  for ( i=frame_num[a] ; i<frame_num[b] ; i++ ) {
    printf("{%.3f,%.3f,%.3f},", low_r, low_g, low_b);
    if (i % 4 == 3) printf("\n");
    low_r += inc_r; low_g += inc_g; low_b += inc_b;
  }
}

main()
{
  int i;

  printf("{");
  for ( i=1 ; i<ctl_pts ; i++ ) {
    interp_points(i-1, i);
  }
  printf("{%.3f,%.3f,%.3f}};\n",
         red[ctl_pts-1], green[ctl_pts-1], blue[ctl_pts-1]);
}
