// view.cpp
// Refactored to C++ by Dylan Palmieri (@dgpalmieri)

#include <iostream>
#include <cmath>
#include "view.hpp"

// transformation to eye ray
MATRIX viewMatrix = {{1.,0.,0.,0.},{0.,1.,0.,0.},{0.,0.,1.,0.},{0.,0.,0.,1.}};

// inverse of transform to eye ray
// This gets set only during the inverse() call in setViewMatrix if the
//     matrix is singular (invertable)
MATRIX viewInverse = {{1.,0.,0.,0.},{0.,1.,0.,0.},{0.,0.,1.,0.},{0.,0.,0.,1.}};

void setViewMatrix(const std::vector<double> & avs_camera) {
    // copy the view matrix over, note that the viewpoint
    // is at (0, 0, 12) facing down the negative Z axis
    for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
        viewMatrix[i][j] = avs_camera[i * 4 + j];

    // invert the matrix so view can be moved instead of moving objects
    if (inverse(viewMatrix, viewInverse) == NON_SINGULAR) {
        std::cerr << "Camera transformation is non-singular - IGNORING" << std::endl;
        MatrixIdentity(viewInverse);
    }
}

void outputCamera() {
    std::cerr << "Camera transformation " << std::endl;
    MatrixPrint(viewMatrix);
    std::cerr << "Inverse camera transformation " << std::endl;
    MatrixPrint(viewInverse);
}


Point cameraTransform(const Point & p) {
    Point a;

    a.x = p.x*viewMatrix[0][0]+p.y*viewMatrix[1][0]+p.z*viewMatrix[2][0]+viewMatrix[3][0];
    a.y = p.x*viewMatrix[0][1]+p.y*viewMatrix[1][1]+p.z*viewMatrix[2][1]+viewMatrix[3][1];
    a.z = p.x*viewMatrix[0][2]+p.y*viewMatrix[1][2]+p.z*viewMatrix[2][2]+viewMatrix[3][2];

    return a;
}

Point invCameraTransform(const Point & p) {
    Point a;

    a.x = p.x*viewInverse[0][0]+p.y*viewInverse[1][0]+p.z*viewInverse[2][0]+viewInverse[3][0];
    a.y = p.x*viewInverse[0][1]+p.y*viewInverse[1][1]+p.z*viewInverse[2][1]+viewInverse[3][1];
    a.z = p.x*viewInverse[0][2]+p.y*viewInverse[1][2]+p.z*viewInverse[2][2]+viewInverse[3][2];

    return a;
}

Vector vecCameraTransform(const Vector & p) {
    Vector a;

    a.x = p.x*viewMatrix[0][0]+p.y*viewMatrix[1][0]+p.z*viewMatrix[2][0];
    a.y = p.x*viewMatrix[0][1]+p.y*viewMatrix[1][1]+p.z*viewMatrix[2][1];
    a.z = p.x*viewMatrix[0][2]+p.y*viewMatrix[1][2]+p.z*viewMatrix[2][2];

    return a;
}

Vector vecInvCameraTransform(const Vector & p) {
    Vector a;

    a.x = p.x*viewInverse[0][0]+p.y*viewInverse[1][0]+p.z*viewInverse[2][0];
    a.y = p.x*viewInverse[0][1]+p.y*viewInverse[1][1]+p.z*viewInverse[2][1];
    a.z = p.x*viewInverse[0][2]+p.y*viewInverse[1][2]+p.z*viewInverse[2][2];

    return a;
}

double cameraLookup(const int & i, const int & j) {
    return viewMatrix[i][j];
}
