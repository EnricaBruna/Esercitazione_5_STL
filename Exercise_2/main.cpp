#include "PolygonalMesh.hpp"
#include "Utils.hpp"
#include <iostream>


using namespace std;
using namespace PolygonalLibrary;

int main()
{
    PolygonalMesh mesh;
    double tol = 1e-15;

    if(!ImportMesh("PolygonalMesh", mesh, tol))
        return 1;

    return 0;
}
