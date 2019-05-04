#include <queue>
#include <string>
#include <cmath>
#include <unordered_set>
#include "TriangleSimplify.h"
#include "Matrix.h"
using namespace std;

const double lambda1 = 0.6;
const double lambda2 = 0.4;
Point TriangleNet::getNorm(size_t trid) const {
    const Point& a = vertexTab.at(triangleTab.at(trid)[0]);
    const Point& b = vertexTab.at(triangleTab.at(trid)[1]);
    const Point& c = vertexTab.at(triangleTab.at(trid)[2]);
    Point ab{a.x - b.x, a.y - b.y, a.z - b.z};
    Point ac{a.x - c.x, a.y - c.y, a.z - c.z};
    double ii = ab.y*ac.z - ab.z*ac.y;
    double jj = ab.z*ac.x - ab.x*ac.z;
    double kk = ab.x*ac.y - ab.y*ac.x;
    return {ii, jj, kk};
}

double TriangleNet::getArea(size_t trid) const {
    Point&& n = getNorm(trid);
    return sqrt(n.x*n.x + n.y*n.y + n.z*n.z)/2;
}

Point TriangleNet::getFoldPoint(size_t trid) const {
    Point ni = getNorm(trid);
    Point efz{0, 0};
    double efm = 0;
    for (size_t i = 0; i < 3; ++i) {
        size_t vid = triangleTab.at(trid)[i];
        Point fz{0, 0};
        double fm = 0;
        unordered_set<size_t> is_vis;
        double gama = 0;
        for (size_t tj : vertexAdj[vid]) {
            if (tj == trid)
                continue;
            double stj = getArea(tj);
            fz = fz + (vertexTab.at(vid)*stj);
            fm += stj;
            is_vis.insert(tj);
            Point nji = getNorm(tj) - ni;
            gama += sqrt(nji.dotPro(nji));
        }
        for (size_t ii : {1, 2}) {
            size_t fff = triangleTab.at(trid)[(i + ii)%3];
            for (size_t tj : vertexAdj[fff]) {
                if (tj == trid || is_vis.find(tj) != is_vis.end())
                    continue;
                Point n = getNorm(tj);
                Point p = vertexTab.at(triangleTab.at(tj)[0]);
                Point v = vertexTab.at(vid);
                double t = (n.dotPro(p) - n.dotPro(v))/n.dotPro(n);
                Point ptj = v + (n*t);
                double stj = getArea(tj);
                fz = fz + (ptj*stj);
                fm += stj;
                is_vis.insert(tj);
            }
        }
        Point vi = fz/fm*lambda1 + vertexTab.at(vid)*lambda2;
        efz = efz + vi*gama;
        efm += gama;
    }
    return efz/efm;
}

double TriangleNet::getVolumeError(size_t trid, const Point& fp) const {
    auto getPTP = [](double a, double b, double c, double d) {
        Matrix<double, 4, 4> ptp;
        ptp[0] = {a*a, a*b, a*c, a*d};
        ptp[1] = {a*b, b*b, b*c, b*d};
        ptp[2] = {a*c, b*c, c*c, c*d};
        ptp[3] = {a*d, b*d, c*d, d*d};
        return ptp;
    };
    unordered_set<size_t> is_vis;
    Matrix<double, 1, 1> res;
    res[0][0] = 0;
    for (size_t i : triangleTab.at(trid)) {
        for (auto j : vertexAdj[i]) {
            if (is_vis.find(j) != is_vis.end())
                continue;
            Point n = getNorm(j);
            double len = sqrt(n.dotPro(n));
            double a = n.x/len;
            double b = n.y/len;
            double c = n.z/len;
            double d = n.dotPro(vertexTab.at(triangleTab.at(j)[0]));
            d = -d/len;
            auto ptp = getPTP(a, b, c, d);
            Matrix<double, 1, 4> vi;
            vi[0] = {fp.x, fp.y, fp.z, 1};
            res = res + vi*ptp*vi.transpose()*getArea(j);
            is_vis.insert(j);
        }
    }
    return res[0][0]/9;
}

void TriangleNet::simplify(double v0) {
    struct Data {
        size_t handle;
        Point foldPoint;
        double volumeError;
        bool operator<(const Data& upper) const {
            return volumeError > upper.volumeError;
        }
    };
    priority_queue<Data> errorHeap;
    for (size_t i = 0; i < triangleTab.vec.size(); ++i) {
        Point fp = getFoldPoint(i);
        Data pushedData{i, fp, getVolumeError(i, fp)};
        errorHeap.push(pushedData);
    }
    while (!errorHeap.empty()) {
        auto topIt = errorHeap.top();
        errorHeap.pop();
        if (triangleTab.is_erased[topIt.handle])
            continue;
        if (topIt.volumeError > v0)    break;
        // TODO 折叠处理，删除和修改
        vector<array<size_t, 2>> vertexSet;
        Triangle& tri = triangleTab[topIt.handle];
        unordered_set<size_t> triSet;
        for (size_t vi : tri) {
            for (size_t tj : vertexAdj[vi]) {
                triSet.insert(tj);
            }
        }
        for (size_t t : triSet) {
            array<size_t, 2> seg;
            size_t idx = 0;
            for (size_t v : triangleTab[t]) {
                if (v != tri[0] && v != tri[1] && v != tri[2])
                    seg[idx++] = v;
                vertexAdj[v].remove(t);
            }
            if (idx == 2)   vertexSet.push_back(seg);
            triangleTab.erase(t);
        }
        for (size_t v : tri) {
            vertexTab.erase(v);
            vertexAdj[v].clear();
        }
        size_t nid = vertexTab.insert(topIt.foldPoint);
        for (auto& seg : vertexSet) {
            Triangle ntri{nid, seg[0], seg[1]};
            size_t ntd = triangleTab.insert(ntri);
            vertexAdj[seg[0]].push_front(ntd);
            vertexAdj[seg[1]].push_front(ntd);
            vertexAdj[nid].push_front(ntd);
        }
        for (size_t pj : vertexAdj[nid]) {
            Point nfp = getFoldPoint(pj);
            Data psd{pj, nfp, getVolumeError(pj, nfp)};
            errorHeap.push(psd);
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
            net.vertexAdj.push_back(forward_list<size_t>());
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
    return input;
}

ostream& operator<<(ostream& output, TriangleNet& net) {
    auto& vtt = net.vertexTab;
    vector<size_t> trans(vtt.vec.size());
    size_t nid = 0;
    for (size_t i = 0; i < vtt.vec.size(); ++i) {
        if (!vtt.is_erased[i]) {
            trans[i] = ++nid;
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
            output << "f  " << trans[trt[i][0]]
                    << " " << trans[trt[i][1]]
                    << " " << trans[trt[i][2]]
                    << endl;
        }
    }
    output << endl;
    return output;
}