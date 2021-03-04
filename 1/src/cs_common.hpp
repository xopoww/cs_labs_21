#pragma once

#include "gmsh.h"

#if defined _CS_LABS_DEBUG && _CS_LABS_DEBUG > 0

    #include <iostream>

    #define LOG(msg) std::cout << " -- " << msg << std::endl

    #define CATCH(type, expr)           \
    try { expr; } catch (type caught)   \
    { LOG("caught at " << __LINE__ << ": " << caught); throw 1; }

    #define _CATCH_GMSH(func, ...) CATCH(int, gmsh::model::geo::func(__VA_ARGS__))

    #define addPoint(...)       _CATCH_GMSH(addPoint, __VA_ARGS__)
    #define addCircleArc(...)   _CATCH_GMSH(addCircleArc, __VA_ARGS__)
    #define addCurveLoop(...)   _CATCH_GMSH(addCurveLoop, __VA_ARGS__)
    #define addSurfaceFilling(...)   _CATCH_GMSH(addSurfaceFilling, __VA_ARGS__)
    #define addSurfaceLoop(...)       _CATCH_GMSH(addSurfaceLoop, __VA_ARGS__)
    #define addVolume(...)       _CATCH_GMSH(addVolume, __VA_ARGS__)

#else

    #define LOG(msg)

    #define CATCH(type, expr) expr

    using gmsh::model::geo::addPoint;
    using gmsh::model::geo::addCircleArc;
    using gmsh::model::geo::addCurveLoop;
    using gmsh::model::geo::addSurfaceFilling;
    using gmsh::model::geo::addSurfaceLoop;
    using gmsh::model::geo::addVolume;

#endif
