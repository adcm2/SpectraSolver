#ifndef ODE_COUPLED_SPECTRA_DRAFT_GUARD_H
#define ODE_COUPLED_SPECTRA_DRAFT_GUARD_H

#include <Eigen/Core>
#include <Eigen/Dense>

#include "FrequencySetup.h"

namespace SpectraSolver {

// draft class for coupled spectra solver
class ODE_Coupled_Spectra_Draft {
   public:
    using VECTOR = Eigen::VectorXcd;
    using MATRIX = Eigen::MatrixXcd;
    using COMPLEX = std::complex<double>;

    // ODE_Coupled_Spectra_Draft() {};
    // ~ODE_Coupled_Spectra_Draft() {};
    ODE_Coupled_Spectra_Draft(MATRIX &, MATRIX &, MATRIX &, freq_setup &);

    // returns of matrices
    MATRIX a0() const;
    MATRIX a1() const;
    MATRIX a2() const;

    // return of frequency setup
    freq_setup freq() const;

    // raw spectra function
    VECTOR
    rawspectra(const VECTOR &, const VECTOR &, const double) const;

   private:
    MATRIX &m_a0, &m_a1, &m_a2;
    std::size_t m_nelem;
    freq_setup m_freq;
};

// constructor
ODE_Coupled_Spectra_Draft::ODE_Coupled_Spectra_Draft(MATRIX &a0, MATRIX &a1,
                                                     MATRIX &a2,
                                                     freq_setup &freq0)
    : m_a0{a0}, m_a1{a1}, m_a2{a2} {
    m_nelem = a0.rows();
    m_freq = freq0;
};

// return definitions
ODE_Coupled_Spectra_Draft::MATRIX
ODE_Coupled_Spectra_Draft::a0() const {
    return m_a0;
}
ODE_Coupled_Spectra_Draft::MATRIX
ODE_Coupled_Spectra_Draft::a1() const {
    return m_a1;
}
ODE_Coupled_Spectra_Draft::MATRIX
ODE_Coupled_Spectra_Draft::a2() const {
    return m_a2;
}
freq_setup
ODE_Coupled_Spectra_Draft::freq() const {
    return m_freq;
}

ODE_Coupled_Spectra_Draft::VECTOR
ODE_Coupled_Spectra_Draft::rawspectra(
    const ODE_Coupled_Spectra_Draft::VECTOR &VR,
    const ODE_Coupled_Spectra_Draft::VECTOR &VS, const double soltol) const {
    // indices
    auto i1 = m_freq.i1();
    auto i2 = m_freq.i2();
    VECTOR tmp = VECTOR::Zero(m_freq.nt() / 2 + 1);

    //////////////////////////////////////////////////////////////////////////////////
    // some simple values
    COMPLEX myi(0.0, 1.0);
    double oneovertwopi = 1.0 / (2.0 * 3.1415926535897932);
    COMPLEX imep = static_cast<COMPLEX>(m_freq.ep());
    MATRIX A(m_nelem, m_nelem);

//////////////////////////////////////////////////////////////////////////////////
#pragma omp parallel private(A) shared(m_a0, m_a1, m_a2)
    {
#pragma omp for schedule(dynamic, 10)

        for (int idx = i1; idx < i2; ++idx) {
            // complex frequency
            COMPLEX winp = m_freq.w(idx) - myi * imep;

            // declare value of A
            A = m_a0 + winp * m_a1 + winp * winp * m_a2;

            //  rhs and guess
            VECTOR vrhs = VS / (myi * winp);
            VECTOR x0(m_nelem);

            // using BiCGSTAB solver from Eigen
            Eigen::BiCGSTAB<MATRIX, Eigen::DiagonalPreconditioner<COMPLEX>>
                solver;

            // set tolerance and compute
            solver.setTolerance(soltol);
            solver.compute(A);
            x0 = solver.preconditioner().solve(vrhs);

            // solve and return
            VECTOR vlhs = solver.solveWithGuess(vrhs, x0);

            // find acceleration response using receiver vectors
            tmp(idx) = -winp * winp * VR.transpose() * vlhs;
        };
    };
    return tmp;
};
}   // namespace SpectraSolver

#endif