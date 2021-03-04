#include "gmsh.h"

#include <cmath>
#include <vector>
#include <set>

#include "cs_common.hpp"

#define MODEL_NAME "pt_rail2"

#ifdef _WIN32
    #define STL_FILE "..\\" MODEL_NAME ".stl"
#else
    #define STL_FILE "../"  MODEL_NAME ".stl"
#endif

#define MSH_FILE MODEL_NAME ".msh"


#define MESH_SIZE "5"

int main(int argc, char **argv)
{
    gmsh::initialize();

    gmsh::model::add(MODEL_NAME);

    LOG("Loading STL");

    try {
        gmsh::merge(STL_FILE);
    } catch(...) {
        gmsh::logger::write("Failed to load STL mesh");
        gmsh::finalize();
    return 0;
    }

    double angle = 40;

    bool forceParametrizablePatches = true;

    bool includeBoundary = false;

    LOG("Classifying surfaces");

    gmsh::model::mesh::classifySurfaces(angle * M_PI / 180., includeBoundary,
                                        forceParametrizablePatches);

    LOG("Creating geometry");

    gmsh::model::mesh::createGeometry();

    std::vector<std::pair<int, int> > s;
    gmsh::model::getEntities(s, 2);
    std::vector<int> sl;
    for(auto surf : s)
    {
        sl.push_back(surf.second);
    }
    addSurfaceLoop(sl, 1);
    addVolume({1});

    LOG("Synchronizing");

    gmsh::model::geo::synchronize();

    int field_tag = gmsh::model::mesh::field::add("MathEval");
    gmsh::model::mesh::field::setString(field_tag, "F", MESH_SIZE);
    gmsh::model::mesh::field::setAsBackgroundMesh(field_tag);

    LOG("Generating");

    gmsh::model::mesh::generate(3);

    gmsh::write(MSH_FILE);

    LOG("All done");

    std::set<std::string> args(argv, argv + argc);

    if (!args.count("-nopopup"))
    {
        gmsh::fltk::run();
    }

    gmsh::finalize();
    return 0;
}