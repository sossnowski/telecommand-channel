#ifndef BCH_CODER
#define BCH_CODER

#include "stdio.h"

//#define PRINT_MATRIXES true
#define CORRECT_ERRORS true
//#define DETECT_ERRORS true

char** generationMatrix;
char** checkMatrix;

struct bchEncoderData {
    short int codewordLength;
    short int messageLength;
    short int polynomialDegree;
    char* genPolynomial;
};
typedef struct bchEncoderData bchEncoderData;
bchEncoderData bchData;

unsigned int* bchEncoder(unsigned int* msg);
unsigned int* bchDecoder(unsigned int* encodedMsg);
void initEncoder();
void initDecoder();
char** generateMatrix(bchEncoderData* bchData);
char** generateParityCheckMatrix(char** generationMatrix);
void print_binary(unsigned int* number, int length);
#endif
