#ifndef GENERATE_CLTU
#define GENERATE_CLTU


#define numberOfCodewordsInCLTU 3
#define numberOfBytesForCLTU ((numberOfCodewordsInCLTU * 2) + 3)
#define startSequence "1110101110010000"
#define tailSequence "1100010111000101110001011100010111000101110001011100010101111001"

unsigned short int binaryStartSequence;
unsigned int* binaryTailSequence;


void generateBinaryStartSequence();
void generateBinaryTailSequence();
unsigned int* generateCLTUs(unsigned int** dataString, int* totalNumberOfCodeword);


#endif
