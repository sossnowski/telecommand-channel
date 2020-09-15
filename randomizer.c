#include "randomizer.h"
#include "string.h"
#include <stdlib.h>

char* randomizer(char* data) {
    int dataLength = strlen(data);

    char* resultData = malloc(sizeof(char) * dataLength);
    int sequenceCounter = 0;
    for (int i = 0; i < dataLength; ++i) {
        if (sequenceCounter == 256) sequenceCounter = 0;
        if ((data[i] + randomizerSeqence[sequenceCounter++]) % 2)
            resultData[i] = '1';
        else
            resultData[i] = '0';
    }
    return resultData;
}

char* fromBinaryToString(unsigned int* data, int length) {
    unsigned int flag = 1;
    int mainCounter = 0;
    int stringDataCounter = 0;
    char* resultData = malloc(sizeof(char) * length);
    int dataLengthCounter = 0;
    int numberOfInts = length / 32 + 1;
    for (int i = 0; i < numberOfInts; ++i) {
        for (int j = 0; j < 32 ; ++j) {
            if (dataLengthCounter == length) break;
            if (mainCounter == 56) {
                mainCounter = 0;
                break;
            }
            if (data[i] & flag)
                resultData[stringDataCounter++] = '1';
            else
                resultData[stringDataCounter++] = '0';
            flag = flag << 1;
            mainCounter++;
            dataLengthCounter++;
        }
        flag = 1;
    }
    return resultData;
}

