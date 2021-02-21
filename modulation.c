#include "modulation.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


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
    //
    const double real = cos(angle);
    const double imaginary = sin(angle);
    double complex offsetComplex = real + I * imaginary;
    //
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
            // zmiana probek jako symulacja zaklocen w kanale transmisji
            //modulatedData[complexCounter] /= 2;
            modulatedData[complexCounter] *= offsetComplex;

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
unsigned int* qpskDemodulation(double complex* data, int length) {
    const double real = cos(angle);
    const double imaginary = sin(angle * -1);
    double complex offsetComplex = real + I * imaginary;
    float numberOfInts =(float) length / 32;
    int parsedNumberOfInts = numberOfInts;
    if (numberOfInts > parsedNumberOfInts) parsedNumberOfInts += 1;
    unsigned int* demodulatedData = malloc(sizeof(complex double) * parsedNumberOfInts );
    // shifting values
    for (int i = 0; i < length; ++i) {
        //printf("%f %f | ", creal(data[i]), cimag(data[i]));
        data[i] = data[i] * offsetComplex;
        //printf("%f %f || ", creal(data[i]), cimag(data[i]));
    }

    double sum = 0;
    for (int i = 0; i < numberOfSamplesToComputeAmplitudeFactor; ++i) {
        sum += 1 / sqrt(creal(data[i]) * creal(data[i]) + cimag(data[i]) * cimag(data[i]));
    }
    double amplitudeFactor = sum / numberOfSamplesToComputeAmplitudeFactor;
    //printf("\n %f \n", amplitudeFactor);

    unsigned int demodulatedValue = 0;
    int counter = 0;
    unsigned int constelationPoint0Value = 0; // bitowo 00
    unsigned int constelationPoint1Value = 1; // bitowo 01
    unsigned int constelationPoint2Value = 2; // bitowo 10
    unsigned int constelationPoint3Value = 3; // bitowo 11
    // scale amplitude to 1
    for (int i = 0; i < length; ++i) {
        data[i] *= amplitudeFactor;

        //printf("%f %f || \n", creal(data[i]), cimag(data[i]));
        if (i != 0 && (i % 16) == 0) {
            demodulatedData[counter++] = demodulatedValue;
            demodulatedValue = 0;
            constelationPoint0Value = 0;
            constelationPoint1Value = 1;
            constelationPoint2Value = 2;
            constelationPoint3Value = 3;
        }
        // pierwsza cwiartka ukladu
        if (creal(data[i]) > 0 && cimag(data[i]) > 0) {
            demodulatedValue = demodulatedValue | constelationPoint0Value;
        } else if (creal(data[i]) > 0 && cimag(data[i]) < 0) {
            demodulatedValue = demodulatedValue | constelationPoint1Value;
        } else if (creal(data[i]) < 0 && cimag(data[i]) > 0) {
            demodulatedValue = demodulatedValue | constelationPoint2Value;
        } else if (creal(data[i]) < 0 && cimag(data[i]) < 0) {
            demodulatedValue = demodulatedValue | constelationPoint3Value;
        }

        constelationPoint0Value = constelationPoint0Value << 2;
        constelationPoint1Value = constelationPoint1Value << 2;
        constelationPoint2Value = constelationPoint2Value << 2;
        constelationPoint3Value = constelationPoint3Value << 2;
    }
    demodulatedData[counter] = demodulatedValue;

    return demodulatedData;

}

int countNumberOfSamplesPerPeriod() {
    float numberOfSamples = (float) (subcarrierFreq + modulatedSignalFactor * bitrate) * 2 / subcarrierFreq;
    int numberOfSamplesInt = (int) numberOfSamples;
    if (numberOfSamples > numberOfSamplesInt) numberOfSamplesInt++;
    return numberOfSamplesInt;
}

double countPhaseOffset(double complex* modulatedData, int length) {
    double sum = 0;
    for (int i = 0; i < length; ++i) {
        sum += cimag(modulatedData[i]);
    }
    double phaseOffset = sum / length;

    return phaseOffset;
}

double complex computeComplexSignalValue(double amplitude, double angle) {
    const double real = cos(2* PI + angle * amplitude);
    const double imaginary = sin(2* PI + angle * amplitude);

    return real + I * imaginary;
}


double complex* shiftModulatedData(double complex* modulatedData, double offset, int length) {
    double complex* shiftedData = malloc(sizeof(double complex) * length);
    const double real = cos(offset) * 1;
    const double imaginary = sin(offset) * 1;
    double complex offsetComplex = real + I * imaginary;

    for (int i = 0; i < length; ++i) {
        shiftedData[i] = modulatedData[i] * offsetComplex;
    }

    return shiftedData;


    // druga FUNKCJA DO obliczania ilosci probek na okres
int countNumberOfSamplesPerPeriod() {
	float numberOfSamples = (subcarrierFreq + modulatedSignalFactor * bitrate) * 2;
	int numberOfSamplesInt = numberOfSamples;
	if (numberOfSamples > numberOfSamplesInt) numberOfSamplesInt++;
    return numberOfSamplesInt;
}


double* getSamplesValues(int numberOfSamplesPerPeriod) {
    double* samplesAmplitude = malloc(sizeof(double) * numberOfSamplesPerPeriod);
    double x = 360 / numberOfSamplesPerPeriod;
    double val = PI / 180;
    for (int i = 0; i < numberOfSamplesPerPeriod; ++i) {
        samplesAmplitude[i] = sin(i * x * val);
    }

    return samplesAmplitude;
}

double complex* oversampling(double complex* modulatedData, int length) {
    double complex* oversampledData = malloc(sizeof(double complex) * length * oversamplingLevel);
    for(int i = 0; i < length; ++i) {
        oversampledData[i * oversamplingLevel] = modulatedData[i];
        for (int j = 1; j < oversamplingLevel; ++j) {
            oversampledData[i * oversamplingLevel + j] = 0 + 0*I;
        }
    }
    return oversampledData;
}

double complex* filtering(double complex* data, int length) {
    double complex* filteredData = malloc(sizeof(double complex) * length * oversamplingLevel);
    int counter = 0;
    double realValue = 0;
    double imagValue = 0;
    for (int i = 0; i < numberOfFiltersValues; ++i) {
        for (int j = 0; j <= i; ++j) {
            realValue += filterValues[j] * creal(data[j]);
            imagValue += filterValues[j] * cimag(data[j]);
        }
        filteredData[counter++] = realValue + imagValue * I;
        realValue = 0;
        imagValue = 0;
    }

    for (int i = numberOfFiltersValues; i < length * oversamplingLevel; ++i) {
        for (int j = 0; j < numberOfFiltersValues; ++j) {
            double value =(double) 1 / 2;
            realValue += filterValues[j] * creal(data[i + j]);
            imagValue += filterValues[j] * cimag(data[i + j]);
        }

        filteredData[counter++] = realValue + imagValue * I;
        realValue = 0;
        imagValue = 0;
    }

    return filteredData;
}

double complex* removeOversampledValues(double complex * data, int length) {
    double complex* filteredData = malloc(sizeof(double complex) * length);
    int counter = 0;
    for (int i = 0; i < length * oversamplingLevel; i += oversamplingLevel) {
        filteredData[counter++] = data[i];
    }

    return filteredData;
}


double complex* analogPhaseModulation(unsigned int* CLTUs, int length) {
    const int numberOfSamplesPerPeriod = countNumberOfSamplesPerPeriod();
    double* samplesValues = getSamplesValues(numberOfSamplesPerPeriod);
    const int outputDataLength = length * numberOfSamplesPerPeriod * 32;
    double complex* outputData = malloc(sizeof(double complex) * outputDataLength);
    int outputDataCounter = 0;
    unsigned int flag = 1;
    double deviation = angleDeviation;
    double complex tmpOffset = cos(PI / 180 * 20) + sin(PI / 180 * 20) * I;
    for (int i = 0; i < length; ++i) {
        for (int j = 0; j < 32; ++j) {

            if ((CLTUs[i] & flag) == 0) {
                for (int sampleNumber = 0; sampleNumber < numberOfSamplesPerPeriod; ++sampleNumber) {
                    double tmp = samplesValues[sampleNumber];
                    double complex tmp1 = computeComplexSignalValue(samplesValues[sampleNumber], deviation) * -1;
                    outputData[outputDataCounter++] = tmp1;// computeComplexSignalValue(samplesValues[sampleNumber], deviation) * -1;
                    deviation *= -1;
                }
            } else {
                for (int sampleNumber = 0; sampleNumber < numberOfSamplesPerPeriod; ++sampleNumber) {
                    double tmp = samplesValues[sampleNumber];
                    double complex tmp1 = computeComplexSignalValue(samplesValues[sampleNumber], deviation);
                    outputData[outputDataCounter++] = tmp1;// computeComplexSignalValue(samplesValues[sampleNumber], deviation);
                    deviation *= -1;
                }
            }
            flag = flag << 1;
        }
        flag = 1;
    }

    return outputData;
}

unsigned int* analogPhaseDemodulation(double complex* modulatedData, int modulatedDataBytesNumber, int numberOfComplexNumbers) {
    const int numberOfSamplesPerPeriod = countNumberOfSamplesPerPeriod();
    double phaseOffset = countPhaseOffset(modulatedData, numberOfSamplesPerPeriod * numberOfSymbolToCalculatePhaseOffset);
    double complex* shiftedData = shiftModulatedData(modulatedData, phaseOffset, numberOfComplexNumbers * 2);

    double complex* filteredData = removeOversampledValues(filtering(shiftedData, numberOfComplexNumbers), numberOfComplexNumbers);

    double* samplesValues = getSamplesValues(numberOfSamplesPerPeriod);
    unsigned int* demodulatedData = malloc(sizeof(unsigned int) * modulatedDataBytesNumber);

    int counter = 0;
    unsigned int demodulatedByteValue = 0;
    unsigned int bit0Value = 0;
    unsigned int bit1Value = 1;
    int demodulatedDataCounter = 0;
    double periodAmplitudeSum = 0;
    double deviation = angleDeviation;

    for (int i = 0; i < numberOfComplexNumbers; i += numberOfSamplesPerPeriod) {
        for (int j = 0; j < numberOfSamplesPerPeriod; ++j) {
            int modulatedDataIndex = i + j;

            double demodulatedValue;
            demodulatedValue = cimag(filteredData[modulatedDataIndex]) / sin(deviation);
            periodAmplitudeSum += demodulatedValue * samplesValues[j];
            deviation *= -1;
        }

        if (periodAmplitudeSum / numberOfSamplesPerPeriod > 0) {
            demodulatedByteValue = demodulatedByteValue | bit1Value;
        } else {
            demodulatedByteValue = demodulatedByteValue | bit0Value;
        }
        periodAmplitudeSum = 0;

        bit1Value = bit1Value << 1;
        counter++;

        if (counter == 32 ) {
            demodulatedData[demodulatedDataCounter++] = demodulatedByteValue;
            counter = 0;
            demodulatedByteValue = 0;
            bit1Value = 1;
        }

    }
    demodulatedData[demodulatedDataCounter] = demodulatedByteValue;

    return demodulatedData;
}


void printComplex(double complex* array, int length) {
    for (int i = 0; i < length; ++i) {
        printf("%f %f |", creal(array[i]), cimag(array[i]));
    }
}

