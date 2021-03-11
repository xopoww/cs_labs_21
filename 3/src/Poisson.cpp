#include "cs_common.hpp"

#include <dolfin.h>
#include "Poisson.h"


#define N_STEPS 100


using namespace dolfin;

// Source term (right-hand side)
class Source : public Expression
{
public:
    int step_i;

private:
    void eval(Array<double>& values, const Array<double>& x) const
    {
        float alpha = (float)step_i / (float)N_STEPS - 0.5f;
        double dx = x[0] - 0.5;
        double dy = x[1] - 0.5;
        values[0] = 10*exp(-(dx*dx + (std::sqrt(16 * std::abs(alpha)) * (alpha > 0 ? 1.f : -1.f))*dx*dy + dy*dy) / 0.02 );
    }
};

// Normal derivative (Neumann boundary condition)
class dUdN : public Expression
{
public:
    int step_i;

private:

    void eval(Array<double>& values, const Array<double>& x) const
    {
        float alpha = (float)step_i / (float)N_STEPS - 0.5f;
        values[0] = sin(5*x[0]) * (-alpha) + cos(3*x[0]) * alpha;
    }
};

// Sub domain for Dirichlet boundary condition
class DirichletBoundary : public SubDomain
{
    bool inside(const Array<double>& x, bool on_boundary) const
    {
        return x[0] < DOLFIN_EPS or x[0] > 1.0 - DOLFIN_EPS;
    }
};


int main()
{
    // Create mesh and function space
    auto mesh = std::make_shared<Mesh>(UnitSquareMesh::create({{32, 32}}, CellType::Type::triangle));
    auto V = std::make_shared<Poisson::FunctionSpace>(mesh);

    // Define boundary condition
    auto u0 = std::make_shared<Constant>(0.0);
    auto boundary = std::make_shared<DirichletBoundary>();

    auto f = std::make_shared<Source>();
    auto g = std::make_shared<dUdN>();

    File file("poisson.pvd");

    for (int step_i = 0; step_i < N_STEPS; step_i++)
    {
        LOG("Performing step " << step_i + 1 << "/" << N_STEPS);

        // update step index
        g->step_i = step_i;
        f->step_i = step_i;

        DirichletBC bc(V, u0, boundary);

        // Define variational forms
        Poisson::BilinearForm a(V, V);
        Poisson::LinearForm L(V);
        L.f = f;
        L.g = g;

        // Compute solution
        Function u(V);
        solve(a == L, u, bc);

        // Save solution in VTK format
        file << u;
    }
    return 0;
}
