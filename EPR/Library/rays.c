/*      File: rays.c                                                       */


#include <stdio.h>
#include <math.h>
#include "rays.h"

#define SMALL_NUMBER    1.0e-8
#define SINGULAR        0
#define INF             1.0e+38
#define TOL             0.0001

/*-------------------------------------------------------------------*/

Vector negate(V)
Vector V;
{
  Vector N;

  N.x = (-V.x);
  N.y = (-V.y);
  N.z = (-V.z);

  return N;
}

/*-------------------------------------------------------------------*/

Vector add(Point P1, Point P2)
{ 
  Vector P;

  P.x = P1.x + P2.x;
  P.y = P1.y + P2.y;         
  P.z = P1.z + P2.z;

  return P;
}

/*-------------------------------------------------------------------*/

Vector subtract(Point P1, Point P2)
{ 
  Vector P;

  P.x = P1.x - P2.x;
  P.y = P1.y - P2.y;         
  P.z = P1.z - P2.z;

  return P;
}

/*-------------------------------------------------------------------*/

Vector multiply(double c, Point P1)
{ 
  Vector P;

  P.x = c * P1.x;
  P.y = c * P1.y;         
  P.z = c * P1.z;

  return P;
}

/*-------------------------------------------------------------------*/

Vector normalize(V)
Vector V;
{
  Vector R;
  double sc = 1.0 / sqrt(V.x*V.x + V.y*V.y + V.z*V.z);

  R.x = sc*V.x;
  R.y = sc*V.y;
  R.z = sc*V.z;

  return R;
}

/*-------------------------------------------------------------------*/

Vector cross(
Vector a,
Vector b
)
{
   Vector c;

   c.x = a.y*b.z - a.z*b.y;
   c.y = a.z*b.x - a.x*b.z;
   c.z = a.x*b.y - a.y*b.x;

   return c;
}

/*-------------------------------------------------------------------*/

double dot_product(
Vector V1,
Vector V2
)
{
  return ( V1.x * V2.x + V1.y * V2.y + V1.z * V2.z );
}

/*-------------------------------------------------------------------*/

double dist2(
Point a,
Point b
)
{
  double dx = a.x - b.x;
  double dy = a.y - b.y;
  double dz = a.z - b.z;
  return ((dx*dx)+(dy*dy)+(dz*dz));
}

/*-------------------------------------------------------------------*/

double distance(
Point a,
Point b
)
{
  double dx = a.x - b.x;
  double dy = a.y - b.y;
  double dz = a.z - b.z;
  return (sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

/*-------------------------------------------------------------------*/

double mag(
Point p
)
{
  return sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
} 
  
/*-------------------------------------------------------------------*/
  
Vector create_vector(
Vector from,
Vector to
)
{
  Vector v;
  double mag;

  v.x = to.x - from.x;
  v.y = to.y - from.y;
  v.z = to.z - from.z;
  mag = 1.0 / sqrt(v.x*v.x + v.y*v.y + v.z*v.z); 
  v.x *= mag;
  v.y *= mag;
  v.z *= mag;

  return v;
}

/*-------------------------------------------------------------------*/

Ray make_ray(x1,y1,z1,x2,y2,z2)
double x1,y1,z1,x2,y2,z2;
{
  Ray R;
  double sc;
  double a = x2 - x1;
  double b = y2 - y1;
  double c = z2 - z1;
  
  R.org.x = x1;
  R.org.y = y1;
  R.org.z = z1;
  sc = 1.0 / sqrt(a*a + b*b + c*c);
  R.dir.x = sc * a;
  R.dir.y = sc * b;
  R.dir.z = sc * c;

  return R;
}
/*-------------------------------------------------------------------*/

int fcmp(a,b)
double a,b;
{
   if (fabs(a - b) < TOL) return 1; 
   return 0;
}

/*-------------------------------------------------------------------*/

void MatrixPrint(a)
MATRIX a;
{
  int i,j;

  fprintf(stderr, "Matrix is:\n");
  for ( i=0 ; i<4 ; i++ ) {
    for ( j=0 ; j<4 ; j++ )
      fprintf(stderr, "%.2lf ",a[i][j]);
    fprintf(stderr, "\n");
  }
  fflush(stderr);
}

/*-------------------------------------------------------------------*/

void MatrixIdentity(a)
MATRIX a;
{
  int i,j;

  for ( i=0 ; i<4 ; i++ )
    for ( j=0 ; j<4 ; j++ )
      a[i][j] = (double) (i == j);
}

/*-------------------------------------------------------------------*/

/* Matrix Inversion by Richard Carling from "Graphics Gems" */

/* 
 *    calculate the inverse of a 4x4 matrix
 *
 *     -1     
 *     A  = ___1__ adjoint A
 *         det A
 */

int inverse(in, out)
MATRIX in, out;
{
  int i, j;
  double det;

  adjoint( in, out );

  det = det4x4( in );

  if ( fabs( det ) < SMALL_NUMBER) return ( NON_SINGULAR );

  /* scale the adjoint matrix to get the inverse */
  for (i=0; i<4; i++)
    for(j=0; j<4; j++)
      out[i][j] = out[i][j] / det;

  return ( SINGULAR );
}

/*-------------------------------------------------------------------*/

/* 
 *     calculate the adjoint of a 4x4 matrix
 *
 *      Let  a   denote the minor determinant of matrix A obtained by
 *           ij
 *
 *      deleting the ith row and jth column from A.
 *
 *                    i+j
 *     Let  b   = (-1)    a
 *          ij            ji
 *
 *    The matrix B = (b  ) is the adjoint of A
 *                     ij
 */

void adjoint( in, out )
MATRIX in; MATRIX out;
{
  double a1, a2, a3, a4, b1, b2, b3, b4;
  double c1, c2, c3, c4, d1, d2, d3, d4;

  /* assign to individual variable names to aid  */
  /* selecting correct values  */

  a1 = in[0][0]; b1 = in[0][1]; c1 = in[0][2]; d1 = in[0][3];
  a2 = in[1][0]; b2 = in[1][1]; c2 = in[1][2]; d2 = in[1][3];
  a3 = in[2][0]; b3 = in[2][1]; c3 = in[2][2]; d3 = in[2][3];
  a4 = in[3][0]; b4 = in[3][1]; c4 = in[3][2]; d4 = in[3][3];

  /* row column labeling reversed since we transpose rows & columns */

  out[0][0]  =   det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
  out[1][0]  = - det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
  out[2][0]  =   det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
  out[3][0]  = - det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);

  out[0][1]  = - det3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
  out[1][1]  =   det3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
  out[2][1]  = - det3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
  out[3][1]  =   det3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);
       
  out[0][2]  =   det3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
  out[1][2]  = - det3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
  out[2][2]  =   det3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
  out[3][2]  = - det3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);
        
  out[0][3]  = - det3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
  out[1][3]  =   det3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
  out[2][3]  = - det3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
  out[3][3]  =   det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

/*-------------------------------------------------------------------*/

double det4x4(m)
MATRIX m;
{
  double ans;
  double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

  /* assign to individual variable names to aid selecting */
  /*  correct elements */

  a1 = m[0][0]; b1 = m[0][1]; c1 = m[0][2]; d1 = m[0][3];
  a2 = m[1][0]; b2 = m[1][1]; c2 = m[1][2]; d2 = m[1][3];
  a3 = m[2][0]; b3 = m[2][1]; c3 = m[2][2]; d3 = m[2][3];
  a4 = m[3][0]; b4 = m[3][1]; c4 = m[3][2]; d4 = m[3][3];

  ans = a1 * det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
      - b1 * det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4)
      + c1 * det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
      - d1 * det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
  return ans;
}

/*-------------------------------------------------------------------*/

/*
 * calculate the determinent of a 3x3 matrix
 * in the form
 *
 *     | a1,  b1,  c1 |
 *     | a2,  b2,  c2 |
 *     | a3,  b3,  c3 |
 */

double det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3 )
double a1, a2, a3, b1, b2, b3, c1, c2, c3;
{
  double ans;

  ans = a1 * det2x2( b2, b3, c2, c3 )
      - b1 * det2x2( a2, a3, c2, c3 )
      + c1 * det2x2( a2, a3, b2, b3 );
  return ans;
}

/*-------------------------------------------------------------------*/

double det2x2( a, b, c, d)
double a, b, c, d; 
{
  return a * d - b * c;
}

/*-------------------------------------------------------------------*/

Point point_times_4x4(x,M)
Point x;
MATRIX M;
{
  Point a;

  a.x = x.x*M[0][0] + x.y*M[1][0] + x.z*M[2][0] + M[3][0];
  a.y = x.x*M[0][1] + x.y*M[1][1] + x.z*M[2][1] + M[3][1];
  a.z = x.x*M[0][2] + x.y*M[1][2] + x.z*M[2][2] + M[3][2];

  return a;
}

/*-------------------------------------------------------------------*/

Vector vector_times_3x3of4x4(x,M)
Vector x;
MATRIX M;
{
  Point a;

  a.x = x.x*M[0][0] + x.y*M[1][0] + x.z*M[2][0] + M[3][0];
  a.y = x.x*M[0][1] + x.y*M[1][1] + x.z*M[2][1] + M[3][1];
  a.z = x.x*M[0][2] + x.y*M[1][2] + x.z*M[2][2] + M[3][2];

  return a;
}

/*--------------------------------------------------------------------*/

/* algorithm found in "An Introduction to Ray Tracing" on page 65 */
#define INF 1.0e+38

int VolumeBoxIntersect(
double rorgx,
double rorgy,
double rorgz,
double rdirx,
double rdiry,
double rdirz,
Point b1,
Point b2,
double *tnear,
double *tfar
)
{
  double t1, t2, tswap, dinv;

  /* check X slabs */
  if (fcmp(rdirx,0.0)) {

    /* ray is parallel to X planes of box */
    if ( !(((b1.x <= rorgx ) && ( rorgx <= b2.x)) ||
           ((b2.x <= rorgx ) && ( rorgx <= b1.x))))
      return 0; /* but not between them */
    *tfar = INF;
    *tnear = -INF;

  } else {

    dinv = 1.0/rdirx;
    t1 = (b1.x - rorgx) * dinv;
    t2 = (b2.x - rorgx) * dinv;

    if (t1 > t2) {
      *tfar = t1;
      *tnear = t2;
    } else {
      *tfar = t2;
      *tnear = t1;
    }

    /* box is behind ray */
    if ( *tfar < 0.0 ) return 0;

  }


  /* check Y slabs */
  if (fcmp(rdiry,0.0)) {
    /* ray is parallel to Y planes of box */
    if (!(((b1.y <= rorgy ) && (rorgy <= b2.y)) ||
          ((b2.y <= rorgy ) && (rorgy <= b1.y))))
      return 0; /* but not between them */
  } else {

    dinv = 1.0 / rdiry;
    t1 = (b1.y-rorgy) * dinv;
    t2 = (b2.y-rorgy) * dinv;

    if (t1>t2) {
      tswap = t1;
      t1 = t2;
      t2 = tswap;
    }

    if (t1 > *tnear) *tnear = t1;
    if (t2 < *tfar)  *tfar = t2;

    /* box is missed */
    if (*tnear > *tfar) return 0;

    /* box is behind ray */
    if (*tfar < 0.0) return 0;
  }

  /* check Z slabs */
  if (fcmp(rdirz,0.0)) {
    /* ray is parallel to Z planes of box */
    if (!(((b1.z <= rorgz ) && (rorgz <= b2.z)) ||
          ((b2.z <= rorgz ) && (rorgz <= b1.z))))
      return 0; /* but not between them */
  } else {

    dinv = 1.0/rdirz;
    t1 = (b1.z-rorgz)*dinv;
    t2 = (b2.z-rorgz)*dinv;

    if (t1 > t2) {
      tswap = t1;
      t1 = t2;
      t2 = tswap;
    }

    if (t1 > *tnear) *tnear = t1;
    if (t2 < *tfar)  *tfar = t2;

    /* box is missed */
    if (*tnear > *tfar) return 0;

    /* box is behind ray */
    if (*tfar < 0.0) return 0;

  }

  return 1;
}

/*--------------------------------------------------------------------*/

