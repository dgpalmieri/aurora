/*      File: view.h                                                       */


/* Functions that set the viewing/raster attributes */

#ifndef _VIEW_H
#define _VIEW_H

#include <vector>
#include "rays.hpp"

void setViewMatrix(const std::vector<double> &);

void outputCamera();

Point cameraXform(const Point &);

Point invCameraXform(const Point &);

Vector vecCameraXform(const Vector &);

Vector vecInvCameraXform(const Vector &);

double cameraLookup(const int &, const int &);

#endif // _VIEW_H
