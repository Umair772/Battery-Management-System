struct MeasurementDataStruct {
  float* currentDataPtr;
  float* maxCurrentPtr;
  float* minCurrentPtr;
  float* voltageDataPtr;
  float* maxVoltagePtr;
  float* minVoltagePtr;
  bool* maxCurrFlagPtr;
  bool* minCurrFlagPtr;
  bool* maxVolFlagPtr;
  bool* minVolFlagPtr;
};

typedef struct MeasurementDataStruct MeasurementData;


// requests data from UNO, decodes it, calculates voltage and current, and stores them
void measurementTaskFnc(void* arg)
{
  MeasurementData* localDataPtr = arg;
  Serial1.write("1");                                // request data from uno
  if(Serial1.available()) {                          // only if data avialable
    int tempV = Serial1.read() << 8;                 // decode bottom bits of voltage
    tempV |= Serial1.read() & 0xFF;                  // decode top bits of voltage
    // int tempC = Serial1.read() << 8;                 // decode top bits of current
    // tempC |= Serial1.read() & 0xFF;                  // decode top bits of current
    float voltTemp = tempV * 450.0 / 1023.0;         // calculate voltage, scaling to [0,450V]
    float curTemp = tempV * 50.0 / 1023.0 - 25.0;     // calculate current, scaling to [-25,25A]
    *(localDataPtr->voltageDataPtr) = voltTemp;      // store votlage
    *(localDataPtr->currentDataPtr) = curTemp;       // store current
    
    if (voltTemp > *(localDataPtr->maxVoltagePtr)) {
      *(localDataPtr->maxVoltagePtr) = voltTemp;
      *(localDataPtr->maxVolFlagPtr) = true;
    } else if (voltTemp < *(localDataPtr->minVoltagePtr)) {
      *(localDataPtr->minVoltagePtr) = voltTemp;
      *(localDataPtr->minVolFlagPtr) = true;
    }
    if (curTemp > *(localDataPtr->currentDataPtr)) {
      *(localDataPtr->maxCurrentPtr) = curTemp;
      *(localDataPtr->maxCurrFlagPtr) = true;
    } else if (curTemp < *(localDataPtr->currentDataPtr)) {
      *(localDataPtr->minCurrentPtr) = curTemp;
      *(localDataPtr->minCurrFlagPtr) = true;
    }
  }
};
