#ifndef TRANGLE_SIMPLIFY_H_
#define TRANGLE_SIMPLIFY_H_
#include <array>
#include <vector>
#include <forward_list>
#include <iostream>
#include "vecex.h"

struct Point {
    double x, y, z;
};

struct TriangleNet {
    vecex<Point> vertexTab;
    using Triangle = std::array<size_t, 3>;
    vecex<Triangle> triangleTab;
    std::vector<std::forward_list<size_t>> vertexAdj;
    Point getFoldPoint(size_t triangleID) const;
    double getVolumeError(size_t triangleID) const;
    double getMaxArea(size_t triangleID) const;
    double getAngleError(size_t triangleID) const;
    void simplify(double v0, double s0, double a0);
    friend std::istream& operator>>(std::istream& input, TriangleNet& net);
    friend std::ostream& operator<<(std::ostream& output, TriangleNet& net);
};

#endif