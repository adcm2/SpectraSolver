#ifndef FrequencyFullConstructor_H
#define FrequencyFullConstructor_H

namespace SpectraSolver {
// constructor
// f1, f2 in mHz, dt in s, tout in hours, df0 in mHz, wtb in mHz, t1 and t2 in
// hours qex is an integer to increase frequency resolution
FreqFull::FreqFull(double f1, double f2, double dt, double tout, double df0,
                   double wtb, double t1, double t2, int qex, double TimeNorm)
    : m_f1{f1 / 1000.0 * TimeNorm},
      m_f2{f2 / 1000.0 * TimeNorm},
      m_dt{dt / TimeNorm},
      m_df0{df0 / 1000.0 * TimeNorm},
      m_tout{tout * 3600.0 / TimeNorm},
      m_t1{t1 * 3600.0 / TimeNorm},
      m_t2{std::min(t2, tout) * 3600.0 / TimeNorm},
      m_wtb{wtb * 3.1415926535 / 500.0 * TimeNorm},
      m_timenorm{TimeNorm},
      m_frequencynorm{1.0 / TimeNorm} {
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    // calculating and storing in s and Hz
    //  Nyquist frequency
    double fn = 0.5 / m_dt;
    // std::cout << "df: " << m_df << std::endl;
    // std::cout << "f1: " << m_f1 << ", f2: " << m_f2 << ", fn: " << fn
    //           << ", dt: " << m_dt << ", tout: " << m_tout << std::endl;
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
    // std::cout << "nt: " << m_nt << std::endl;
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

    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
};
}   // namespace SpectraSolver
#endif