#ifndef EIGEN_DONT_PARALLELIZE
#define EIGEN_DONT_PARALLELIZE
#endif

#include <SpectraSolver/All>
#include <iostream>

int
main() {
    double x = 3.0;
    std::cout << "Hello World! " << x << std::endl;

    double f1, f2, dt, tout, df0, wtb, t1, t2, soltol;
    int qex;
    f1 = 0.2;
    f2 = 4.0;
    dt = 20;
    tout = 256;
    df0 = 0.2;
    wtb = 0.05;
    t1 = 0;
    t2 = 60;
    soltol = 1e-6;
    qex = 4;

    // getting setup of frequencies etc used in idsm
    freq_setup myfreq(f1, f2, dt, tout, df0, wtb, t1, t2, qex);
    std::cout << "f1: " << myfreq.f1() << ", f2: " << myfreq.f2()
              << ", df: " << myfreq.df() << ", nt: " << myfreq.nt() << "\n";            
    return 0;
}
