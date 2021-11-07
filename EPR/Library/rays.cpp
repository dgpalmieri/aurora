#include <math.h>
#include <stdexcept>
#include <iostream>
#include <iomanip>

#include "rays.hpp"

const double SMALL_NUMBER = 1.0e-8;
const int NON_SINGULAR = 1;
const int SINGULAR = 0;
const double INF = 1.0e+38;
const double TOL = 0.0001;

// ************************* Definition for Vector ***********************
Vector & Vector::operator-=(Point rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

Vector &Vector::operator+=(Point lhs) {
    x += lhs.x;
    y += lhs.y;
    z += lhs.z;
    return *this;
}

Vector &Vector::operator*=(double c) {
    x *=c;
    y *=c;
    z *=c;
    return *this;
}

Vector Vector::operator-() {
    return (*this *= -1.0);
}

Vector& Vector::normalize(){
    double sc = 1.0 / sqrt(x*x + y*y + z*z);
    return *this *=sc;
}

Vector Vector::cross(const Vector & obj) const {
    Vector c;

    c.x = y*obj.z - z*obj.y;
    c.y = z*obj.x - x*obj.z;
    c.z = x*obj.y - y*obj.x;

    return c;
}

double Vector::dot_product(const Vector &obj) const {
    return (x * obj.x + y * obj.y + z * obj.z);
}

double Vector::dist2(const Point &obj) const {
    Point d = *this - obj;
    return ((d.x*d.x)+(d.y*d.y)+(d.z*d.z));
}

double Vector::distance( const Point & obj) const {
    return sqrt(dist2(obj));
}

double Vector::mag() const {
    return sqrt(x*x + y*y + z*z);
}


//******************* Alias Functions For Vector Definition: ***************
Vector operator-(Point P1, Point P2) {
    return P1 -=P2;
}

Vector operator+(Point P1, Point P2) {
    return P1 +=P2;
}

Vector operator*(double c, Point obj) {
    return obj *=c;
}

Vector operator*(Point obj, double c) {
    return obj *= c;
}

Vector negate(Vector V) {
    return -V;
}
Vector add(Point P1, Point P2) {
    return P1 + P2;
}

Vector subtract(Point P1, Point P2) {
    return P1 - P2;
}

Vector multiply(double c, Point P) {
    return P * c;
}

Vector normalize(Vector V){
    return V.normalize();
}

Vector cross(const Vector & a, const Vector & b){
    return a.cross(b);
}

double dot_product(const Vector & a, const Vector & b){
    return (a.dot_product(b));
}

double dist2(const Point & a, const Point & b){
    return a.dist2(b);
}

double distance(const Point & a, const Point & b){
    return a.distance(b);
}

double mag(const Point & P){
    return P.mag();
}

Vector create_vector(const Point & From, const Point & To){
    Vector v = To - From;
    double mag = 1.0 / v.mag();
    v *= mag;
    return v;
}

// ************************ End of Definition for Vector **********************

// ************************* Definition for Ray ***********************
Ray::Ray(double x1, double y1, double z1, double x2, double y2, double z2) {
    double sc;
    double a = x2 - x1;
    double b = y2 - y1;
    double c = z2 - z1;

    org = Vector {x1,y1,z1};
    sc = 1.0 / sqrt(a*a + b*b + c*c);
    dir = Vector {sc * a, sc * b, sc * c};
}

//******************* Alias Functions For Ray Definition: ***************
Ray make_ray(double x1,double y1, double z1, double x2,double y2,double z2) {
    Ray r {x1, y1, z1, x2, y2, z2};
    return r;
}
// ************************ End of Definition for Ray **********************

// ************************* Definition for MATRIX ***********************
MATRIX::MATRIX(std::initializer_list<std::initializer_list<double>> init_list) {
    int counter = 0;
    for (auto row : init_list)
        std::copy(row.begin(), row.end(), m[counter++].begin());

}
auto MATRIX::operator[](int indx)const -> decltype(m[indx]) {
    if (indx >3) throw std::out_of_range
    ("Index must not be greater than the length of MATRIX: 4x4");

    return m[indx];
}
auto MATRIX::operator[](int indx)-> decltype(m[indx]) {
    if (indx >3) throw std::out_of_range
                ("Index must not be greater than the length of MATRIX: 4x4");

    return m[indx];
}

std::ostream &operator<<(std::ostream &os, const MATRIX & a) {

    os<< "Matrix is:\n";
    for ( int i=0 ; i<4 ; i++ ) {
        for ( int j=0 ; j<4 ; j++ )
            os<< std::setprecision(2) <<a.m[i][j]<<" ";
        os<< "\n";
    }
    return os;
}

void MATRIX::MatrixIdentity() {

    for ( int i=0 ; i<4 ; i++ ) {
        for (int j = 0; j < 4; j++)
            m[i][j] = (double) (i == j);
    }
}

MATRIX MATRIX::adjoint() const {
    double a1, a2, a3, a4, b1, b2, b3, b4;
    double c1, c2, c3, c4, d1, d2, d3, d4;
    MATRIX out;
    /* assign to individual variable names to aid  */
    /* selecting correct values  */

    a1 = m[0][0]; b1 = m[0][1]; c1 = m[0][2]; d1 = m[0][3];
    a2 = m[1][0]; b2 = m[1][1]; c2 = m[1][2]; d2 = m[1][3];
    a3 = m[2][0]; b3 = m[2][1]; c3 = m[2][2]; d3 = m[2][3];
    a4 = m[3][0]; b4 = m[3][1]; c4 = m[3][2]; d4 = m[3][3];

    /* row column labeling reversed since we transpose rows & columns */

    out[0][0]  =   det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
    out[1][0]  = - det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
    out[2][0]  =   det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
    out[3][0]  = - det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);

    out[0][1]  = - det3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
    out[1][1]  =   det3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
    out[2][1]  = - det3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
    out[3][1]  =   det3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);

    out[0][2]  =   det3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
    out[1][2]  = - det3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
    out[2][2]  =   det3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
    out[3][2]  = - det3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);

    out[0][3]  = - det3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
    out[1][3]  =   det3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
    out[2][3]  = - det3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
    out[3][3]  =   det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);

    return out;
}

double MATRIX::det4x4() const {
    double ans;
    double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

    /* assign to individual variable names to aid selecting */
    /*  correct elements */

    a1 = m[0][0]; b1 = m[0][1]; c1 = m[0][2]; d1 = m[0][3];
    a2 = m[1][0]; b2 = m[1][1]; c2 = m[1][2]; d2 = m[1][3];
    a3 = m[2][0]; b3 = m[2][1]; c3 = m[2][2]; d3 = m[2][3];
    a4 = m[3][0]; b4 = m[3][1]; c4 = m[3][2]; d4 = m[3][3];

    ans = a1 * det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
          - b1 * det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4)
          + c1 * det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
          - d1 * det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
    return ans;
}

std::pair<MATRIX, int> MATRIX::inverse() const {
    int i, j;
    double det;

    MATRIX out = adjoint();

    det = det4x4();

    if ( fabs( det ) < SMALL_NUMBER) return {out, NON_SINGULAR };

    /* scale the adjoint matrix to get the inverse */
    for (i=0; i<4; i++)
        for(j=0; j<4; j++)
            out[i][j] = out[i][j] / det;

    return {out, SINGULAR };
}


//******************* Alias Functions For MATRIX Definition: ***************
void MatrixPrint(const MATRIX & a) {
    std::cout<<a;
}

void MatrixIdentity(MATRIX &a) {
    a.MatrixIdentity();
}

void adjoint(const MATRIX & in, MATRIX &out) {
    out = in.adjoint();
}
double det4x4(const MATRIX & m) {
    return m.det4x4();
}

int inverse(const MATRIX & in,MATRIX &out) {
    auto inv = in.inverse();
    out = inv.first;
    return inv.second;
}
// ********************** End of Definition for MATRIX ******************

//*********************** Definition for Utility Functions **************
int fcmp(double a,double b) {
    if (fabs(a - b) < TOL) return 1;
    return 0;
}

double det2x2( double a, double b, double c, double d) {
    return a * d - b * c;
}

double det3x3( double a1, double a2, double a3, double b1,
               double b2, double b3, double c1, double c2, double c3 ) {

    double ans;

    ans = a1 * det2x2( b2, b3, c2, c3 )
          - b1 * det2x2( a2, a3, c2, c3 )
          + c1 * det2x2( a2, a3, b2, b3 );
    return ans;
}

Vector vector_times_3x3of4x4(const Vector & x, const MATRIX & M) {
    Point a;

    a.x = x.x*M[0][0] + x.y*M[1][0] + x.z*M[2][0] + M[3][0];
    a.y = x.x*M[0][1] + x.y*M[1][1] + x.z*M[2][1] + M[3][1];
    a.z = x.x*M[0][2] + x.y*M[1][2] + x.z*M[2][2] + M[3][2];

    return a;
}

Point point_times_4x4(const Point & x, const MATRIX & M) {
    return vector_times_3x3of4x4(x,M);
}

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
){
    double t1, t2, tswap, dinv;

    /* check X slabs */
    if (fcmp(rdirx,0.0)) {

        /* ray is parallel to X planes of box */
        if ( !(((b1.x <= rorgx ) && ( rorgx <= b2.x)) ||
               ((b2.x <= rorgx ) && ( rorgx <= b1.x))))
            return 0; /* but not between them */
        *tfar = INF;
        *tnear = -INF;

    } else {

        dinv = 1.0/rdirx;
        t1 = (b1.x - rorgx) * dinv;
        t2 = (b2.x - rorgx) * dinv;

        if (t1 > t2) {
            *tfar = t1;
            *tnear = t2;
        } else {
            *tfar = t2;
            *tnear = t1;
        }

        /* box is behind ray */
        if ( *tfar < 0.0 ) return 0;

    }

    /* check Y slabs */
    if (fcmp(rdiry,0.0)) {
        /* ray is parallel to Y planes of box */
        if (!(((b1.y <= rorgy ) && (rorgy <= b2.y)) ||
              ((b2.y <= rorgy ) && (rorgy <= b1.y))))
            return 0; /* but not between them */
    } else {

        dinv = 1.0 / rdiry;
        t1 = (b1.y-rorgy) * dinv;
        t2 = (b2.y-rorgy) * dinv;

        if (t1>t2) {
            tswap = t1;
            t1 = t2;
            t2 = tswap;
        }

        if (t1 > *tnear) *tnear = t1;
        if (t2 < *tfar)  *tfar = t2;

        /* box is missed */
        if (*tnear > *tfar) return 0;

        /* box is behind ray */
        if (*tfar < 0.0) return 0;
    }

    /* check Z slabs */
    if (fcmp(rdirz,0.0)) {
        /* ray is parallel to Z planes of box */
        if (!(((b1.z <= rorgz ) && (rorgz <= b2.z)) ||
              ((b2.z <= rorgz ) && (rorgz <= b1.z))))
            return 0; /* but not between them */
    } else {

        dinv = 1.0/rdirz;
        t1 = (b1.z-rorgz)*dinv;
        t2 = (b2.z-rorgz)*dinv;

        if (t1 > t2) {
            tswap = t1;
            t1 = t2;
            t2 = tswap;
        }

        if (t1 > *tnear) *tnear = t1;
        if (t2 < *tfar)  *tfar = t2;

        /* box is missed */
        if (*tnear > *tfar) return 0;

        /* box is behind ray */
        if (*tfar < 0.0) return 0;

    }

    return 1;
}