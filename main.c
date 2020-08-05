#include <stdio.h>
#include <stdlib.h>
#include "bchCoder.h"
#include "prepareData.h"
#include "generateCLTU.h"
#include "modulation.h"

void displayDoubleArray(unsigned int** decoded, int length);


int main() {
    /*********************************** init part ****************************************/
    initEncoder();
    initDecoder();
    char* msg = "111111111111111111111111111111111111111111111111111111111111111111";
    int numberOfCodeword;
    /*********************************** ///// ****************************************/

    /*********************************** prepare data ****************************************/
    //przygotuj dane w bloki odpowiedniej dlugosci (56 bitow na 64 (2 inty))
    printf("********** data ************ \n");
    unsigned int* preparedData = prepareData(msg, &numberOfCodeword);
    print_binary(preparedData, 10);
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
    flag = flag << 4;
    encodedCodewords[0][0] = encodedCodewords[0][0] ^ flag;
    encodedCodewords[1][0] = encodedCodewords[1][0] ^ flag;

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
    printf("\n modulated data \n\n");
    double complex* modulatedData = qpskModulation(CLTUs, numberOfGeneratedCLTUs * numberOfBytesForCLTU);
    printComplex(modulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU * 16);

    unsigned int* demodulatedData = qpskDemodulation(modulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU * 16);
    printf("\n demodulated %d \n", numberOfGeneratedCLTUs * numberOfBytesForCLTU * 16);
    print_binary(demodulatedData, numberOfGeneratedCLTUs * numberOfBytesForCLTU);
    return 0;

    /*****************************/// MODULATION END ///*******************************************************/


    /************************************ decode CLTUs ********************************/

    printf("\n data from CLTUs \n");
    unsigned int* decodedCLTUs = decodeCLTUs(CLTUs, numberOfGeneratedCLTUs * numberOfBytesForCLTU);
    print_binary(decodedCLTUs, numberOfGeneratedCLTUs * numberOfCodewordsInCLTU * 2);
    /*********************************** ///// ****************************************/

    
    
    /************************************ decoding ********************************/
    unsigned int** decodedCodewords = (unsigned int**) malloc(sizeof(unsigned int*) * numberOfCodeword);
    for (int i = 0; i < numberOfCodeword ; i++) {
        decodedCodewords[i] = (unsigned int*) malloc(sizeof(unsigned int) * 2);
        unsigned int* decodedMsg = bchDecoder(encodedCodewords[i]);
        decodedCodewords[i][0] = decodedMsg[0];
        decodedCodewords[i][1] = decodedMsg[1];
    }
    printf("\n--------------- decoded----------- \n");
    displayDoubleArray(decodedCodewords, numberOfCodeword);
    /*********************************** ///// ****************************************/





    return 0;
}


void displayDoubleArray(unsigned int** decoded, int length) {
    for (int i = 0; i < length; i++) {
        print_binary(decoded[i], 2);
        printf("\n");
    }
}

