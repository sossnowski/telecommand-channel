#ifndef BCH_MODULATION_H
#define BCH_MODULATION_H
#include <complex.h>


static const double complex ConstelationPoint0 = 0.707 + 0.707 *I; // for 00 binary value
static const double complex ConstelationPoint1 = 0.707 - 0.707 *I; // for 01 binary value
static const double complex ConstelationPoint2 = -0.707 + 0.707 *I; // for 10 binary value
static const double complex ConstelationPoint3 = -0.707 - 0.707 *I; // for 11 binary value

double complex* qpskModulation(unsigned int* CLTUs, int length);
unsigned int* qpskDemodulation(double complex* modulatedData, int length);
void printComplex(double complex* data, int length);

#endif //BCH_MODULATION_H
