#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bchCoder.h"
#include "prepareData.h"
#include "generateCLTU.h";

void displayDecoded(unsigned int** decoded, int length);


int main() {
    initEncoder();
    initDecoder();
    char* msg = "1111110111111111111111111111111111111011111111111111111111101111111111111111011111111111111111111111111111111111111111111111111111111011111111111111110111111111111111111111111111111111111111111111111111111110111111111111111101111111111111111111111111111111111111111111";
    int numberOfCodeword;

    //przygotuj dane w bloki odpowiedniej dlugosci (56 bitow na 64 (2 inty))
    unsigned int* preparedData = prepareData(msg, &numberOfCodeword);

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
    displayDecoded(encodedCodewords, numberOfCodeword);

    //zmiana bitu w celu sprawdzenia poprawnosci korekcji bledu
    int flag = 1;
    flag = flag << 4;
    encodedCodewords[0][0] = encodedCodewords[0][0] ^ flag;
    encodedCodewords[1][0] = encodedCodewords[1][0] ^ flag;

    printf("\n ---------encoded with bit error----------- \n");
    displayDecoded(encodedCodewords, numberOfCodeword);

    //dekodowanie dla kazdego slowa kodowego
    unsigned int** decodedCodewords = (unsigned int**) malloc(sizeof(unsigned int*) * numberOfCodeword);
    for (int i = 0; i < numberOfCodeword ; i++) {
        decodedCodewords[i] = (unsigned int*) malloc(sizeof(unsigned int) * 2);
        unsigned int* decodedMsg = bchDecoder(encodedCodewords[i]);
        decodedCodewords[i][0] = decodedMsg[0];
        decodedCodewords[i][1] = decodedMsg[1];
    }
    printf("\n--------------- decoded----------- \n");
    displayDecoded(decodedCodewords, numberOfCodeword);


    /************************************ generate CLTU ********************************/
    unsigned int* CLTUs = generateCLTUs(encodedCodewords, &numberOfCodeword);
    int numberOfGeneratedCLTUs = numberOfCodeword / 2;
    if (numberOfCodeword % 2 != 0) numberOfGeneratedCLTUs++;
    print_binary(CLTUs, numberOfGeneratedCLTUs * numberOfBytesForCLTU);



    return 0;
}


void displayDecoded(unsigned int** decoded, int length) {
    for (int i = 0; i < length; i++) {
        print_binary(decoded[i], 2);
        printf("\n");
    }
}

