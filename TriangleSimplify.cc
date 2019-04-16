#include <set>
#include <string>
#include <cmath>
#include "TriangleSimplify.h"
using namespace std;

Point TriangleNet::getFoldPoint(size_t trid) const {
    const Point& a = vertexTab.vec[triangleTab.vec[trid][0]];
    const Point& b = vertexTab.vec[triangleTab.vec[trid][1]];
    const Point& c = vertexTab.vec[triangleTab.vec[trid][2]];
    double nx = (a.x + b.x + c.x)/3;
    double ny = (a.y + b.y + c.y)/3;
    double nz = (a.z + b.z + c.z)/3;
    return  {nx, ny, nz};
}

double TriangleNet::getVolumeError(size_t trid) const {
    const Point& a = vertexTab.vec[triangleTab.vec[trid][0]];
    const Point& b = vertexTab.vec[triangleTab.vec[trid][1]];
    const Point& c = vertexTab.vec[triangleTab.vec[trid][2]];
    Point ab{a.x - b.x, a.y - b.y, a.z - b.z};
    Point ac{a.x - c.x, a.y - c.y, a.z - c.z};
    double ii = ab.y*ac.z - ab.z*ac.y;
    double jj = ab.z*ac.x - ab.x*ac.z;
    double kk = ab.x*ac.y - ab.y*ac.x;
    return sqrt(ii*ii + jj*jj + kk*kk)/2;
}

void TriangleNet::simplify(double v0, double s0, double a0) {
    struct Data {
        size_t handle;
        Point foldPoint;
        double volumeError;
        double maxArea;
        double angleError;
        bool operator<(const Data& upper) const {
            return volumeError < upper.volumeError;
        }
    };
    set<Data> errorHeap;
    for (size_t i = 0; i < triangleTab.vec.size(); ++i) {
        Data pushedData = { 
            i, 
            getFoldPoint(i), 
            getVolumeError(i),
            getMaxArea(i),
            getAngleError(i)
        };
        errorHeap.insert(pushedData);
    }
    while (!errorHeap.empty()) {
        auto topIt = errorHeap.begin();
        while (topIt->volumeError < v0) {
            if (topIt->maxArea < s0 && topIt->angleError < a0)
                break;
            else ++topIt;
        }
        if (topIt->volumeError >= v0)
            break;
        // TODO 折叠处理，删除和修改
        vector<array<size_t, 2>> vertexSet;
        Triangle& tri = triangleTab.vec[topIt->handle];
        for (size_t i = 0; i < 3; ++i) {
            size_t vertexId = tri[i];
            for (auto j = vertexAdj[vertexId].begin();
                j != vertexAdj[vertexId].end(); ++j) {
                Triangle& adjTri = triangleTab.vec[*j];
                array<size_t, 2> seg{vertexTab.vec.size(), vertexTab.vec.size()};
                size_t idx = 0;
                for (size_t k = 0; k < 3; ++k) {
                    size_t adjVertex = adjTri[k];
                    if (adjVertex != vertexId) {
                        if (adjVertex != tri[(i + 1)%3] && adjVertex != tri[(i + 2)%3])
                            seg[idx++] = adjVertex;
                        vertexAdj[adjVertex].remove(*j);
                    }
                }
                if (seg[0] != vertexTab.vec.size() && seg[1] != vertexTab.vec.size())
                    vertexSet.push_back(seg);
                triangleTab.erase(*j);
            }
            vertexTab.erase(vertexId);
        }
        triangleTab.erase(topIt->handle);
        errorHeap.erase(topIt);
        size_t nid = vertexTab.insert(topIt->foldPoint);
        for (auto& seg : vertexSet) {
            Triangle ntri{nid, seg[0], seg[1]};
            size_t ntd = triangleTab.insert(ntri);
            vertexAdj[seg[0]].push_front(ntd);
            vertexAdj[seg[1]].push_front(ntd);
            vertexAdj[nid].push_front(ntd);
        }
    }
}

istream& operator>>(istream& input, TriangleNet& net) {
    Point pin;
    string str;
    size_t xx;
    char ch;
    TriangleNet::Triangle tri;
    while (input >> str) {
        if (str == "v") {
            input >> pin.x >> pin.y >> pin.z;
            net.vertexTab.insert(pin);
        }
        else if (str == "f") {
            input >> tri[0] >> ch >> xx >> ch >> xx;
            input >> tri[1] >> ch >> xx >> ch >> xx;
            input >> tri[2] >> ch >> xx >> ch >> xx;
            --tri[0], --tri[1], --tri[2];
            size_t nid = net.triangleTab.insert(tri);
            net.vertexAdj[tri[0]].push_front(nid);
            net.vertexAdj[tri[1]].push_front(nid);
            net.vertexAdj[tri[2]].push_front(nid);
        }
        else    getline(input, str);
    }
}

ostream& operator<<(ostream& output, TriangleNet& net) {

}