#ifndef EIGEN_DONT_PARALLELIZE
#define EIGEN_DONT_PARALLELIZE
#endif

#include <SpectraSolver/FF>
#include <iostream>

int
main() {
    // Example initialization of FreqFull
    double f1 = 0.2, f2 = 4.0, dt = 20, tout = 256, df0 = 0.2, wtb = 0.05,
           t1 = 0, t2 = 60;
    int qex = 4;

    SpectraSolver::FreqFull myff(f1, f2, dt, tout, df0, wtb, t1, t2, qex);

    std::cout << "FreqFull object created.\n";
    // Optionally, print some member or call a method if available
    // std::cout << "f1: " << myff.f1() << std::endl;

    return 0;
}