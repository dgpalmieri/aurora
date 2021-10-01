/*      File: rays.h                                                       */


/* Functions that manipulate rays, vectors, points and matrices */

#ifndef _RAYS_H

#define _RAYS_H

#define NON_SINGULAR    1

/* primitive types */
typedef struct {
  double x,y,z;          /* direction in x,y,z directions */
} Vector, Point;

typedef struct {
  Point org;             /* position (origin) */
  Point dir;             /* direction (unitized) */
} Ray;

typedef double MATRIX[4][4];

/* macros definitions */
#define DOT_PRODUCT(V1,V2) ((V1).x*(V2).x + (V1).y*(V2).y + (V1).z*(V2).z)

/* functions to manipulate rays/vectors */
Vector normalize(Vector);
Vector negate(Vector);
Vector cross(Vector, Vector);
Vector add(Point, Point);
Vector subtract(Point, Point);
Vector multiply(double, Point);
double dot_product(Vector, Vector);
double distance(Point, Point);
double mag(Point);
Ray    make_ray(double, double, double, double, double, double);
Vector create_vector(Point, Point);
int    fcmp(double, double);
int    VolumeBoxIntersect(double, double, double, double, double, double,
                          Point, Point, double *, double *);

/* functions to manipulate matrices */
void   MatrixPrint(MATRIX);
void   MatrixIdentity(MATRIX);
int    inverse(MATRIX,MATRIX);
void   adjoint(MATRIX,MATRIX);
double det4x4(MATRIX);
double det3x3(double,double,double,double,double,double,
              double,double,double);
double det2x2(double,double,double,double);

/* functions to manipulate rays/matrices */
Point  point_times_4x4(Point, MATRIX);
Vector vector_times_3x3of4x4(Vector, MATRIX);

#endif
