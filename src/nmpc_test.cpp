
#include "main.h"

#include <stdio.h>
#include <vector>
#include "timer.hpp"

#define SOLVER_ASSERT(x)
#define EIGEN_NO_DEBUG
// #define QP_SOLVER_PRINTING
#define eigen_assert(x) my_assert(__FILE__, __LINE__, x)

#include "control/nmpc.hpp"
#include "control/simple_robot_model.hpp"
#include "polynomials/ebyshev.hpp"

#include <unsupported/Eigen/AutoDiff>

namespace nmpc {

using Problem = polympc::OCProblem<MobileRobot<float>, Lagrange<float>, Mayer<float>>;
using Approximation = Chebyshev<3, GAUSS_LOBATTO, float>; // POLY_ORDER = 3

using controller_t = polympc::nmpc<Problem, Approximation, int>;
using var_t = controller_t::var_t;
using dual_t = controller_t::dual_t;
using State = controller_t::State;
using Control = controller_t::Control;
using Parameters = controller_t::Parameters;

enum
{
    NX = State::RowsAtCompileTime,
    NU = Control::RowsAtCompileTime,
    NP = Parameters::RowsAtCompileTime,
    VARX_SIZE = controller_t::VARX_SIZE,
    VARU_SIZE = controller_t::VARU_SIZE,
    VARP_SIZE = controller_t::VARP_SIZE,
    VAR_SIZE = var_t::RowsAtCompileTime,
};

void print_info(void)
{
    printf("NX = %d\n", NX);
    printf("NU = %d\n", NU);
    printf("NP = %d\n", NP);
    printf("VARX_SIZE = %d\n", VARX_SIZE);
    printf("VARU_SIZE = %d\n", VARU_SIZE);
    printf("VARP_SIZE = %d\n", VARP_SIZE);
    printf("VAR_SIZE = %d\n", VAR_SIZE);

    printf("controller_t size %u\n", sizeof(controller_t));
    printf("controller_t::cost_colloc_t size %u\n", sizeof(controller_t::cost_colloc_t));
    printf("controller_t::ode_colloc_t size %u\n", sizeof(controller_t::ode_colloc_t));
    printf("controller_t::sqp_t size %u\n", sizeof(controller_t::sqp_t));
    printf("controller_t::sqp_t::qp_t size %u\n", sizeof(controller_t::sqp_t::qp_t));
    printf("controller_t::sqp_t::qp_solver_t size %u\n", sizeof(controller_t::sqp_t::qp_solver_t));
}

template<typename Derived>
inline bool is_nan(const Eigen::MatrixBase<Derived>& x)
{
    return !((x.array() == x.array())).all();
}

template <typename Mat>
void print_mat(const Mat &m, const char *fmt = "%2f ")
{
    for (int i = 0; i < m.rows(); i++) {
        for (int j = 0; j < m.cols(); j++) {
            printf(fmt, m.coeff(i,j));
        }
        printf("\n");
    }
}

template <typename Solver>
void callback(void *solver_p)
{
    Solver& s = *static_cast<Solver*>(solver_p);

    print_mat(s._x.transpose());
}


// controller must be 16 byte aligned because of Eigen
__attribute__((aligned(16)))
__attribute__((section(".ram0")))
static char controller_buf[sizeof(controller_t)];

controller_t* get_controller()
{
    static controller_t *cp = NULL;
    if (cp == NULL) {
        // use placement new on the preallocated buffer for late initialization.
        cp = new(controller_buf) controller_t;
        printf("new controller_t: %p\n", cp);
        if (cp == NULL) {
            panic("controller == NULL");
        }
    }
    return cp;
}

void run_test(void)
{
    Timer t;

    // State x = {-1, -0.5, -0.5}; // gives NaN in third iter
    State x = {-1, -0.5, 0};
    State dx;
    Control u;
    Parameters p(0.5);

    // bounds
    State xu, xl;
    xu << 10, 10, 1e20;
    xl << -xu;
    Control uu, ul;
    uu << 1, 1;
    ul << 0, -1;

    controller_t& robot_controller = *get_controller();
    robot_controller.set_constraints(xl, xu, ul, uu);
    // robot_controller.solver.settings().iteration_callback = callback<controller_t::sqp_t>;

    print_info();
    print_mat(x.transpose());

    for (int i = 0; i < 30; i++) {

        var_t sol;

        t.tic();
        if (i == 0) {
            sol = robot_controller.solve(x, p);
        } else {
            sol = robot_controller.solve_warm_start(x, p);
        }
        t.toc();
        t.print();
        t.clear();

        printf("t: %d  ", i);
        printf("iter %d ", robot_controller.solver.info().iter);
        printf("qp %d\n", robot_controller.solver.info().qp_solver_iter);

        u = sol.segment<2>(VARX_SIZE+VARU_SIZE-NU);

        printf("x ");
        print_mat(x.transpose());
        printf("u ");
        print_mat(u.transpose());

        // crude integrator
        const float dt = 0.001;
        for (int j = 0; j < 200; j++) {
            robot_controller.ps_ode.m_f(x, u, p, dx);
            x = x + dt * dx;
        }

        if (is_nan<var_t>(sol)) {
            printf("ERROR: NAN\n");
            break;
        }
    }
}

} // namespace nmpc

extern "C" {

void nmpc_test(void)
{
    nmpc::run_test();
}

} // extern "C"
