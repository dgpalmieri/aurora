/*typedef struct{ double x, y, z; } Vector; */
#include "rays.h"

void   initnoise(void);
double noise(Vector p);
Vector Dnoise(Vector p);
double turbulence(double x, double y, double z, double size);

