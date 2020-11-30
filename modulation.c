#include "modulation.h"
#include <stdlib.h>
#include <stdio.h>


/**
 * Funkcja moduluje ciąg danych
 *
 * @param CLTUs ciąg bloków CLTU
 * @return modulatedData zmodulowany ciąg danych
 */
double complex* qpskModulation(unsigned int* CLTUs, int length) {
    double complex* modulatedData = malloc(sizeof(double complex) * 32 * length / 2); //jedna liczba zespolona na każde dwa bity danych
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

/**
 * Funkcja demoduluje ciąg danych
 *
 * @param CLTUs zmidulowany ciąg danych
 * @return zdemodulowany ciąg danych
 */
unsigned int* qpskDemodulation(double complex* modulatedData, int modulatedDatalength) {
    unsigned int* demodulatedData = malloc(sizeof(unsigned int) * modulatedDatalength / 16 );
    unsigned int demodulatedByteValue = 0;
    unsigned int constelationPoint0Value = 0; // bitowo 00
    unsigned int constelationPoint1Value = 1; // bitowo 01
    unsigned int constelationPoint2Value = 2; // bitowo 10
    unsigned int constelationPoint3Value = 3; // bitowo 11
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

/**
 * Funkcja przeprowadz nadpróbkowanie danych
 *
 * @param modulatedData ciąg danych wejściowych
 * @param length długość wejściowego ciągu danych
 *
 * @return oversampledData nadpróbkowany ciąg danych
 */
double complex* oversampling(double complex* modulatedData, int length) {
    double complex* oversampledData = malloc(sizeof(double complex) * length * oversamplingLevel);
    for(int i = 0; i < length; ++i) {
        oversampledData[i * oversamplingLevel] = modulatedData[i];
        for (int j = 1; j < oversamplingLevel; ++j) {
            oversampledData[i * oversamplingLevel + j] = ConstelationPoint0;
        }
    }
    return oversampledData;
}

double complex* filtering(double complex* oversampledModulatedData, int length) {
    double complex* modulatedDataAfterFiltering = malloc(sizeof(double complex) * length / oversamplingLevel);
    for (int i = 0; i < length; ++i) {
        modulatedDataAfterFiltering[i / oversamplingLevel] = oversampledModulatedData[i];
        i += oversamplingLevel - 1;
    }
    return modulatedDataAfterFiltering;
}

int countNumberOfSamplesPerPeriod() {
    return subcarrierFreq / 2 * bitrate / 2;
}

double complex* getSamplesValues(int numberOfSamplesPerPeriod) {
    const double complex* samplesValues;
    if (numberOfSamplesPerPeriod == 4) {
        samplesValues = FourSamples;
    } else if (numberOfSamplesPerPeriod == 8 ) {
        samplesValues = EightSamples;
    } else if (numberOfSamplesPerPeriod == 16 ) {
        samplesValues = SixteenPoints;
    } else {
        samplesValues = ThirtyTwoPoints;
    }

    return samplesValues;
}

double complex* analogPhaseModulation(unsigned int* CLTUs, int length) {
    const int numberOfSamplesPerPeriod = countNumberOfSamplesPerPeriod();
    double complex* samplesValues = getSamplesValues(numberOfSamplesPerPeriod);
    const int outputDataLength = length * numberOfSamplesPerPeriod * 32;
    double complex* outputData = malloc(sizeof(double complex) * outputDataLength);
    int outputDataCounter = 0;
    unsigned int flag = 1;
    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < 32; ++j) {
            if ((CLTUs[i] & flag) == 0) {
                for (int sampleNumber = 0; sampleNumber < numberOfSamplesPerPeriod; ++sampleNumber) {
                    outputData[outputDataCounter] = samplesValues[sampleNumber] * -1;
                    outputDataCounter++;
                }
            } else {
                for (int sampleNumber = 0; sampleNumber < numberOfSamplesPerPeriod; ++sampleNumber) {
                    outputData[outputDataCounter] = samplesValues[sampleNumber];
                    outputDataCounter++;
                }
            }
            flag = flag << 1;
        }
        flag = 1;
    }

    return outputData;
}

unsigned int* analogPhaseDemodulation(double complex* modulatedData, int modulatedDataBytesNumber) {
    const int numberOfSamplesPerPeriod = countNumberOfSamplesPerPeriod();
    double complex* samplesValues = getSamplesValues(numberOfSamplesPerPeriod);
    unsigned int* demodulatedData = malloc(sizeof(unsigned int) * modulatedDataBytesNumber);
    int counter = 0;
    unsigned int demodulatedByteValue = 0;
    unsigned int bit0Value = 0;
    unsigned int bit1Value = 1;
    int demodulatedDataCounter = 0;
    int modulatedDataLength = modulatedDataBytesNumber * 32 * numberOfSamplesPerPeriod;

    for (int i = 0; i < modulatedDataLength; i+= numberOfSamplesPerPeriod) {
        if (modulatedData[i] == samplesValues[0]) {
            demodulatedByteValue = demodulatedByteValue | bit1Value;
        } else {
            demodulatedByteValue = demodulatedByteValue | bit0Value;
        }

        bit1Value = bit1Value << 1;
        counter++;

        if (counter == 32 ) {
            demodulatedData[demodulatedDataCounter++] = demodulatedByteValue;
            counter = 0;
            demodulatedByteValue = 0;
            bit1Value = 1;
        }

    }

    return demodulatedData;
}


void printComplex(double complex* array, int length) {
    for (int i = 0; i < length; ++i) {
        printf("%f %f |", creal(array[i]), cimag(array[i]));
    }
}

