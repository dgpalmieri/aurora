/*      File: view.h                                                       */


/* Functions that set the viewing/raster attributes */

#ifndef _VIEW_H
#define _VIEW_H

#include <vector>
#include "rays.hpp"

void setViewMatrix(const std::vector<double> &);

void outputCamera();

Point cameraTransform(const Point &);

Point invCameraTransform(const Point &);

Vector vecCameraTransform(const Vector &);

Vector vecInvCameraTransform(const Vector &);

double cameraLookup(const int &, const int &);

#endif // _VIEW_H
