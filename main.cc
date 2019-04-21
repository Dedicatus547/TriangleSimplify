#include <fstream>
#include "TriangleSimplify.h"
#include "resourse.h"
using namespace std;

int main() {
    ifstream fin("input.obj");
    TriangleNet net;
    fin >> net;
    fin.close();
    net.simplify(MAXAREA);
    ofstream fout("output.obj");
    fout << net;
    fout.close();
    return 0;
}