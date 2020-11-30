#include <stdio.h>
#include <stdlib.h>
#include "bchCoder.h"
#include "prepareData.h"
#include "generateCLTU.h"
#include "modulation.h"
#include "randomizer.h"
#include "dataFlowVariant.h"
#include <string.h>

void displayDoubleArray(unsigned int** decoded, int length);


int main() {
    /*********************************** init part ****************************************/
    initEncoder();
    initDecoder();
    char* msg = "1111111111111111111111111111111111111111111111111111";
    int numberOfCodeword;
    /*********************************** ///// ****************************************/
    // char* tmp = normalToSPL(msg);
    //printf("\n %s \n", tmp);


    char* randomizedData = randomizer(msg);
    int dataLength = strlen(randomizedData);

    printf("randomized data \n");
    printf("%s \n", randomizedData);

    /*********************************** prepare data ****************************************/
    //przygotuj dane w bloki odpowiedniej dlugosci (56 bitow na 64 (2 inty))
    printf("********** prepared data ************ \n");
    unsigned int* preparedData = prepareData(randomizedData, &numberOfCodeword);
    print_binary(preparedData, numberOfCodeword * 2);
    /*********************************** ///// ****************************************/


    /*********************************** bch coding for prepared data ****************************************/
    //jedno slowo kodowe sklada sie z 64 bitow (63 informacji + dodatkowe 0), a wiec z dwoch elementow tablicy ktora przechowuje unsigned inty.
    unsigned int** encodedCodewords =(unsigned int**) malloc(sizeof(unsigned int*) * numberOfCodeword);
    unsigned int* oneCodewordData = malloc(sizeof(unsigned int) * 2);

    //kodowanie bch dla kazdego zestawu 56 bitow z danymi
    for (int i = 0; i < numberOfCodeword ; i++) {
        encodedCodewords[i] = (unsigned int*)malloc(sizeof(unsigned int) * 2);
        oneCodewordData[0] = preparedData[2*i];
        oneCodewordData[1] = preparedData[2*i + 1];
        unsigned int* t = bchEncoder(oneCodewordData);
        encodedCodewords[i][0] = t[0];
        encodedCodewords[i][1] = t[1];
    }

    printf("\n ---------encoded----------- \n");
    displayDoubleArray(encodedCodewords, numberOfCodeword);
    /*********************************** ///// ****************************************/



    /*********************************** change bit to check correction ****************************************/
    int flag = 1;
    flag = flag << 2;
    encodedCodewords[0][0] = encodedCodewords[0][0] ^ flag;
    flag = flag << 2;
//    encodedCodewords[1][0] = encodedCodewords[1][0] ^ flag;

    printf("\n ---------encoded with bit error----------- \n");
    displayDoubleArray(encodedCodewords, numberOfCodeword);
    /*********************************** ///// ****************************************/

    /************************************ generate CLTUs ********************************/
    printf("generated CLTUs \n");
    unsigned int* CLTUs = generateCLTUs(encodedCodewords, &numberOfCodeword);
    int numberOfGeneratedCLTUs = numberOfCodeword / numberOfCodewordsInCLTU;
    if (numberOfCodeword % numberOfCodewordsInCLTU != 0) numberOfGeneratedCLTUs++;
    print_binary(CLTUs, numberOfGeneratedCLTUs * numberOfBytesForCLTU);
    /*********************************** ///// ****************************************/


    /***************************** MODULATION *****************************************************************/
    printf("\n modulated data \n");
//    double complex* modulatedData = qpskModulation(CLTUs, numberOfGeneratedCLTUs * numberOfBytesForCLTU);
    double complex* modulatedData = analogPhaseModulation(CLTUs, numberOfGeneratedCLTUs * numberOfBytesForCLTU);
    printComplex(modulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU * 32);
    printf("\n nowa \n");

    /***************************** OVERSAMPLING ***************/
//    double complex* oversampledModulatedData = oversampling(modulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU * 16);

//    printComplex(oversampledModulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU * 16 * oversamplingLevel);

    ///////^^^^^^^^^^^^^^^^^^^^^^^^^^ SIGNAL TRANSMITION ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^/////////////////

//    unsigned int* filteredModulatedData = filtering(oversampledModulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU * 16 * oversamplingLevel);
//    printf("\n after filtering \n");
//    printComplex(filteredModulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU * 16);
    unsigned int* demodulatedData = analogPhaseDemodulation(modulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU);
    printf("\n after demodulation\n");
    print_binary(demodulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU);


    /*****************************/// MODULATION END ///*******************************************************/


    /************************************ decode CLTUs ********************************/

    printf("\n data from CLTUs \n");
    unsigned int* decodedCLTUs = decodeCLTUs(demodulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU);
    print_binary(decodedCLTUs, numberOfGeneratedCLTUs * numberOfCodewordsInCLTU * 2);
    /*********************************** ///// ****************************************/



    /************************************ decoding ********************************/
    int lengthOfdecodedData = numberOfGeneratedCLTUs * numberOfCodewordsInCLTU * 2;
    unsigned int* decodedCodewords = malloc(sizeof(unsigned int) * numberOfCodeword * 2);
    unsigned int* oneCodewordToDecode = malloc(sizeof(unsigned int) * 2);
    for (int i = 0; i < lengthOfdecodedData ; i++) {
        oneCodewordToDecode[0] = decodedCLTUs[i];
        oneCodewordToDecode[1] = decodedCLTUs[i + 1];
        unsigned int* decodedMsg = bchDecoder(oneCodewordToDecode);
        decodedCodewords[i] = decodedMsg[0];
        decodedCodewords[i+1] = decodedMsg[1];
        ++i;
    }
    printf("\n--------------- decoded----------- \n");
    print_binary(decodedCodewords, numberOfGeneratedCLTUs * numberOfCodewordsInCLTU * 2);
    char* stringDataFromBinary = fromBinaryToString(decodedCodewords, dataLength);
    char* derandomizedData = randomizer(stringDataFromBinary);
    printf("\n", derandomizedData);
    /*********************************** ///// ****************************************/

//    int data[8];
//    for (int i = 0; i < 8; ++i) {
//        data[i] = 1;
//    }
//    int result = 0;
//    for (int i = 0; i < 256; ++i) {
//        result = data[0] + data[1];
//        result %= 2;
//        result += data[2];
//        result %= 2;
//        result += data[3];
//        result %=2;
//        result += data[4];
//        result %=2;
//        result += data[6];
//        result %=2;
//
//        printf("%d", data[0]);
//        for (int z = 0; z < 7; ++z) {
//            data[z] = data[z + 1];
//        }
//        data[7] = result;
//    }


    return 0;
}


void displayDoubleArray(unsigned int** decoded, int length) {
    for (int i = 0; i < length; i++) {
        print_binary(decoded[i], 2);
        printf("\n");
    }
}

