#ifndef FrequencyFullDefinitions_H
#define FrequencyFullDefinitions_H

#include <cassert>
#include <iomanip>

#include "BlockPreconditioner.h"
#include "matrix_rcopy.h"

namespace SpectraSolver {
// returns
// double functions
double
FreqFull::f(int idx) const {
    return static_cast<double>(idx) * m_df;
}
double
FreqFull::f0(int idx) const {
    return static_cast<double>(idx) * m_df0;
}
double
FreqFull::f2(int idx) const {
    return static_cast<double>(idx) * m_df2;
}
double
FreqFull::f1() const {
    return m_f1;
}
double
FreqFull::f2() const {
    return m_f2;
}
double
FreqFull::tout() const {
    return m_tout;
}
double
FreqFull::df() const {
    return m_df;
}
double
FreqFull::df0() const {
    return m_df0;
}
double
FreqFull::df2() const {
    return m_df2;
}
double
FreqFull::wtb() const {
    return m_wtb;
}
double
FreqFull::t1() const {
    return m_t1;
}
double
FreqFull::t2() const {
    return m_t2;
}
double
FreqFull::ep() const {
    return m_ep;
}
double
FreqFull::dt() const {
    return m_dt;
}

// integer functions
int
FreqFull::nt() const {
    return m_nt;
}
int
FreqFull::nt0() const {
    return m_nt0;
}
int
FreqFull::i1() const {
    return m_i1;
}
int
FreqFull::i2() const {
    return m_i2;
}
int
FreqFull::i12() const {
    return m_i12;
}
int
FreqFull::i22() const {
    return m_i22;
}

// vector returns
std::vector<double>
FreqFull::w() const {
    return m_w;
}
std::vector<double>
FreqFull::t() const {
    return m_t;
}
double
FreqFull::w(int idx) const {
    return m_w[idx];
}
double
FreqFull::t(int idx) const {
    return m_t[idx];
}

// only including one source at the moment
FreqFull::MATRIX
FreqFull::Spectra_Raw(const MATRIX &a0, const MATRIX &a1, const MATRIX &a2,
                      const MATRIX &VR, const VECTOR &VS,
                      const double soltol) const {
    // check sizes with assertions
    assert((a0.rows() == a0.cols()) && "a0 not square");
    assert((a1.rows() == a1.cols()) && "a1 not square");
    assert((a2.rows() == a2.cols()) && "a2 not square");
    assert(((a0.rows() == a1.rows()) && (a0.rows() == a2.rows())) &&
           "Different matrix sizes");
    assert((a0.rows() == VR.rows()) && "VR wrong size");
    assert((a0.rows() == VS.rows()) && "VS wrong size");

    // indices
    auto nm = a0.rows();
    std::size_t nr = VR.cols();
    MATRIX tmp = MATRIX::Zero(nr, m_nt / 2 + 1);
    // VECTOR tmp = VECTOR::Zero(m_nt / 2 + 1);
    //////////////////////////////////////////////////////////////////////////////////
    // some simple values
    COMPLEX myi(0.0, 1.0);
    double oneovertwopi = 1.0 / (2.0 * 3.1415926535897932);
    COMPLEX imep = static_cast<COMPLEX>(m_ep);
    // MATRIX A(nm, nm);

    //////////////////////////////////////////////////////////////////////////////////
    // #pragma omp parallel private(A) shared(a0, a1, a2)
    //     {
    // #pragma omp for schedule(dynamic, 10)
    for (int idx = m_i1; idx < m_i2; ++idx) {
        // HACKY: just to see values
        // complex frequency
        COMPLEX winp = m_w[idx] - myi * imep;

        // declare value of A
        MATRIX A = a0 + winp * a1 - winp * winp * a2;

        //  rhs and guess
        VECTOR vrhs = VS / (myi * winp);

        // using BiCGSTAB solver from Eigen
        Eigen::BiCGSTAB<MATRIX, Eigen::DiagonalPreconditioner<COMPLEX>> solver;

        // set tolerance and compute
        solver.setTolerance(soltol);
        solver.compute(A);
        VECTOR x0 = solver.preconditioner().solve(vrhs);

        // solve and return
        VECTOR vlhs = solver.solveWithGuess(vrhs, x0);

        // find acceleration response using receiver vectors
        tmp.block(0, idx, nr, 1) = -winp * winp * VR.transpose() * vlhs;
    };
    return tmp;
};

FreqFull::MATRIX
FreqFull::Spectra_Raw_Low_Memory(const MATRIX &a0, const MATRIX &a1,
                                 const MATRIX &a2, const MATRIX &VR,
                                 const VECTOR &VS, const double soltol) const {
    // check sizes with assertions
    assert((a0.rows() == a0.cols()) && "a0 not square");
    assert((a1.rows() == a1.cols()) && "a1 not square");
    assert((a2.rows() == a2.cols()) && "a2 not square");
    assert(((a0.rows() == a1.rows()) && (a0.rows() == a2.rows())) &&
           "Different matrix sizes");
    assert((a0.rows() == VR.rows()) && "VR wrong size");
    assert((a0.rows() == VS.rows()) && "VS wrong size");

    // indices
    auto nm = a0.rows();
    std::size_t nr = VR.cols();
    MATRIX tmp = MATRIX::Zero(nr, m_nt / 2 + 1);
    // VECTOR tmp = VECTOR::Zero(m_nt / 2 + 1);
    //////////////////////////////////////////////////////////////////////////////////
    // some simple values
    COMPLEX myi(0.0, 1.0);
    // double oneovertwopi = 1.0 / (2.0 * 3.1415926535897932);
    COMPLEX imep = static_cast<COMPLEX>(m_ep);
    // MATRIX A(nm, nm);

    //////////////////////////////////////////////////////////////////////////////////
    // #pragma omp parallel private(A) shared(a0, a1, a2)
    //     {
    // #pragma omp for schedule(dynamic, 10)
    MatrixReplaceFT<COMPLEX> A(a0, a1, a2, 0.0);
    Eigen::BiCGSTAB<MatrixReplaceFT<COMPLEX>,
                    Eigen::BlockMatFreePreconditioner<COMPLEX>>
        solver;

    // set tolerance and compute
    solver.setTolerance(soltol);
    for (int idx = m_i1; idx < m_i2; ++idx) {
        // COMPLEX myvali = COMPLEX(0.0, -0.2);
        COMPLEX winp = m_w[idx] - myi * imep;
        // COMPLEX winp = m_w[idx] + myvali;

        // if (idx == m_i1) {
        //     std::cout << std::setprecision(15) << idx << " " << winp << "\n";
        // }

        // complex frequency

        A.UpdateFrequency(winp);
        // declare value of A
        // MATRIX A = a0 + winp * a1 - winp * winp * a2;

        //  rhs and guess
        VECTOR vrhs = VS / (myi * winp);
        // VECTOR vrhs = VS;

        // using BiCGSTAB solver from Eigen
        solver.compute(A);
        VECTOR x0 = solver.preconditioner().solve(vrhs);

        // solve and return
        // VECTOR vlhs = solver.solve(vrhs);
        // VECTOR vlhs = solver.solveWithGuess(vrhs, x0);
        VECTOR vlhs = solver.solve(vrhs);

        // find acceleration response using receiver vectors
        tmp.block(0, idx, nr, 1) = -winp * winp * VR.transpose() * vlhs;
        // tmp.block(0, idx, nr, 1) = -VR.transpose() * vlhs;
        // if (std::abs(idx - m_i1) < 5) {
        //     std::cout << std::setprecision(15) << idx << " "
        //               << VR.transpose() * vlhs << "\n";
        // }
        // if (std::abs(idx - m_i2) < 5) {
        //     std::cout << std::setprecision(15) << idx << " " << winp.real()
        //               << "  " << winp.imag() << "\n";
        // }
    };
    return tmp;
};

FreqFull::MATRIX
FreqFull::Spectra_Raw_No_Coriolis_Low_Memory(const MATRIX &a0, const MATRIX &a2,
                                             const MATRIX &VR, const VECTOR &VS,
                                             const double soltol) const {
    MATRIX a1 = MATRIX::Zero(a0.rows(), a0.cols());
    return Spectra_Raw_Low_Memory(a0, a1, a2, VR, VS, soltol);
};

FreqFull::MATRIX
FreqFull::Spectra_Raw_NoCoriolis(const MATRIX &a0, const MATRIX &a2,
                                 const MATRIX &VR, const VECTOR &VS,
                                 const double soltol) const {
    // check sizes with assertions
    assert((a0.rows() == a0.cols()) && "a0 not square");
    // assert((a1.rows() == a1.cols()) && "a1 not square");
    assert((a2.rows() == a2.cols()) && "a2 not square");
    assert(((a0.rows() == a2.rows())) && "Different matrix sizes");
    assert((a0.rows() == VR.rows()) && "VR wrong size");
    assert((a0.rows() == VS.rows()) && "VS wrong size");

    // indices
    auto nm = a0.rows();
    std::size_t nr = VR.cols();
    MATRIX tmp = MATRIX::Zero(nr, m_nt / 2 + 1);
    // VECTOR tmp = VECTOR::Zero(m_nt / 2 + 1);
    //////////////////////////////////////////////////////////////////////////////////
    // some simple values
    COMPLEX myi(0.0, 1.0);
    // double oneovertwopi = 1.0 / (2.0 * 3.1415926535897932);
    COMPLEX imep = static_cast<COMPLEX>(m_ep);
    // MATRIX A(nm, nm);

    //////////////////////////////////////////////////////////////////////////////////
    // #pragma omp parallel private(A) shared(a0, a1, a2)
    //     {
    // #pragma omp for schedule(dynamic, 10)
    for (int idx = m_i1; idx < m_i2; ++idx) {
        // HACKY: just to see values
        // complex frequency
        COMPLEX winp = m_w[idx] - myi * imep;

        // declare value of A
        MATRIX A = a0 - winp * winp * a2;

        //  rhs and guess
        VECTOR vrhs = VS / (myi * winp);

        // using BiCGSTAB solver from Eigen
        Eigen::BiCGSTAB<MATRIX, Eigen::DiagonalPreconditioner<COMPLEX>> solver;

        // set tolerance and compute
        solver.setTolerance(soltol);
        solver.compute(A);
        VECTOR x0 = solver.preconditioner().solve(vrhs);

        // solve and return
        VECTOR vlhs = solver.solveWithGuess(vrhs, x0);

        // find acceleration response using receiver vectors
        tmp.block(0, idx, nr, 1) = -winp * winp * VR.transpose() * vlhs;
    };
    return tmp;
};

FreqFull::MATRIX
FreqFull::Spectra_Raw_NoCoriolis_LU(const MATRIX &a0, const MATRIX &a2,
                                    const MATRIX &VR, const VECTOR &VS) const {
    // check sizes with assertions
    assert((a0.rows() == a0.cols()) && "a0 not square");
    // assert((a1.rows() == a1.cols()) && "a1 not square");
    assert((a2.rows() == a2.cols()) && "a2 not square");
    assert(((a0.rows() == a2.rows())) && "Different matrix sizes");
    assert((a0.rows() == VR.rows()) && "VR wrong size");
    assert((a0.rows() == VS.rows()) && "VS wrong size");

    // indices
    auto nm = a0.rows();
    std::size_t nr = VR.cols();
    MATRIX tmp = MATRIX::Zero(nr, m_nt / 2 + 1);
    // VECTOR tmp = VECTOR::Zero(m_nt / 2 + 1);
    //////////////////////////////////////////////////////////////////////////////////
    // some simple values
    COMPLEX myi(0.0, 1.0);
    // double oneovertwopi = 1.0 / (2.0 * 3.1415926535897932);
    COMPLEX imep = static_cast<COMPLEX>(m_ep);
    COMPLEX myvali = COMPLEX(0.0, -0.2);
    // MATRIX A(nm, nm);

    //////////////////////////////////////////////////////////////////////////////////
    // #pragma omp parallel private(A) shared(a0, a1, a2)
    //     {
    // #pragma omp for schedule(dynamic, 10)
    for (int idx = m_i1; idx < m_i2; ++idx) {
        // HACKY: just to see values
        // complex frequency
        COMPLEX winp = m_w[idx] - myi * imep;
        // COMPLEX winp = m_w[idx] + myvali;

        // declare value of A
        MATRIX A = a0 - winp * winp * a2;

        //  rhs and guess
        VECTOR vrhs = VS / (myi * winp);

        // using BiCGSTAB solver from Eigen
        Eigen::FullPivLU<MATRIX> solver;

        // set tolerance and compute
        // solver.setTolerance(soltol);
        solver.compute(A);
        // VECTOR x0 = solver.preconditioner().solve(vrhs);

        // solve and return
        // VECTOR vlhs = solver.solveWithGuess(vrhs, x0);
        VECTOR vlhs = solver.solve(vrhs);

        // find acceleration response using receiver vectors
        tmp.block(0, idx, nr, 1) = -winp * winp * VR.transpose() * vlhs;

        // if ((idx > 9780) && (idx < 9785)) {
        //     std::cout << std::setprecision(15) << idx << " "
        //               << VR.transpose() * vlhs << "\n";
        // }
    };
    return tmp;
};
}   // namespace SpectraSolver
#endif