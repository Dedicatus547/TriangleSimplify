#include <queue>
#include <string>
#include <cmath>
#include "TriangleSimplify.h"
using namespace std;

Point TriangleNet::getFoldPoint(size_t trid) const {
    const Point& a = vertexTab.at(triangleTab.at(trid)[0]);
    const Point& b = vertexTab.at(triangleTab.at(trid)[1]);
    const Point& c = vertexTab.at(triangleTab.at(trid)[2]);
    double nx = (a.x + b.x + c.x)/3;
    double ny = (a.y + b.y + c.y)/3;
    double nz = (a.z + b.z + c.z)/3;
    return  {nx, ny, nz};
}

double TriangleNet::getMaxArea(size_t trid) const {
    const Point& a = vertexTab.at(triangleTab.at(trid)[0]);
    const Point& b = vertexTab.at(triangleTab.at(trid)[1]);
    const Point& c = vertexTab.at(triangleTab.at(trid)[2]);
    Point ab{a.x - b.x, a.y - b.y, a.z - b.z};
    Point ac{a.x - c.x, a.y - c.y, a.z - c.z};
    double ii = ab.y*ac.z - ab.z*ac.y;
    double jj = ab.z*ac.x - ab.x*ac.z;
    double kk = ab.x*ac.y - ab.y*ac.x;
    return sqrt(ii*ii + jj*jj + kk*kk)/2;
}

void TriangleNet::simplify(double s0) {
    struct Data {
        size_t handle;
        double maxArea;
        bool operator<(const Data& upper) const {
            return maxArea > upper.maxArea;
        }
    };
    priority_queue<Data> errorHeap;
    for (size_t i = 0; i < triangleTab.vec.size(); ++i) {
        Data pushedData = {i, getMaxArea(i)};
        errorHeap.push(pushedData);
    }
    while (!errorHeap.empty()) {
        auto topIt = errorHeap.top();
        if (triangleTab.is_erased[topIt.handle])
            continue;
        if (topIt.maxArea > s0)    break;
        // TODO 折叠处理，删除和修改
        vector<array<size_t, 2>> vertexSet;
        Triangle& tri = triangleTab[topIt.handle];
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
        triangleTab.erase(topIt.handle);
        size_t nid = vertexTab.insert(getFoldPoint(topIt.handle));
        for (auto& seg : vertexSet) {
            Triangle ntri{nid, seg[0], seg[1]};
            size_t ntd = triangleTab.insert(ntri);
            vertexAdj[seg[0]].push_front(ntd);
            vertexAdj[seg[1]].push_front(ntd);
            vertexAdj[nid].push_front(ntd);
            errorHeap.push({ntd, getMaxArea(ntd)});
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
    auto& vtt = net.vertexTab;
    for (size_t i = 0; i < vtt.vec.size(); ++i) {
        if (!vtt.is_erased[i]) {
            output << "v  " << vtt[i].x
                    << " " << vtt[i].y
                    << " " << vtt[i].z
                    << endl;
        }
    }
    output << endl;
    auto& trt = net.triangleTab;
    for (size_t i = 0; i < trt.vec.size(); ++i) {
        if (!trt.is_erased[i]) {
            output << "f  " << trt[i][0]
                    << " " << trt[i][1]
                    << " " << trt[i][3]
                    << endl;
        }
    }
}