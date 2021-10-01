#include <stdio.h>
#include <math.h>

#define NUM_POINTS 103

#if 0
/* curtainMain.data */
static int ctl_pts = 3;
static int frame_num[103] =
{   0, 550, 800 };
static float texture2use[103] =
{ 0.0, 210.0, 0.0 };
#endif

#if 1
/* secondaryCurtain.data */
static int ctl_pts = 5;
static int frame_num[103] =
{   0, 100,   500, 650, 800 };
static float texture2use[103] =
{ 0.0, 0.0, 210.0, 0.0, 0.0 };
#endif

#if 0
/* tertiaryCurtain.data & clouds.data */
static int ctl_pts = 4;
static int frame_num[103] =
{   0, 325,   500, 800 };
static float texture2use[103] =
{ 0.0, 0.0, 210.0, 0.0 };
#endif

void interp_points(
int a,
int b
)
{
  int i;
  float low, inc;

/*
  printf("Processing %d to %d\n", a, b);
  printf("frame %d to %d\n", frame_num[a], frame_num[b]-1);
*/
  
  low  = texture2use[a];
  inc = (texture2use[b]-texture2use[a]) / (frame_num[b] - frame_num[a]);
  for ( i=frame_num[a] ; i<frame_num[b] ; i++ ) {
    printf("%d %f\n", i, low);
    low += inc;
  }

}

main()
{
  int i;

  for ( i=1 ; i<ctl_pts ; i++ ) {
    interp_points(i-1, i);
  }
  printf("%d %f\n", frame_num[ctl_pts-1], texture2use[ctl_pts-1]);

}

