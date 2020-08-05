#include "modulation.h"
#include <stdlib.h>
#include <stdio.h>


double complex* qpskModulation(unsigned int* CLTUs, int length) {
    double complex* modulatedData = malloc(sizeof(double complex) * 32 * length / 2); //one complex number per two bits from cltu
    unsigned int firstFlag = 1;
    unsigned int secondFlag = 1;
    int complexCounter = 0;
    secondFlag = secondFlag << 1;
    for (int i = 0; i < length ; ++i) {
        for (int j = 0; j < 16; ++j) {
            if ((CLTUs[i] & firstFlag) == 0 && (CLTUs[i] & secondFlag) == 0) {
                modulatedData[complexCounter] = ConstelationPoint0;
            } else if ((CLTUs[i] & firstFlag) != 0 && (CLTUs[i] & secondFlag) == 0) {
                modulatedData[complexCounter] = ConstelationPoint1;
            } else if ((CLTUs[i] & firstFlag) == 0 && (CLTUs[i] & secondFlag) != 0) {
                modulatedData[complexCounter] = ConstelationPoint2;
            } else {
                modulatedData[complexCounter] = ConstelationPoint3;
            }
            firstFlag = firstFlag << 2;
            secondFlag = secondFlag << 2;
            complexCounter++;
        }
        firstFlag = 1;
        secondFlag = 1;
        secondFlag = secondFlag << 1;
    }
    return modulatedData;
}


unsigned int* qpskDemodulation(double complex* modulatedData, int modulatedDatalength) {
    printf("\ndata length %d", modulatedDatalength);
    unsigned int* demodulatedData = malloc(sizeof(unsigned int) * modulatedDatalength / 16 );
    unsigned int demodulatedByteValue = 0;
    unsigned int constelationPoint0Value = 0;
    unsigned int constelationPoint1Value = 1;
    unsigned int constelationPoint2Value = 2;
    unsigned int constelationPoint3Value = 3;
    int demodulatedByteCounter = 0;


    for (int i = 0; i <= modulatedDatalength; ++i) {
        if ((i % 16) == 0 && i != 0) {
            demodulatedData[demodulatedByteCounter++] = demodulatedByteValue;
            demodulatedByteValue = 0;
            constelationPoint0Value = 0;
            constelationPoint1Value = 1;
            constelationPoint2Value = 2;
            constelationPoint3Value = 3;
            if (i == modulatedDatalength) break;
        }

        if (modulatedData[i] == ConstelationPoint0) {
            demodulatedByteValue = demodulatedByteValue | constelationPoint0Value;
        } else if (modulatedData[i] == ConstelationPoint1) {
            demodulatedByteValue = demodulatedByteValue | constelationPoint1Value;
        } else if (modulatedData[i] == ConstelationPoint2) {
            demodulatedByteValue = demodulatedByteValue | constelationPoint2Value;
        } else {
            demodulatedByteValue = demodulatedByteValue | constelationPoint3Value;
        }
        constelationPoint0Value = constelationPoint0Value << 2;
        constelationPoint1Value = constelationPoint1Value << 2;
        constelationPoint2Value = constelationPoint2Value << 2;
        constelationPoint3Value = constelationPoint3Value << 2;
    }
    return demodulatedData;
}


void printComplex(double complex* array, int length) {
    for (int i = 0; i < length; ++i) {
        printf("%f %f |", creal(array[i]), cimag(array[i]));
    }
}

