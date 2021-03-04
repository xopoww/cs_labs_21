#include "gmsh.h"

#include <cmath>
#include <vector>
#include <set>

#include "cs_common.hpp"


// ===== CONSTANTS ======

std::size_t n_segments = 5;

const double R = 1000.;
const double r1 = 400.;
const double r2 = 340.;

const double mesh_size = 1e2;


// ===== TAG MAGIC FUNCTIONS =====

// --- POINT TAGS

const int center = 1;

inline int top_center(const int torus_i)
{
    return torus_i ? 2 : 3;
}

inline int bot_center(const int torus_i)
{
    return torus_i ? 4 : 5;
}


inline int circle_center(const int torus_i, const int seg_i)
{
    return 6 + n_segments * torus_i + seg_i;
}

enum CirclePoint : int
{
    TOP = 0,
    IN,
    BOT,
    OUT
};

inline int circle_point(const int torus_i, const int seg_i, const CirclePoint point)
{
    return 6 + 2 * n_segments +
        4 * n_segments * torus_i + seg_i * 4 + (int)point;
}


// --- ARC TAGS

inline int small_arc(const int torus_i, const int seg_i, const CirclePoint start)
{
    return 1 + 4 * n_segments * torus_i + seg_i * 4 + (int)start;
}

inline int big_arc(const int torus_i, const int end_seg_i, const CirclePoint which)
{
    return 1 + n_segments * 8 + 4 * n_segments * torus_i + end_seg_i * 4 + (int)which;
}


// --- CURVE TAGS

inline int curve_loop(const int torus_i, const int seg_i, const CirclePoint start)
{
    return 1 + 4 * n_segments * torus_i + seg_i * 4 + (int)start;
}


// --- SURFACE TAGS

inline int surface(const int torus_i, const int seg_i, const CirclePoint start)
{
    return curve_loop(torus_i, seg_i, start);
}



int main(int argc, char **argv)
{
    const double pi = 2 * std::acos(0);
    
    gmsh::initialize(argc, argv);

    gmsh::model::add("torus");

    addPoint(0, 0, 0, mesh_size, center);

    for (int torus_i = 0; torus_i < 2; torus_i++)
    {
        double r = torus_i ? r2 : r1;

        addPoint(0, 0,  r, mesh_size, top_center(torus_i));
        addPoint(0, 0, -r, mesh_size, bot_center(torus_i));

        LOG("Building segments...");

        for (std::size_t seg_i = 0; seg_i < n_segments; seg_i++)
        {
            double angle = 2 * pi / (double)n_segments * (seg_i);

            double
                c_x = R * std::cos(angle),
                c_y = R * std::sin(angle),
                d_x = r * std::cos(angle),
                d_y = r * std::sin(angle);
            
            // center of the circle
            addPoint(c_x, c_y, 0, mesh_size, circle_center(torus_i, seg_i));
            
            // 4 points on the circle
            addPoint(c_x + d_x, c_y + d_y, 0,  mesh_size, circle_point(torus_i, seg_i, OUT));
            addPoint(c_x,       c_y,       r,  mesh_size, circle_point(torus_i, seg_i, TOP));
            addPoint(c_x - d_x, c_y - d_y, 0,  mesh_size, circle_point(torus_i, seg_i, IN));
            addPoint(c_x,       c_y,       -r, mesh_size, circle_point(torus_i, seg_i, BOT));
            
            // 4 arcs of the circle
            addCircleArc(circle_point(torus_i, seg_i, TOP),  circle_center(torus_i, seg_i), circle_point(torus_i, seg_i, IN),  small_arc(torus_i, seg_i, TOP));
            addCircleArc(circle_point(torus_i, seg_i, IN),   circle_center(torus_i, seg_i), circle_point(torus_i, seg_i, BOT), small_arc(torus_i, seg_i, IN));
            addCircleArc(circle_point(torus_i, seg_i, BOT),  circle_center(torus_i, seg_i), circle_point(torus_i, seg_i, OUT), small_arc(torus_i, seg_i, BOT));
            addCircleArc(circle_point(torus_i, seg_i, OUT),  circle_center(torus_i, seg_i), circle_point(torus_i, seg_i, TOP), small_arc(torus_i, seg_i, OUT));

            LOG(" - done for seg_i = " << seg_i);
        }


        LOG("Building connecting arcs and surfaces...");

        std::vector<int> surfaces;

        for (std::size_t seg_i = 0; seg_i < n_segments; seg_i++)
        {
            std::size_t prev_seg = seg_i ? seg_i - 1 : n_segments - 1;
            
            // 4 arcs connecting the circle with the previous one
            addCircleArc(circle_point(torus_i, prev_seg, TOP),  top_center(torus_i), circle_point(torus_i, seg_i, TOP), big_arc(torus_i, seg_i, TOP));
            addCircleArc(circle_point(torus_i, prev_seg, IN),   center,              circle_point(torus_i, seg_i, IN),  big_arc(torus_i, seg_i, IN));
            addCircleArc(circle_point(torus_i, prev_seg, BOT),  bot_center(torus_i), circle_point(torus_i, seg_i, BOT), big_arc(torus_i, seg_i, BOT));
            addCircleArc(circle_point(torus_i, prev_seg, OUT),  center,              circle_point(torus_i, seg_i, OUT), big_arc(torus_i, seg_i, OUT));

            // curve loops
            addCurveLoop({small_arc(torus_i, prev_seg, TOP), big_arc(torus_i, seg_i, IN),  -small_arc(torus_i, seg_i, TOP), -big_arc(torus_i, seg_i, TOP)}, curve_loop(torus_i, seg_i, TOP)); // 190 chars in a line
            addCurveLoop({small_arc(torus_i, prev_seg, IN),  big_arc(torus_i, seg_i, BOT), -small_arc(torus_i, seg_i, IN),  -big_arc(torus_i, seg_i, IN)},  curve_loop(torus_i, seg_i, IN));  // show the quality of
            addCurveLoop({small_arc(torus_i, prev_seg, BOT), big_arc(torus_i, seg_i, OUT), -small_arc(torus_i, seg_i, BOT), -big_arc(torus_i, seg_i, BOT)}, curve_loop(torus_i, seg_i, BOT)); // the code
            addCurveLoop({small_arc(torus_i, prev_seg, OUT), big_arc(torus_i, seg_i, TOP), -small_arc(torus_i, seg_i, OUT), -big_arc(torus_i, seg_i, OUT)}, curve_loop(torus_i, seg_i, OUT));

            // surfaces
            addSurfaceFilling({curve_loop(torus_i, seg_i, TOP)}, curve_loop(torus_i, seg_i, TOP));
            addSurfaceFilling({curve_loop(torus_i, seg_i, IN)},  curve_loop(torus_i, seg_i, IN));
            addSurfaceFilling({curve_loop(torus_i, seg_i, BOT)}, curve_loop(torus_i, seg_i, BOT));
            addSurfaceFilling({curve_loop(torus_i, seg_i, OUT)}, curve_loop(torus_i, seg_i, OUT));

            surfaces.push_back(curve_loop(torus_i, seg_i, TOP));
            surfaces.push_back(curve_loop(torus_i, seg_i, IN));
            surfaces.push_back(curve_loop(torus_i, seg_i, BOT));
            surfaces.push_back(curve_loop(torus_i, seg_i, OUT));

            LOG(" - done for seg_i = " << seg_i);
        }

        addSurfaceLoop(surfaces, torus_i + 1);
    }

    addVolume({1, 2});

    gmsh::model::geo::synchronize();
    gmsh::model::mesh::generate(3);

    gmsh::write("torus.msh");

    std::set<std::string> args(argv, argv + argc);
    if (!args.count("-nopopup"))
    {
        gmsh::fltk::run();
    }

    gmsh::finalize();
    return 0;
}
