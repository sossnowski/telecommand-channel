#include "generateCLTU.h"
#include "stdbool.h"
#include "string.h"
#include "bchCoder.h"
#include "stdlib.h"

//generowanie binarnej sekwencji startowej ze stringu podanego w pliku generateCLTU.h
void generateBinaryStartSequence() {
    unsigned short int binarySequence = 0;
    unsigned short int flag = 1;

    for (int i = 0; i < 16; ++i) {
        if(startSequence[i] == '1') binarySequence = binarySequence | flag;
        flag = flag << 1;
    }

    binaryStartSequence = binarySequence;
}

//generowanie binarnej sekwencji koncowej ze stringu podanego w pliku generateCLTU.h
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

//generowanie blokow CLTU z zakodowanych wczesniej danych
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
            allCLTUs[i + (j * (numberOfBytesForCLTU))] = data[counter][0];
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

//odczytywanie zakodowanych danych z blokow CLTU i zebranie ich do wspolnej tablicy
unsigned int* decodeCLTUs(unsigned int* data, int length) {
    int numberOfCLTUs = length / (numberOfCodewordsInCLTU * 2 + 3);
    unsigned int* decodedCLTUs = malloc(sizeof(unsigned int ) * numberOfCLTUs * numberOfCodewordsInCLTU * 2);
    int counter = 0;

    for (int i = 0; i < numberOfCLTUs ; ++i) {
        for (int j = 0; j <= numberOfCodewordsInCLTU * 2 ; ++j) {
            if (j == 0) continue;
            decodedCLTUs[counter] = data[i * numberOfBytesForCLTU + j];
            counter++;
        }
    }

    return decodedCLTUs;
}