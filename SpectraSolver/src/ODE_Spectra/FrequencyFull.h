#ifndef NEW_FREQ_FULL_GUARD_H
#define NEW_FREQ_FULL_GUARD_H

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <cmath>
#include <iostream>
#include <vector>

namespace SpectraSolver {
// class for frequency setup
class FreqFull {
   public:
    using VECTOR = Eigen::VectorXcd;
    using MATRIX = Eigen::MatrixXcd;
    using COMPLEX = std::complex<double>;

    // constructor
    FreqFull() {};
    ~FreqFull() {};
    FreqFull(double, double, double, double, double, double, double, double,
             int);

    // spectra
    VECTOR Spectra_Raw(const MATRIX &, const MATRIX &, const MATRIX &,
                       const VECTOR &, const VECTOR &, const double) const;
    VECTOR Spectra_Raw_NoCoriolis(const MATRIX &, const MATRIX &,
                                  const VECTOR &, const VECTOR &,
                                  const double) const;

    // double functions
    double f(int) const;
    double f0(int) const;
    double f2(int) const;
    double f1() const;
    double f2() const;
    double tout() const;
    double df() const;
    double df0() const;
    double df2() const;
    double wtb() const;
    double t1() const;
    double t2() const;
    double ep() const;
    double dt() const;

    // integer functions
    int nt() const;
    int nt0() const;
    int i1() const;
    int i2() const;
    int i12() const;
    int i22() const;

    // vector returns
    std::vector<double> w() const;
    std::vector<double> t() const;
    double w(int) const;
    double t(int) const;

   private:
    double m_f1, m_f2, m_tout, m_df0, m_wtb, m_t1, m_t2, m_df, m_ep, m_df2,
        m_dt;
    int m_nt, m_nt0, m_i1, m_i2, m_i12, m_i22;

    std::vector<double> m_w;
    std::vector<double> m_t;
};

}   // namespace SpectraSolver

#endif