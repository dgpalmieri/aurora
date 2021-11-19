/*      File: view.h                                                       */


/* Functions that set the viewing/raster attributes */

#ifndef _VIEW_H

#define _VIEW_H

#include "rays.hpp"

void  set_view_matrix(double *);
void  output_camera(void);
Point camera_xform(Point);
Point inv_camera_xform(Point);
Vector vec_camera_xform(Vector);
Vector vec_inv_camera_xform(Vector);
double camera_lookup(int,int);

#endif
