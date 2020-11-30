#include "bchCoder.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"

/**
 * Funkcja dzieli wejsciowy string danych na bloki o dlugosc 64 bitow oraz umieszcza w nich 56 kolejnych bitow informacyjnych
 *
 * @param dataString wskaznik na dane do zakodowania
 * @param numberOfCodeword ilosc slow kodowych wymaganych do zakodowanie danych
 * @return dataBits wskaznik na tablice danych umieszczonych w 64 bitowych slowach kodowych
 */
unsigned int* prepareData(char* dataString, int* numberOfCodeword) {
    int length = strlen(dataString);
    unsigned int flag = 1;
    //liczba slow kodowych ktora musi powstac aby zakodowac wszystkie dane
    int numberOfInts = length / 56;
    if (length % 56) numberOfInts++;
    //kazde slowo kodowe sklada sie z 2 unsigned intow
    numberOfInts *= 2;
    unsigned int* dataBits = malloc(sizeof(unsigned int) * numberOfInts);
    int arrayIndex = 0;
    int bitIndex = 0;

    for (int i = 0; i < length ; i++) {
        if (bitIndex == 56) {
            arrayIndex++;
            bitIndex = 0;
            flag = 1;
        }

        if (bitIndex == 32 ) {
            flag = 1;
            arrayIndex++;
        }
        if (dataString[i] == '1') dataBits[arrayIndex] = dataBits[arrayIndex] | flag;
        flag = flag << 1;
        bitIndex++;
    }
    *numberOfCodeword = numberOfInts / 2;
    return dataBits;
}