#ifndef _RAYS_HPP_
#define _RAYS_HPP_

extern const double SMALL_NUMBER;
extern const int NON_SINGULAR;
extern const int SINGULAR;
extern const double INF;
extern const double TOL;

#include <array>
#include <ostream>

typedef struct Vector Point; // assigning Point as an Alias of Vector
struct Vector {
    double x=0,y=0,z=0;

    Vector& operator-=(Point lhs);
    Vector& operator+=(Point lhs);
    Vector& operator*=(double c);
    Vector operator-();

    Vector& normalize();
    [[nodiscard]] Vector cross(const Vector & obj) const;
    [[nodiscard]] double dot_product(const Vector & obj) const;
    [[nodiscard]] double dist2(const Point & obj) const;
    [[nodiscard]] double distance(const Point & obj) const;
    [[nodiscard]] double mag() const;
};

struct Ray {
    Ray() = default;
    Ray(double x1,double y1, double z1, double x2,double y2,double z2);

    Point org;             /* position (origin) */
    Point dir;             /* direction (unitized) */
};

struct MATRIX {
    std::array<std::array<double,4>,4> m ={};
    MATRIX() = default;
    MATRIX(std::initializer_list<std::initializer_list<double>> init_list);

    auto operator[](int index)const ->decltype(m[index]);
    auto operator[](int index)->decltype(m[index]);
    void MatrixIdentity();
    [[nodiscard]] MATRIX adjoint()const;
    [[nodiscard]] double det4x4()const;

    [[nodiscard]] std::pair<MATRIX,int> inverse()const;

};

std::ostream& operator<<(std::ostream& os, const MATRIX & a);

// Alias Functions For Vector:
// Provide more functionality and backward compatibility
Vector operator*(Point obj, double c);
Vector operator*(double c, Point obj);
Vector operator-(Point P1, Point P2);
Vector operator+(Point P1, Point P2);

[[maybe_unused]] Vector negate(Vector V);
[[maybe_unused]] Vector add(Point P1, Point P2);
[[maybe_unused]] Vector subtract(Point P1, Point P2);
[[maybe_unused]] Vector multiply(double c, Point P);
[[maybe_unused]] Vector normalize(Vector V);
[[maybe_unused]] Vector cross(const Vector & a, const Vector & b);
[[maybe_unused]] double dot_product(const Vector & a, const Vector & b);
[[maybe_unused]] double dist2(const Point & a, const Point & b);
[[maybe_unused]] double distance(const Point & a, const Point & b);
[[maybe_unused]] double mag(const Point & P);
Vector create_vector(const Point & From, const Point & To);


// Alias Functions for Ray:
// Provide more functionality and backward compatibility
[[maybe_unused]] Ray make_ray(double x1,double y1, double z1,
                              double x2,double y2,double z2);

// Alias Functions for MATRIX:
// Provide more functionality and backward compatibility
[[maybe_unused]] void MatrixPrint(const MATRIX &a);
[[maybe_unused]] void MatrixIdentity(MATRIX & a);
[[maybe_unused]] void adjoint(const MATRIX & in, MATRIX & out);
[[maybe_unused]] double det4x4(const MATRIX & m);
[[maybe_unused]] int inverse(const MATRIX & in, MATRIX & out);

// Utility Functions:
int fcmp(double, double);
double det2x2( double a, double b, double c, double d);
double det3x3( double a1, double a2, double a3, double b1,
               double b2, double b3, double c1, double c2, double c3 );

Vector vector_times_3x3of4x4(const Vector & x, const MATRIX &M);
// Alias of vector_times_3x3of4x4: For backward compatibility
[[maybe_unused]] Point point_times_4x4(const Point & x, const MATRIX & M);

int VolumeBoxIntersect(
        double rorgx,
        double rorgy,
        double rorgz,
        double rdirx,
        double rdiry,
        double rdirz,
        Point b1,
        Point b2,
        double *tnear,
        double *tfar
);

#endif //_RAYS_HPP_
