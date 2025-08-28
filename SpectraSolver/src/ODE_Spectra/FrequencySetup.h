#ifndef NEW_FREQ_SETUP_GUARD_H
#define NEW_FREQ_SETUP_GUARD_H

#include <Eigen/IterativeLinearSolvers>
#include <cmath>
#include <iostream>
#include <vector>

namespace SpectraSolver {
// class for frequency setup
class freq_setup {
   public:
    // constructor
    freq_setup() {};
    ~freq_setup() {};
    freq_setup(double, double, double, double, double, double, double, double,
               int);

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
    // number of points
    int m_nt, m_nt0, m_i1, m_i2, m_i12, m_i22;

    std::vector<double> m_w;
    std::vector<double> m_t;
};

// constructor
freq_setup::freq_setup(double f1, double f2, double dt, double tout, double df0,
                       double wtb, double t1, double t2, int qex)
    : m_f1{f1 / 1000.0},
      m_f2{f2 / 1000.0},
      m_dt{dt},
      m_df0{df0 / 1000.0},
      m_tout{tout * 3600.0},
      m_t1{t1 * 3600.0},
      m_t2{std::min(t2, tout) * 3600.0},
      m_wtb{wtb * 3.1415926535 / 500.0} {
    // Nyquist frequency
    double fn = 0.5 / dt;

    // check for f2
    if (fn < m_f2) {
        std::cout << "f2 is greater than the Nyquist frequency for the time "
                     "step. Behaviour may be unexpected"
                  << std::endl;
        m_f2 = fn;
    };

    // multipliers for epsilon in inverse FL and q (need to clarify this)
    int mex = 5;
    // int qex = 4;
    m_ep = mex / m_tout;

    // David's version
    m_df = m_ep / (6.28318530718 * qex);

    // my version
    // df = 1.0 / this->tout;             // find df
    m_nt = std::ceil(1.0 / (m_df * m_dt));   // find nt
    int ne = static_cast<int>(log(static_cast<double>(m_nt)) / log(2.0) + 1);
    m_nt = pow(2, ne);   // finish increase in density
    std::cout << "nt: " << m_nt << std::endl;
    m_df = 1.0 / (m_nt * m_dt);   // new df

    // finding new values of f1 and f2 and corresponding integer numbers
    m_i1 = std::max(static_cast<int>(std::floor(m_f1 / m_df)), 0);
    m_i2 = std::min(static_cast<int>(std::floor(m_f2 / m_df)) + 2, m_nt);
    m_f1 = (m_i1 - 1) * m_df;
    m_f2 = (m_i2 - 1) * m_df;

    // fill out w, remembering w = 2pi f, and only needing to go up to nt/2+1 as
    // half length of time series
    for (int idx = 0; idx < m_nt / 2 + 1; ++idx) {
        m_w.push_back(2.0 * 3.1415926535 * m_df * static_cast<double>(idx));
    }

    // fill out t
    for (int idx = 0; idx < m_nt; ++idx) {
        m_t.push_back(m_dt * static_cast<double>(idx));
    }
    m_nt0 = floor(1.0 / m_df0 * m_dt);
    if (m_nt0 > m_nt) {
        int ne2 = log(static_cast<double>(m_nt0)) / log(2.0) + 1;
        m_nt0 = pow(2, ne2);
    } else {
        m_nt0 = m_nt;
    }

    m_df2 = 1.0 / (m_nt0 * m_dt);
    m_i12 = std::max(static_cast<int>(floor(m_f1 / m_df2)) - 1, 0);
    m_i22 = static_cast<int>(floor(m_f2 / m_df2)) + 1;
};

// returns
// double functions
double
freq_setup::f(int idx) const {
    return static_cast<double>(idx) * m_df;
}
double
freq_setup::f0(int idx) const {
    return static_cast<double>(idx) * m_df0;
}
double
freq_setup::f2(int idx) const {
    return static_cast<double>(idx) * m_df2;
}
double
freq_setup::f1() const {
    return m_f1;
}
double
freq_setup::f2() const {
    return m_f2;
}
double
freq_setup::tout() const {
    return m_tout;
}
double
freq_setup::df() const {
    return m_df;
}
double
freq_setup::df0() const {
    return m_df0;
}
double
freq_setup::df2() const {
    return m_df2;
}
double
freq_setup::wtb() const {
    return m_wtb;
}
double
freq_setup::t1() const {
    return m_t1;
}
double
freq_setup::t2() const {
    return m_t2;
}
double
freq_setup::ep() const {
    return m_ep;
}
double
freq_setup::dt() const {
    return m_dt;
}

// integer functions
int
freq_setup::nt() const {
    return m_nt;
}
int
freq_setup::nt0() const {
    return m_nt0;
}
int
freq_setup::i1() const {
    return m_i1;
}
int
freq_setup::i2() const {
    return m_i2;
}
int
freq_setup::i12() const {
    return m_i12;
}
int
freq_setup::i22() const {
    return m_i22;
}

// vector returns
std::vector<double>
freq_setup::w() const {
    return m_w;
}
std::vector<double>
freq_setup::t() const {
    return m_t;
}
double
freq_setup::w(int idx) const {
    return m_w[idx];
}
double
freq_setup::t(int idx) const {
    return m_t[idx];
}

}   // namespace SpectraSolver

#endif