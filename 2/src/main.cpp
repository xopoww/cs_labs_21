#include "gmsh.h"

#include <cmath>
#include <vector>
#include <set>

#include "cs_common.hpp"

#include "calcmesh.hpp"


#define MODEL_NAME "pt_rail"

#ifdef _WIN32
    #define STL_FILE "..\\..\\1\\" MODEL_NAME ".stl"
#else
    #define STL_FILE "../../1/"  MODEL_NAME ".stl"
#endif


#define MESH_SIZE "5"

#define N_STEPS 100
#define TAU 0.1


int main(int argc, char **argv)
{
    // this code is copied from 1st lab
    // v v v v v
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
    gmsh::model::geo::addSurfaceLoop(sl, 1);
    gmsh::model::geo::addVolume({1});

    LOG("Synchronizing");

    gmsh::model::geo::synchronize();

    int field_tag = gmsh::model::mesh::field::add("MathEval");
    gmsh::model::mesh::field::setString(field_tag, "F", MESH_SIZE);
    gmsh::model::mesh::field::setAsBackgroundMesh(field_tag);

    LOG("Generating");

    gmsh::model::mesh::generate(3);

    // ^ ^ ^ ^ ^

    LOG("Extracting mesh from gmsh");

    // this is copied from example code
    // v v v v v
    
    const unsigned int GMSH_TETR_CODE = 4;

    // Теперь извлечём из gmsh данные об узлах сетки
    std::vector<double> nodesCoord;
    std::vector<std::size_t> nodeTags;
    std::vector<double> parametricCoord;
    gmsh::model::mesh::getNodes(nodeTags, nodesCoord, parametricCoord);

    // И данные об элементах сетки тоже извлечём, нам среди них нужны только тетраэдры, которыми залит объём
    std::vector<std::size_t>* tetrsNodesTags = nullptr;
    std::vector<int> elementTypes;
    std::vector<std::vector<std::size_t>> elementTags;
    std::vector<std::vector<std::size_t>> elementNodeTags;
    gmsh::model::mesh::getElements(elementTypes, elementTags, elementNodeTags);
    for(unsigned int i = 0; i < elementTypes.size(); i++) {
        if(elementTypes[i] != GMSH_TETR_CODE)
            continue;
        tetrsNodesTags = &elementNodeTags[i];
    }

    if(tetrsNodesTags == nullptr) {
        std::cout << "Can not find tetra data. Exiting." << std::endl;
        gmsh::finalize();
        return -2;
    }

    std::cout << "The model has " <<  nodeTags.size() << " nodes and " << tetrsNodesTags->size() / 4 << " tetrs." << std::endl;

    // На всякий случай проверим, что номера узлов идут подряд и без пробелов
    for(int i = 0; i < nodeTags.size(); ++i) {
        // Индексация в gmsh начинается с 1, а не с нуля. Ну штош, значит так.
        assert(i == nodeTags[i] - 1);
    }
    // И ещё проверим, что в тетраэдрах что-то похожее на правду лежит.
    assert(tetrsNodesTags->size() % 4 == 0);

    // ^ ^ ^ ^ ^

    CalcMesh mesh{nodesCoord, *tetrsNodesTags};

    gmsh::finalize();

    LOG("Starting the calculation");

    for (std::size_t step_i = 0; step_i <= N_STEPS; step_i++)
    {
        if (step_i)
        {
            mesh.doTimeStep(TAU);
        }
        mesh.snapshot(step_i);
    }

    LOG("Done");

    return 0;
}
