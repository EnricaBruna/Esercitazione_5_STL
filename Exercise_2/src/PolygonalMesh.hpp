#pragma once
#include <iostream>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;

namespace PolygonalLibrary {

struct PolygonalMesh {

    unsigned int NumberCell0D = 0;
    vector<unsigned int> IdCell0D = {};
    vector<Vector2d> CoordinatesCell0D = {};
    map<unsigned int, list<unsigned int>> MarkersCell0D = {};

    unsigned int NumberCell1D = 0;
    vector<unsigned int> IdCell1D = {};
    vector<Vector2i> VerticesCell1D = {};
    map<unsigned int, list<unsigned int>> MarkersCell1D = {};

    unsigned int NumberCell2D = 0;
    vector<unsigned int> IdCell2D = {};
    vector<vector<unsigned int>> VerticesCell2D = {};
    vector<vector<unsigned int>> EdgesCell2D = {};
    map<unsigned int, list<unsigned int>> MarkersCell2D = {};
};

}
