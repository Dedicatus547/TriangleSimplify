#include "resourse.h"
using namespace std;

int main(int args, const char* argv[]) {
    if (args < 2)
        cerr << "error: no input files\n";
    ifstream fin(argv[1]);
    TriangleNet net;
    fin >> net;
    fin.close();
    net.simplify(VOLUMEERROR);
    ofstream fout(OUTPUTFILE);
    fout << net;
    fout.close();
    return 0;
}