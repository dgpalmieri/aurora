// view.hpp
// Refactored to C++17 by Dylan Palmieri @dgpalmieri
// Functions that set the viewing/raster attributes

#ifndef VIEW_H
#define VIEW_H

#include <vector>
#include "rays.hpp"

void setViewMatrix(const std::vector<double> &);

void outputCamera();

Point cameraTransform(const Point &);

Point invCameraTransform(const Point &);

Vector vecCameraTransform(const Vector &);

Vector vecInvCameraTransform(const Vector &);

double cameraLookup(const int &, const int &);

#endif // VIEW_H
