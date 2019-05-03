#ifndef TRANGLE_SIMPLIFY_H_
#define TRANGLE_SIMPLIFY_H_
#include <array>
#include <vector>
#include <forward_list>
#include <iostream>
#include "vecex.h"

struct Point {
    double x, y, z;
    Point operator*(double a) const {
        return {a*x, a*y, a*z};
    }
    Point operator/(double a) const {
        return {x/a, y/a, z/a};
    }
    Point operator+(const Point& a) const {
        return {x + a.x, y + a.y, z + a.z};
    }
    Point operator-(const Point& a) const {
        return {x - a.x, y - a.y, z - a.z};
    }
    double dotPro(const Point& a) const {
        return x*a.x + y*a.y + z*a.z;
    }
};


struct TriangleNet {
    vecex<Point> vertexTab;
    using Triangle = std::array<size_t, 3>;
    vecex<Triangle> triangleTab;
    std::vector<std::forward_list<size_t>> vertexAdj;
    Point getNorm(size_t triangleID) const;
    Point getFoldPoint(size_t triangleID) const;
    double getVolumeError(size_t triangleID, const Point& foldPoint) const;
    double getArea(size_t triangleID) const;
    double getAngleError(size_t triangleID) const;
    void simplify(double s0);
    friend std::istream& operator>>(std::istream& input, TriangleNet& net);
    friend std::ostream& operator<<(std::ostream& output, TriangleNet& net);
};

#endif