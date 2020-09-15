#ifndef BCH_RANDOMIZER_H
#define BCH_RANDOMIZER_H

static const char* randomizerSeqence = "1111111100111001100111100101101001101000111010010000011011110101011011001000100100101111101000010011000101011110000010001100000001010010101010001011101110101110010011101100001011000111111011010110011011011100001110001101010011111000100001100101000000111101";

char* randomizer(char* data);
char* fromBinaryToString(unsigned int* data, int length);

#endif //BCH_RANDOMIZER_H