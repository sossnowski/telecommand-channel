#include "dataFlowVariant.h"
#include <string.h>
#include <stdlib.h>

char* normalToNRZM(char* data) {
    int dataLength = strlen(data);
    char* resultSignal = malloc(sizeof(char) * dataLength);
    if (data[0] == '1')
        resultSignal[0] = '1';
    else
        resultSignal[0] = '0';
    for (int i = 1; i < dataLength; ++i) {
        if (data[i] == '1') {
            if (resultSignal[i - 1] == '1')
                resultSignal[i] = '0';
            else
                resultSignal[i] = '1';
        } else {
            resultSignal[i] = resultSignal[i -1];
        }
    }
    return resultSignal;
}

char* NRZMtoNormal(char* data) {
    int dataLength = strlen(data);
    char* resultSignal = malloc(sizeof(char) * dataLength);
    if (data[0] == '1')
        resultSignal[0] = '1';
    else
        resultSignal = '0';
    for (int i = 1; i < dataLength - 1; ++i) {
        if (data[i] == '1' && resultSignal[i-1] == '1')
            resultSignal[i] = '0';
        else if (data[i] == '1' && resultSignal[i-1] == '0')
            resultSignal[i] = '1';
        else if (data[i] == '0')
            resultSignal[i] = resultSignal[i-1];
    }

    return resultSignal;
}

char* normalToSPL(char* data) {
    int dataLength = strlen(data);
    char* resultData = malloc(sizeof(char) * dataLength * 2);
    int counter = 0;
    for (int i = 0; i < dataLength ; ++i) {
        if (data[i] == '1') {
            resultData[i * 2] = '1';
            resultData[i *2 + 1] = '0';
        } else {
            resultData[i * 2] = '0';
            resultData[i *2 + 1] = '1';
        }
    }
    return resultData;
}

char* SPLToNormal(char* data) {
    int dataLength = strlen(data);
    char* resultSignal = malloc(sizeof(char) * dataLength / 2);
    for (int i = 0; i < dataLength; ++i) {
        if (data[i] == '1' && data[i + 1] == '0')
            resultSignal[i / 2] = '1';
        else
            resultSignal[i / 2] = '0';

        ++i;
    }
    return resultSignal;
}
