#ifndef UTILS_HEADER
#define UTILS_HEADER

#include "Eigen/Dense"

/** Generate n collocation points. 
    TODO: These should not be uniform */
template<typename Scalar, typename Index, Index n_c>
Eigen::Matrix<Scalar, n_c, 1>
generateCollocationPoints() {
    return Eigen::Matrix<Scalar, n_c, 1>::LinSpaced(n_c, 0, n_c - 1) / (n_c - 1);
}

/**
 * This function calculates a matrix of coefficients which can be used to calculate the
 * derivatives of a function f. Specifically, if fx denotes a matrix
 * containing the values of a function f evaluated at the collocation points, c:
 *
 * fx = [ f( collocation_points(0) ), ... f( collocation_points(n_c-1) ) ]
 *
 * and if coeffs denotes the output of this function, then we will approximately find that
 *
 * fx * coeffs = [ df/dt( collocation_points(0) ), ... df/dt( collocation_points(n_c-1) ) ]
 */
template<typename Derived>
Eigen::Matrix<typename Derived::Scalar, Derived::SizeAtCompileTime, Derived::SizeAtCompileTime>
lagrangeDerivativeCoefficients(const Derived &collocation_points) {

    /* Calculate a matrix of differences between the collocation points so that
     * dt(i,j) = c(i) - c(j)*/
    using Scalar = typename Derived::Scalar;
    using Index = size_t;
    const Index n_c = Derived::SizeAtCompileTime;
    Eigen::Matrix<typename Derived::Scalar, n_c, n_c> dt = collocation_points.replicate(1, n_c);
    dt -= dt.transpose().eval();

    /* Now calculate all of the coefficients */
    Eigen::Matrix<Scalar, n_c, n_c> derivative_coefficients;
    for (Index i = 0; i < n_c; ++i) {
        for (Index j = 0; j < n_c; ++j) {
            Scalar dc_j_i = 0;
            Scalar temp = 1;
            for (Index k = 0; k < n_c; ++k) {
                if (j != k) {
                    dc_j_i = (temp + dc_j_i * dt(i, k)) / dt(j, k);
                    temp = temp * dt(i, k) / dt(j, k);
                }
            }
            derivative_coefficients(j, i) = dc_j_i;
        }
    }
    return derivative_coefficients;
}

#endif /* UTILS_HEADER */