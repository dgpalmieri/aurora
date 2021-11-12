/*typedef struct{ double x, y, z; } Vector; */
#include "rays.hpp"

void   initnoise(void);
double noise(Vector p);
Vector Dnoise(Vector p);
double turbulence(double x, double y, double z, double size);

