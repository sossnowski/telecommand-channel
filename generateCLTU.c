#include "generateCLTU.h"
#include "stdbool.h"
#include "string.h"


void generateBinaryStartSequence() {
    unsigned short int binarySequence = 0;
    unsigned short int flag = 1;

    for (int i = 0; i < 16; ++i) {
        if(startSequence[i] == '1') binarySequence = binarySequence | flag;
        flag = flag << 1;
    }

    binaryStartSequence = binarySequence;
}

void generateBinaryTailSequence() {
    unsigned int* binarySequence = malloc(sizeof(unsigned int) * 2);
    unsigned int flag = 1;

    for (int j = 0; j < 2 ; ++j) {
        for (int i = 0; i < 32; ++i) {
            if(tailSequence[i + (j * 32)] == '1') binarySequence[j] = binarySequence[j] | flag;
            flag = flag << 1;
        }
        flag = 1;
    }

    binaryTailSequence = binarySequence;
}

unsigned int* generateCLTUs(unsigned int** data, *totalNumberOfCodeword) {
    generateBinaryStartSequence();
    generateBinaryTailSequence();
    int numberOfCLTUNeeded = *totalNumberOfCodeword / numberOfCodewordsInCLTU;
    if (*totalNumberOfCodeword % numberOfCodewordsInCLTU != 0) numberOfCLTUNeeded++;
    int numberOfIntsNeededForData = numberOfCodewordsInCLTU * 2;
    int counter = 0;
    unsigned int* allCLTUs = malloc(sizeof(unsigned int) * numberOfBytesForCLTU * numberOfCLTUNeeded);

    for (int j = 0; j < numberOfCLTUNeeded; ++j ) {
        for (int i = 1; i < numberOfIntsNeededForData; ++i) {
            if (counter == *totalNumberOfCodeword) break;
            allCLTUs[i + (j * (numberOfBytesForCLTU))] = data[counter][0]; //set data bit inside cltu
            allCLTUs[i + 1 + (j * (numberOfBytesForCLTU))] = data[counter][1];
            counter++;
            i++;
        }
        int startSequencePosition = j * (numberOfBytesForCLTU);
        int tailSequencePosition = startSequencePosition + numberOfIntsNeededForData;
        allCLTUs[startSequencePosition] = binaryStartSequence;
        allCLTUs[startSequencePosition] = allCLTUs[startSequencePosition] << 16;
        allCLTUs[tailSequencePosition + 1] = binaryTailSequence[0];
        allCLTUs[tailSequencePosition + 2] = binaryTailSequence[1];
    }

    return allCLTUs;
}