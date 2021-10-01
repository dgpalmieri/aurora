#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rays.h"

#define CurtainWidth 60.0

/* info to read the texture map */
#define TEXT_XRES 128
#define TEXT_YRES 4096
static unsigned char texture[TEXT_XRES*TEXT_YRES];
static char *texture_name = "/Genetti/Hayden/05-10-31/128/snap0150.pbm";

/* info of file to write out */
#define OUT_XRES 301
#define OUT_YRES 301
static unsigned char raster[OUT_XRES*OUT_YRES];
static char *out_name = "dmsp_00480.pgm";

/* spline files to read in */
#define NUM_SPLINES 2
#define NUM_POINTS 103
static char *splines[NUM_SPLINES] =
  { "22100_1.data", "22100_2.data", "22100_3.data", "22100_4.data" };
static int  seg_step_size[NUM_SPLINES] = { 1.0, 1.0, 1.0, 1.0 };
/* 0.02, 0.08, 0.06, 0.025 */
static double spX[NUM_POINTS];
static double spY[NUM_POINTS];
static double spZ[NUM_POINTS];

typedef struct { Point v[4]; Vector n; double d; } Polygon;

#define MAX_POLYS 100000
static int num_polys=0;
static Polygon polys[MAX_POLYS];

void write_image()
{
  FILE *fp;

  fp = fopen(out_name, "wb");
  if (!fp) { printf("Can't open %s\n", out_name); exit(1); }
  fprintf(fp, "P5\n");
  fprintf(fp, "%d %d\n%d\n", OUT_XRES, OUT_YRES, 255);
  fwrite(raster, sizeof(unsigned char), OUT_XRES*OUT_YRES, fp);
  fclose(fp);
}

void get_texture()
{
  FILE *fp;
  int xres, yres, maxval, u, v;

  if ((fp = fopen(texture_name, "r")) == 0)
    { printf("Can't open %s\n", texture_name); exit(1); }
  
  fscanf(fp, "P5\n");
  if (fscanf(fp, "%d %d\n%d\n", &xres, &yres, &maxval) != 3)
    { printf("Error reading header\n"); exit(1); }

  if (xres != TEXT_XRES || yres != TEXT_YRES)
    { printf("Invalid size for texture map\n"); exit(1); }
  
  for ( v=0 ; v<yres ; v++ ) 
    for ( u=0 ; u<xres ; u++ )
      texture[v+u*TEXT_YRES] = (unsigned char) getc(fp);

  fclose(fp);

  printf("Read texture %s (%dx%d)\n", texture_name, TEXT_XRES, TEXT_YRES);
}

void read_spline(
int sp_num
)
{
  FILE *fp;
  int i;

  if ((fp = fopen(splines[sp_num], "r")) == 0)
    { printf("Can't open %s\n", splines[sp_num]); exit(1); }

  for ( i=0 ; i<NUM_POINTS ; i++ ) {
    fscanf(fp, "%lf %lf %lf\n", &(spX[i]), &(spY[i]), &(spZ[i]));
  }

  fclose(fp);

  printf("Read spline %s\n", splines[sp_num]);
}

Point spline_point(double u)
{
  Point P;
  int i;
  double u1, u2, u3;
  double w1, w2, w3, w4;
  
  i = (int) u;
  if (i < 1 || i>=NUM_POINTS-2)
    { P.x = P.y = P.z = 0.0; return P; } 
    
  u1 = u - i;
  u2 = u1 * u1;
  u3 = u2 * u1;

  w1 = -0.5*u3 +     u2 - 0.5*u1;
  w2 =  1.5*u3 - 2.5*u2 + 1.0;
  w3 = -1.5*u3 + 2.0*u2 + 0.5*u1;
  w4 =  0.5*u3 - 0.5*u2;

  P.x = w1*spX[i-1] + w2*spX[i] + w3*spX[i+1] + w4*spX[i+2];
  P.y = w1*spY[i-1] + w2*spY[i] + w3*spY[i+1] + w4*spY[i+2];
  P.z = w1*spZ[i-1] + w2*spZ[i] + w3*spZ[i+1] + w4*spZ[i+2];

  return P;

}

unsigned int trace_polygons(
Ray r
)
{
  int np;
  Polygon *p;
  double vd, v0, t;
  double t_u, t_v;
  Point P;
  double u[4], v[4];
  int i;
  int NC, SH, NSH;
  int a,b;
  Ray E;
  double d0, d1;

  /* ray-trace the polygons */
  for ( np=0 ; np<num_polys ; np++ ) {

//    printf("Tracing polygon %d\n", np);

    p = &(polys[np]);

    vd = dot_product(p->n, r.dir);
    if (vd == 0.0) continue; /* // to the plane */

    v0 = -(dot_product(p->n, r.org) + p->d);       
    t = v0/vd;
    if (t <= 0.001) continue; /* behind the ray origin */

    /* project onto U,V ignoring the dominant coefficient */
    /* also move the intersection point to (0,0) */
    P.x = r.org.x + t*r.dir.x;
    P.y = r.org.y + t*r.dir.y;
    P.z = r.org.z + t*r.dir.z;
    if (( fabs(p->n.x) > fabs(p->n.y) ) && ( fabs(p->n.x) > fabs(p->n.z) ) ) {
      for ( i=0 ; i<4 ; i++ ) {
        u[i] = p->v[i].y - P.y;
        v[i] = p->v[i].z - P.z;
      } 
    } else if ( fabs(p->n.y) > fabs(p->n.z) ) {
      for ( i=0 ; i<4 ; i++ ) {
        u[i] = p->v[i].x - P.x;
        v[i] = p->v[i].z - P.z;
      } 
    } else {
      for ( i=0 ; i<4 ; i++ ) {
        u[i] = p->v[i].x - P.x;
        v[i] = p->v[i].y - P.y;
      } 
    } 

    /* note - interpolate uv texture coordinates when crossing +x & -x */

    NC = 0;
    if (v[0] < 0.0) SH = -1;
    else            SH = 1;

    for ( a=0 ; a<4 ; a++ ) {
      b = (a+1) % 4;
      if (v[b] < 0.0) NSH = -1;
      else            NSH = 1;
      if (SH != NSH) {
        if ((u[a] > 0.0) && (u[b] > 0.0)) {
          NC++;
        } else if ((u[a] > 0.0) || (u[b] > 0.0)) {
          if ((u[a]-v[a]*(u[b]-u[a])/(v[b]-v[a])) > 0.0) {
            NC++;
          }
        }
        SH = NSH;
      }
    }

    if (NC % 2) {

//printf("Found a hit\n");

      /* it's in */
//    hit->u = hit->v = 0.0;
      return 255;

    }

  }

  return 0;
}

main()
{
  int i,j,c,p;
  double seg, seg_step;
  Point a;
  Point iP[4];
  Vector iV[4], iN[4], iC[4];
  Point Q[4];
  const double center = (OUT_XRES-1) / 2.0;
  const double radius2 = (OUT_XRES-1)*(OUT_XRES-1) / 4.0;
  Ray R;
  double cx, cy, tx, ty, dist, max_dist;
  double theta, phi, costheta, sintheta, cosphi, sinphi;
  
  get_texture();

  for ( c=0 ; c<NUM_SPLINES ; c++ ) {

    read_spline(c);

    seg_step = seg_step_size[c];
    for ( seg=1.0 ; seg<98.0 ; seg+=seg_step ) {

      printf("Spline %d segment %lf to %lf\n", c, seg, seg+seg_step);
      a = spline_point(seg+seg_step);
//      printf("  P=<%lf,%lf,%lf\n", a.x, a.y, a.z);

      iP[0] = spline_point(seg);
      iP[1] = spline_point(seg+seg_step);
      iP[2] = spline_point(seg+seg_step+seg_step);
      iP[3] = spline_point(seg+seg_step+seg_step+seg_step);

      iP[0] = multiply(6471.0, normalize(iP[0]));
      iP[1] = multiply(6471.0, normalize(iP[1]));
      iP[2] = multiply(6471.0, normalize(iP[2]));
      iP[3] = multiply(6471.0, normalize(iP[3]));

      iV[1] = normalize(subtract(iP[2], iP[0]));
      iV[2] = normalize(subtract(iP[3], iP[1]));

      iN[1] = normalize(iP[1]);
      iN[2] = normalize(iP[2]);

      iC[1] = cross(iV[1], iN[1]);
      iC[2] = cross(iV[2], iN[2]);

      /* the 4 corner points of lower bound of curtain */
      Q[0] = add(iP[1], multiply( CurtainWidth*0.5, iC[1]));
      Q[1] = add(iP[1], multiply(-CurtainWidth*0.5, iC[1]));
      Q[2] = add(iP[2], multiply(-CurtainWidth*0.5, iC[2]));
      Q[3] = add(iP[2], multiply( CurtainWidth*0.5, iC[2]));
//      printf("  Q0=<%lf,%lf,%lf\n", Q[0].x, Q[0].y, Q[0].z);
//      printf("  Q1=<%lf,%lf,%lf\n", Q[1].x, Q[1].y, Q[1].z);
//      printf("  Q2=<%lf,%lf,%lf\n", Q[2].x, Q[2].y, Q[2].z);
//      printf("  Q3=<%lf,%lf,%lf\n", Q[3].x, Q[3].y, Q[3].z);

    /* create u/v coordinates */
#if 0
      tmp0 = interp_uv(c, seg1);
      v0 = tmp0 - floor(tmp0);
      tmp1 = interp_uv(c, seg2);
      v1 = tmp1 - floor(tmp1);
#endif

      polys[num_polys].v[0] = Q[0];
      polys[num_polys].v[1] = Q[1];
      polys[num_polys].v[2] = Q[2];
      polys[num_polys].v[3] = Q[3];
      polys[num_polys].n  = normalize(cross(subtract(Q[1],Q[0]),subtract(Q[2],Q[0])));
      polys[num_polys].d  = dot_product( negate(Q[0]), polys[num_polys].n);
      num_polys ++;

    }

  }

  printf("Number of polys = %d\n", num_polys);

  for ( j=0 ; j<OUT_YRES; j++ ) {

    for ( i=0 ; i<OUT_XRES ; i++ ) {

      /* skip if outside circle */
      if (((i-center)*(i-center)+(j-center)*(j-center)) > radius2)
        continue;

      /* viewpint is at (0,0,0) */
      R.org.x = R.org.y = R.org.z = 0.0;

      /* determine Rdir - apex of dome is down -Z axis */
      if ((i*2)==(OUT_XRES-1) && (j*2)==(OUT_YRES-1)) {
        
        /* special case of exactly the apex */
        theta = 0.0; costheta = 1.0; sintheta = 0.0;
        phi = 0.0;   cosphi = 1.0;   sinphi = 0.0;
            
      } else {
        
        /* calculate vector from viewpoint to i,j on viewplane */
        cx = (OUT_XRES-1) / 2.0; tx = i - cx;
        cy = (OUT_YRES-1) / 2.0; ty = j - cy;
        dist = sqrt(tx*tx + ty*ty);
        max_dist = OUT_XRES-1 - cx;
              
        /* calculate angle down from the apex based on pixel[i,j] */
        /* constant is 90 * PI / 180 */
        theta = (dist/max_dist) * M_PI_2;
              
        /* calculate polar coordinates */
        costheta = cos(theta);
        sintheta = sin(theta);
        cosphi = tx / dist;
        sinphi = ty / dist;
            
      }
            
      /* create direction vector */
      R.dir.x = cosphi * sintheta;
      R.dir.y = costheta; /* down +Y axis */
      R.dir.z = sinphi * sintheta;

// printf("Pixel[%d][%d] = <%.2lf,%.2lf,%.2lf>+<%.2lf,%.2lf,%.2lf>t\n",
//        i, j, R.org.x, R.org.y, R.org.z, R.dir.x, R.dir.y, R.dir.z);

      raster[i+j*OUT_XRES] = (unsigned char) trace_polygons(R);


    }

  }

  write_image();

  exit(0);
}


