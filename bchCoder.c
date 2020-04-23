#include "bchCoder.h"
#include "stdbool.h"
#include "stdio.h"

/**
 *
 * @param msg 56 bitow danych
 * @return finalCodeword zakodowany ciag danych
 */

unsigned int* bchEncoder( unsigned int* msg) {
    unsigned int* finalCodeword = malloc(sizeof(unsigned int) * 2);
    finalCodeword = msg;
    short int counter = 0;
    unsigned int flag = 1;
    unsigned int messageFlag = 1;
    short int messageDataIndex = 0;
    flag = flag << bchData.messageLength % 32;

    for (int i = bchData.messageLength; i < bchData.codewordLength; i++) {
        for (int j = 0; j < bchData.messageLength; j++) {
            messageDataIndex = j / 32;
            if (j % 32 == 0 ) messageFlag = 1;
            if (generationMatrix[j][i] == '1' && ( msg[messageDataIndex] & messageFlag )) counter++;
            messageFlag = messageFlag << 1;
        }
        //dodaj bity kontrolne
        if (counter % 2 == 1) {
            finalCodeword[1] = finalCodeword[1] | flag;
        }
        flag = flag << 1;
        counter = 0;
    }
    return finalCodeword;
}

/**
 * Dekodowanie slow kodowych. Mozliwe sa dwa tryby: detekcji bledu oraz korekcji bledu, odpowiedni tryb jest wywolywany przez zdefiniowanie stalej w pliku bchCoder.h
 *
 * @param encodedMsg slowo kodowe 56 bitow danych + 8 bitow dodatkowych
 * @return zdekodowana wiadomosc
 */
unsigned int* bchDecoder(unsigned int* encodedMsg) {

    //tryb korekcji bledu
    #ifdef CORRECT_ERRORS

    int syndromeVector[bchData.polynomialDegree];
    unsigned int* decodedMsg = malloc(sizeof(unsigned int) * 2);
    bool error = false;
    unsigned int flag = 1;
    unsigned int messageIndex = 0;
    short int numberOfColumnWithError; //number of column in parity check matrix which is equal to syndrome


    for (int i = 0; i < bchData.polynomialDegree ; i++) {
        syndromeVector[i] = 0;
        for (int j = 0; j < bchData.codewordLength ; j++) {
            messageIndex = j / 32;
            if (j % 32 == 0 ) flag = 1;
            if (checkMatrix[i][j] == '1' && ( encodedMsg[messageIndex] & flag )) syndromeVector[i]++;
            flag = flag << 1;
        }
        if (syndromeVector[i] % 2) error = true; // if syndrome vector on i position is equal 1 modulo 2 it means that there is error
    }

    if (error) {
        for (int i = 0; i < bchData.codewordLength ; i++) {
            for (int j = 0; j < bchData.polynomialDegree ; j++) {
                int moduloVector = syndromeVector[j] % 2;
                if (checkMatrix[j][i] == '0' && moduloVector == 0 || checkMatrix[j][i] == '1' && moduloVector == 1) {
                    if (j == bchData.polynomialDegree - 1) numberOfColumnWithError = i;
                } else {
                    break;
                }
            }
        }
        messageIndex = 0;
        if (numberOfColumnWithError > 32) messageIndex = 1;
        numberOfColumnWithError = numberOfColumnWithError % 32;
        encodedMsg[messageIndex] = encodedMsg[messageIndex] ^ 1 << numberOfColumnWithError;
    }

    //flaga - 255 to bitowo 11111111
    flag = 255;
    unsigned int messageFlag = 0;
    flag = flag << bchData.messageLength % 32;
    messageFlag = ~ flag;
    decodedMsg[0] = encodedMsg[0];
    decodedMsg[1] = encodedMsg[1] & messageFlag;

    return decodedMsg;
    #endif


    #ifdef DETECT_ERRORS

    unsigned int* receivedMessage = malloc(sizeof(unsigned int) * 2);
    //flaga - 255 to bitowo 11111111
    unsigned int flag = 255;
    unsigned int messageFlag = 0;
    flag = flag << bchData.messageLength % 32;
    messageFlag = ~ flag;
    receivedMessage[0] = encodedMsg[0];
    receivedMessage[1] = encodedMsg[1] & messageFlag;
    unsigned int* encodedAgain = bchEncoder(receivedMessage);

    if (encodedMsg[0] != encodedAgain[0] || encodedMsg[1] != encodedAgain[1]) return 0;

    return receivedMessage;



    #endif
}

/**
 * Function create generation matrix using bchData object
 *
 * @return wskaznik do macierzy generacji
 */
char** generateMatrix() {
    short int numberOfParityBits = bchData.polynomialDegree;
    char **generateMatrix = (char**) malloc(bchData.messageLength * sizeof(char*));
    short int currentPolynomialBit = 0;
    //generowanie macierzy generacji przez przesuniecie wilomianu generacji
    for ( int i = 0; i < bchData.messageLength; i++ ) {
        generateMatrix[i] = (char*) malloc(bchData.codewordLength * sizeof(char));
        for (int j = 0; j < bchData.codewordLength ; ++j) {
            if (i <= j && bchData.polynomialDegree + i >= j ) {
                generateMatrix[i][j] = bchData.genPolynomial[currentPolynomialBit];
                currentPolynomialBit++;
            } else {
                generateMatrix[i][j] = '0';
            }
        }
        currentPolynomialBit = 0;
    }
    //przeksztalcenie macierzy generacji do macierzy jednostkowej do k - tej kolumny aby widomosc w slowie kodowym pozostala bez zmian
    for (int i = 0; i < bchData.messageLength; i++) {
        for (int j = 0; j < bchData.messageLength ; j++) {
            if (i >= j) continue;
            if (generateMatrix[i][j] == '1') {
                for (int k = 0; k <= bchData.polynomialDegree ; k++) {
                    if (generateMatrix[i][j + k] == '1' && generateMatrix[j][j + k] == '1' ||
                        generateMatrix[i][j + k] == '0' && generateMatrix[j][j + k] == '0') {
                        generateMatrix[i][j + k] = '0';
                    } else {
                        generateMatrix[i][j + k] = '1';
                    }
                }
            }
        }
    }
    //wypisanie w konsoli macierzy
    #ifdef PRINT_MATRIXES
    for (int l = 0; l < bchData.messageLength ; ++l) {
        for (int i = 0; i < bchData.codewordLength ; ++i) {
            printf("%c ", generateMatrix[l][i]);
        }
        printf("\n");
    }
    #endif

    return generateMatrix;
}

/**
 * Generowanie macierzy korekcji na podstawie macierzy generacji
 *
 * @return wskaznik do macierzy generacji
 */
char** generateParityCheckMatrix() {
    char **parityCheckMatrix = (char**) malloc(bchData.polynomialDegree * sizeof(char*));
    for ( int i = 0; i < bchData.polynomialDegree; i++ ) {
        parityCheckMatrix[i] = (char*) malloc(bchData.codewordLength * sizeof(char));
    }

    for (int i = bchData.messageLength; i < bchData.codewordLength; i++) {
        for (int j = 0; j < bchData.codewordLength; j++) {
            if (j < bchData.messageLength) {
                parityCheckMatrix[i - bchData.messageLength][j] = generationMatrix[j][i];
            } else {
                if (j == i) {
                    parityCheckMatrix[i - bchData.messageLength][j] = '1';
                } else {
                    parityCheckMatrix[i - bchData.messageLength][j] = '0';
                }
            }

        }
    }
    #ifdef PRINT_MATRIXES
    printf("\n");
    for (int k = 0; k < bchData.polynomialDegree; k++) {
        for (int i = 0; i < bchData.codewordLength ; i++) {
            printf("%c ", parityCheckMatrix[k][i]);
        }
        printf("\n");
    }
    #endif

    return parityCheckMatrix;
}

/**
 * Wypisywanie liczb w formie binarnej
 *
 * @param number wskaznik na tablice
 * @param length dlugosc tablicy
 */
void print_binary(unsigned int* number, int length) {
    int flag = 1;
    for (int i = 0; i < length ; ++i) {
        for (int j = 0; j < 32; ++j) {
            if (number[i] & flag) {
                printf("1");
            } else {
                printf("0");
            }
            flag = flag << 1;
        }
        printf("|");
        flag = 1;
    }
}

/**
 * Inicjowanie danych wymaganych dla kodera BCH
 */
void initEncoder() {
    bchData.codewordLength = 63;
    bchData.messageLength = 56;
    bchData.genPolynomial = "10100011";
    bchData.polynomialDegree = bchData.codewordLength - bchData.messageLength;
    generationMatrix = generateMatrix(&bchData);

}

/**
 * Inicjowanie danych wymaganych dla dekodera BCH
 */
void initDecoder() {
    bchData.codewordLength = 63;
    bchData.messageLength = 56;
    bchData.genPolynomial = "10100011";
    bchData.polynomialDegree = bchData.codewordLength - bchData.messageLength;
    checkMatrix = generateParityCheckMatrix(&bchData);
}

