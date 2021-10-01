typedef struct{ double x, y, z; } Vector;

void   initnoise(void);
double noise(Vector p);
Vector Dnoise(Vector p);

