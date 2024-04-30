#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
namespace PolygonalLibrary{

bool ImportMesh (const string& filepath, PolygonalMesh& mesh, double& tolInput)
{
    if (!ImportCell0Ds(filepath + "/Cell0Ds.csv", mesh))
        return false;
    else
    {
        cout << "Cell0D marker: " << endl;
        for (auto it = mesh.MarkersCell0D.begin(); it != mesh.MarkersCell0D.end(); it++)
        {
            cout << "key: \t" << it -> first << "\t values:";
            for(const unsigned int id: it -> second)
                cout << "\t" << id;
            cout << endl;
        }
    }
    if (!ImportCell1Ds(filepath + "/Cell1Ds.csv", mesh))
        return false;
    else
    {
        cout << "Cell1D marker: " << endl;
        for (auto it = mesh.MarkersCell1D.begin(); it != mesh.MarkersCell1D.end(); it++)
        {
            cout << "key: \t" << it -> first << "\t values:";
            for(const unsigned int id: it -> second)
                cout << "\t" << id;
            cout << endl;
        }
    }
    if (!ImportCell2Ds(filepath + "/Cell2Ds.csv", mesh))
        return false;
    else
    {
        //Test
        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            vector<unsigned int> edges = mesh.EdgesCell2D[c];
            unsigned int numedges = edges.size();

            for(unsigned int e = 0; e < numedges; e++)
            {
                const unsigned int origin = mesh.VerticesCell1D[edges[e]][0];
                const unsigned int end = mesh.VerticesCell1D[edges[e]][1];

                auto findOrigin = find(mesh.VerticesCell2D[c].begin(), mesh.VerticesCell2D[c].end(), origin);
                if(findOrigin == mesh.VerticesCell2D[c].end())
                {
                    cerr<< "Wrong mesh" << endl;
                    return 2;
                }

                auto findEnd = find(mesh.VerticesCell2D[c].begin(), mesh.VerticesCell2D[c].end(), end);
                if(findEnd == mesh.VerticesCell2D[c].end())
                {
                    cerr<< "Wrong mesh" << endl;
                    return 3;
                }
            }
        }
        double tol = max(tolInput, 10* numeric_limits<double>::epsilon() );

        CheckEdges(mesh, tol);
        CheckArea(mesh, tol);

    }
    return true;
}

bool ImportCell0Ds (const string& filename, PolygonalMesh& mesh)
{
    ifstream file;
    file.open(filename);

    if (file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
    {
        replace(line.begin(), line.end(), ';', ' ');
        listLines.push_back(line);
    }

    listLines.pop_front();

    mesh.NumberCell0D = listLines.size();

    if (mesh.NumberCell0D == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }
    mesh.IdCell0D.reserve(mesh.NumberCell0D);
    mesh.CoordinatesCell0D.reserve(mesh.NumberCell0D);

    for (const string& line : listLines)
    {
        istringstream converter(line);
        unsigned int id;
        unsigned int marker;
        Vector2d coordinates;

        converter >> id >> marker >> coordinates(0) >> coordinates(1);

        mesh.IdCell0D.push_back(id);
        mesh.CoordinatesCell0D.push_back(coordinates);

        if (marker != 0)
        {
            auto ret = mesh.MarkersCell0D.insert({marker, {id}});
            if(!ret.second)
                (ret.first) -> second.push_back(id);
        }
    }
    file.close();
    return true;

}
bool ImportCell1Ds (const string& filename, PolygonalMesh& mesh)
{
    ifstream file;
    file.open(filename);

    if (file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
    {
        replace(line.begin(), line.end(), ';', ' ');
        listLines.push_back(line);
    }

    listLines.pop_front();

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }
    mesh.IdCell1D.reserve(mesh.NumberCell1D);
    mesh.VerticesCell1D.reserve(mesh.NumberCell1D);

    for (const string& line : listLines)
    {
        istringstream converter(line);
        unsigned int id;
        unsigned int marker;
        Vector2i origend;

        converter >> id >> marker >> origend(0) >> origend(1);

        mesh.IdCell1D.push_back(id);
        mesh.VerticesCell1D.push_back(origend);

        if (marker != 0)
        {
            auto ret = mesh.MarkersCell1D.insert({marker, {id}});
            if(!ret.second)
                (ret.first) -> second.push_back(id);
        }
    }
    file.close();
    return true;
}
bool ImportCell2Ds (const string& filename, PolygonalMesh& mesh)
{
    ifstream file;
    file.open(filename);

    if (file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
    {
        replace(line.begin(), line.end(), ';', ' ');
        listLines.push_back(line);
    }

    listLines.pop_front();

    mesh.NumberCell2D = listLines.size();

    if (mesh.NumberCell2D == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }
    mesh.IdCell2D.reserve(mesh.NumberCell2D);
    mesh.VerticesCell2D.reserve(mesh.NumberCell2D);
    mesh.EdgesCell2D.reserve(mesh.NumberCell2D);

    for (const string& line : listLines)
    {
        istringstream converter(line);
        unsigned int id;
        unsigned int marker;
        unsigned int numvertices;
        unsigned int numedges;


        converter >> id >> marker >> numvertices;
        vector<unsigned int> vertices(numvertices);

        for (unsigned int i = 0; i < numvertices; i++)
            converter >> vertices[i];

        converter >> numedges;
        vector<unsigned int> edges(numedges);

        for (unsigned int j = 0; j < numedges; j++)
            converter >> edges[j];

        mesh.IdCell2D.push_back(id);
        mesh.VerticesCell2D.push_back(vertices);
        mesh.EdgesCell2D.push_back(edges);

        if (marker != 0)
        {
            auto ret = mesh.MarkersCell2D.insert({marker, {id}});
            if(!ret.second)
                (ret.first) -> second.push_back(id);
        }
    }
    file.close();
    return true;
}

void CheckEdges (PolygonalMesh& mesh, double& tol)
{
    for (unsigned int e = 0; e < mesh.NumberCell1D; e++ )
    {
        const unsigned int origin = mesh.VerticesCell1D[e][0];
        const unsigned int end = mesh.VerticesCell1D[e][1];
        double coord1orig = mesh.CoordinatesCell0D[origin][0];
        double coord2orig = mesh.CoordinatesCell0D[origin][1];
        double coord1end = mesh.CoordinatesCell0D[end][0];
        double coord2end = mesh.CoordinatesCell0D[end][1];
        double length = sqrt(pow(coord1orig - coord1end, 2) + pow(coord2orig - coord2end, 2));
        if (length < tol)
        {
            cout << "The edge " << e << " has length less than the tollerance" << endl;
        }
    }
    cout << "The length of all the edges has been checked" << endl;
}

void CheckArea (PolygonalMesh& mesh, double& tol)
{
    double tolArea = pow(tol, 2);
    for (unsigned int f = 0; f < mesh.NumberCell2D; f++)
    {
        unsigned int numvertices = mesh.VerticesCell2D[f].size();
        double sum = 0.0;
        for ( unsigned int i=0; i< numvertices; i++  )
        {
            unsigned int vert1 = mesh.VerticesCell2D[f][i];
            double x1 = mesh.CoordinatesCell0D[vert1][0];
            double y1 = mesh.CoordinatesCell0D[vert1][1];

            unsigned int vert2;
            if (i == numvertices-1)
                vert2 = mesh.VerticesCell2D[f][0];

            else
                vert2 = mesh.VerticesCell2D[f][i+1];

            double x2 = mesh.CoordinatesCell0D[vert2][0];
            double y2 = mesh.CoordinatesCell0D[vert2][1];

            sum+= (x1*y2 - x2*y1);

        }
        double area = abs(sum)/2;
        if (area < tolArea)
        {
            cout << "The polygon" << f << "has area less than tollerance" << endl;
        }
    }
    cout << "The area of all the polygons has been checked"<< endl;
}

}
