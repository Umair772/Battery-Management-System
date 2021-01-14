#include <EEPROM.h>

struct DataLoggingDataStruct {
    float* maxCurrentPtr;
    float* minCurrentPtr;
    float* maxVoltagePtr;
    float* minVoltagePtr;
    float* maxSOCPtr;
    float* minSOCPtr;
    bool* maxCurrFlagPtr;
    bool* minCurrFlagPtr;
    bool* maxVolFlagPtr;
    bool* minVolFlagPtr;
    bool* maxSocFlagPtr;
    bool* minSocFlagPtr;
    bool* resetFlagPtr;
};

typedef struct DataLoggingDataStruct DataLoggingData;

int floatSize = sizeof(float);
int maxCAddr = 0;
int minCAddr = maxCAddr + floatSize;
int maxVAddr = minCAddr + floatSize;
int minVAddr = maxVAddr + floatSize;
int maxSocAddr = minVAddr + floatSize;
int minSocAddr = maxSocAddr + floatSize;

void dataLoggingTaskFnc(void* arg){
    DataLoggingData* localDataPtr = arg;
    if (*(localDataPtr->resetFlagPtr)) {
        *(localDataPtr->maxCurrentPtr) = 0;
        EEPROM.put(maxCAddr, 0);
        *(localDataPtr->minCurrentPtr) = 0;
        EEPROM.put(minCAddr, 0);
        *(localDataPtr->maxVoltagePtr) = -1;
        EEPROM.put(maxVAddr, -1);
        *(localDataPtr->minVoltagePtr) = -1;
        EEPROM.put(minVAddr, -1);
        *(localDataPtr->maxSOCPtr) = -1;
        EEPROM.put(maxSocAddr, -1);
        *(localDataPtr->minSOCPtr) = -1;
        EEPROM.put(minSocAddr, -1);
        *(localDataPtr->resetFlagPtr) = false;
    } else {
        if (*(localDataPtr->maxCurrFlagPtr)) {
            EEPROM.put(maxCAddr, *(localDataPtr->maxCurrentPtr));
            *(localDataPtr->maxCurrentPtr) = false;
        }
        if (*(localDataPtr->minCurrFlagPtr)) {
            EEPROM.put(minCAddr, *(localDataPtr->minCurrentPtr));
            *(localDataPtr->minCurrentPtr) = false;
        }
        if (*(localDataPtr->maxVolFlagPtr)) {
            EEPROM.put(maxVAddr, *(localDataPtr->maxVoltagePtr));
            *(localDataPtr->maxVolFlagPtr) = false;
        }
        if (*(localDataPtr->minVolFlagPtr)) {
            EEPROM.put(minVAddr, *(localDataPtr->minVoltagePtr));
            *(localDataPtr->minVolFlagPtr) = false;
        }
        if (*(localDataPtr->maxSocFlagPtr)) {
            EEPROM.put(maxSocAddr, *(localDataPtr->maxSOCPtr));
            *(localDataPtr->maxSocFlagPtr) = false;
        }
        if (*(localDataPtr->minSocFlagPtr)) {
            EEPROM.put(minSocAddr, *(localDataPtr->minSOCPtr));
            *(localDataPtr->minSocFlagPtr) = false;
        }
    }
};