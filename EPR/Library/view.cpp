/*      File: view.c                                                       */


/* set_polar_view ... */

#include <stdio.h>
#include <math.h>
#include "view.h"

/* transformation to eye ray */
MATRIX xform={1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.};

/* inverse of transform to eye ray  */
MATRIX invrse={1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.,0.,0.,0.,0.,1.};

/*-------------------------------------------------------------------*/

void set_view_matrix(
double *avs_camera
)
{
  int i,j;

  /* copy the view matrix over, note that the viewpoint */
  /* is at (0,0,12) facing down negative Z  */
  for (i=0 ; i<4 ; i++)
    for (j=0 ; j<4 ; j++)
      xform[i][j] = *(avs_camera + (i*4+j));

  /* invert the matrix so view can be moved instead of moving objects */
  if (inverse(xform, invrse) == NON_SINGULAR) {
    fprintf(stderr,"Camera transformation is non-singular - IGNORING\n");
    fflush(stderr);
    MatrixIdentity(invrse);
  }

}

/*-------------------------------------------------------------------*/

void output_camera(
)
{
  fprintf(stderr, "Camera transformation ");
  MatrixPrint(xform);
  fprintf(stderr, "Inverse Camera transformation ");
  MatrixPrint(invrse);
}

/*-------------------------------------------------------------------*/

Point camera_xform(
Point p
)
{
  Point a;

  /* pre-multiply the matrix (PxM) */
  a.x = p.x*xform[0][0]+p.y*xform[1][0]+p.z*xform[2][0]+xform[3][0];
  a.y = p.x*xform[0][1]+p.y*xform[1][1]+p.z*xform[2][1]+xform[3][1];
  a.z = p.x*xform[0][2]+p.y*xform[1][2]+p.z*xform[2][2]+xform[3][2];

  return a;
}

/*-------------------------------------------------------------------*/

Point inv_camera_xform(
Point p
)
{
  Point a;

  /* pre-multiply the inverse camera matrix (PxM-1) */
  a.x = p.x*invrse[0][0]+p.y*invrse[1][0]+p.z*invrse[2][0]+invrse[3][0];
  a.y = p.x*invrse[0][1]+p.y*invrse[1][1]+p.z*invrse[2][1]+invrse[3][1];
  a.z = p.x*invrse[0][2]+p.y*invrse[1][2]+p.z*invrse[2][2]+invrse[3][2];

  return a;
}

/*-------------------------------------------------------------------*/

Vector vec_camera_xform(
Vector p
)
{
  Vector a;

  /* pre-multiply the inverse camera matrix (PxM) */
  a.x = p.x*xform[0][0]+p.y*xform[1][0]+p.z*xform[2][0];
  a.y = p.x*xform[0][1]+p.y*xform[1][1]+p.z*xform[2][1];
  a.z = p.x*xform[0][2]+p.y*xform[1][2]+p.z*xform[2][2];

  return a;
}

/*-------------------------------------------------------------------*/

Vector vec_inv_camera_xform(
Vector p
)
{
  Vector a;

  /* pre-multiply the inverse camera matrix (PxM-1) */
  a.x = p.x*invrse[0][0]+p.y*invrse[1][0]+p.z*invrse[2][0];
  a.y = p.x*invrse[0][1]+p.y*invrse[1][1]+p.z*invrse[2][1];
  a.z = p.x*invrse[0][2]+p.y*invrse[1][2]+p.z*invrse[2][2];

  return a;
}

/*-------------------------------------------------------------------*/

double camera_lookup(
int i,
int j
)
{
  return xform[i][j];
}

/*-------------------------------------------------------------------*/

