#ifndef BCH_MODULATION_H
#define BCH_MODULATION_H
#include <complex.h>


static const double complex ConstelationPoint0 = 0.707 + 0.707 *I; // dla wartości binarnej 00
static const double complex ConstelationPoint1 = 0.707 - 0.707 *I; // dla wartości binarnej 01
static const double complex ConstelationPoint2 = -0.707 + 0.707 *I; // dla wartości binarnej 10
static const double complex ConstelationPoint3 = -0.707 - 0.707 *I; // dla wartości binarnej 11

static const int oversamplingLevel = 2;
static const int bitrate = 2; // kbits (1, 2, 4)
static const int subcarrierFreq = 8; //kHz ( 8 lub 16)

static const double complex FourSamples[] = {1 + 0*I, 0 + 1*I, -1 + 0*I, 0 - 1*I};

static const double complex EightSamples[] = {1 + 0*I, 0.707 + 0.707 *I, 0 + 1*I, -0.707 + 0.707 *I,
                                            -1 + 0*I, -0.707 - 0.707 *I, 0 - 1*I, 0.707 - 0.707 *I};

static const double complex SixteenPoints[] = {1 + 1*I, 0.924 + 0.383*I, 0.707 + 0.707*I, 0.383 + 0.924*I, 0 + 1*I,
                                             -0.383 + 0.924*I, -0.707 + 0.707*I, -0.924 + 0.383*I, -1 + 0*I,
                                             -0.924 - 0.383*I, -0.707 - 0.707*I, -0.383 - 0.924*I, 0 - 1*I,
                                             0.383 - 0.924*I, 0.707 - 0.707*I, 0.924 - 0.383*I};

static const double complex ThirtyTwoPoints[] = {1 + 0*I, 0.981 + 0.199*I, 0.924 + 0.383*I, 0.831 + 0.556*I,
                                                0.707 + 0.707 *I, 0.556 + 0.831*I, 0.383 + 0.924*I, 0.199 + 0.981*I,
                                                0 + 1*I, -0.199 + 0.981*I, -0.383 + 0.924*I, -0.556 + 0.831*I,
                                                -0.707 + 0.707*I, -0.831 + 0.556*I, -0.924 + 0.383*I, -0.981 + 0.199*I,
                                                -1 + 0*I, -0.981 - 0.199*I, -0.924 - 0.383*I, -0.831 - 0.556*I,
                                                -0.707 - 0.707 *I, -0.556 - 0.831*I, -0.383 - 0.924*I, -0.199 - 0.981*I,
                                                0 - 1*I, 0.199 - 0.981*I, 0.383 - 0.924*I, 0.556 - 0.831*I,
                                                0.707 - 0.707*I, 0.831 - 0.556*I, 0.924 - 0.383*I, 0.981 - 0.199*I};

double complex* qpskModulation(unsigned int* CLTUs, int length);
unsigned int* qpskDemodulation(double complex* modulatedData, int length);
double complex* oversampling(double complex* modulatedData, int length);
double complex* filtering(double complex* oversampledModulatedData, int length);
void printComplex(double complex* data, int length);
int countNumberOfSamplesPerPeriod();
double complex* analogPhaseModulation(unsigned int* CLTUs, int length);
unsigned int* analogPhaseDemodulation(double complex* modulatedData, int length);


#endif //BCH_MODULATION_H
