struct SocDataStruct {
  float* voltageDataPtr;
  float* currentDataPtr;
  float* SOCDataPtr; 
  float* maxCurrDataPtr;
  float* minCurrDataPtr;
  float* maxVoltDataPtr;
  float* minVoltDataPtr;
  float* maxSOC;
  float* minSOC;
  bool*  maxCurrFlag;
  bool*  minCurrFlag;
  bool*  maxVolFlag;
  bool*  minVolFlag;
  bool*  maxSOCFlag;
  bool*  minSOCFlag;
};

typedef struct SocDataStruct SocData;

float Rbatt = 0.5;
void socTaskFnc(void* arg)
{
  SocData* localDataPtr = arg;
  //Calculating SOC based on the voltage range
  float volt = *(localDataPtr->voltageDataPtr);
  float Curr = *(localDataPtr->currentDataPtr);
  float voltOpenCircuit = volt + Curr*Rbatt;
  float SOC = 0.0;
  if (volt >= 200.0 && volt <= 250.0) {
    SOC = 0;
  } else if (volt > 250.0 && volt <= 300.0) {
    SOC = (voltOpenCircuit - 250.0) * 2/5;
  } else if (volt > 300.0 && volt <= 350.0) {
    SOC = (voltOpenCircuit - 300.0) * 6/5;
  } else if (volt > 350.0 && volt <= 400.0) {
    SOC = (voltOpenCircuit - 350.0) * 2/5;
  }
  *(localDataPtr->SOCDataPtr) = SOC;
  //Setting the max and min values for each measurment
  //Voltage
  float maxVolt = *(localDataPtr->maxVoltDataPtr);
  float minVolt = *(localDataPtr->minVoltDataPtr);
  if (volt > maxVolt) {
    *(localDataPtr->maxVoltDataPtr) = volt;
    *(localDataPtr->maxVolFlag) = true;
  }
  if (volt < minVolt) {
    *(localDataPtr->minVoltDataPtr) = volt;
    *(localDataPtr->minVolFlag) = true;
  }
  //current
  float curr = *(localDataPtr->currentDataPtr);
  float maxCurr = *(localDataPtr->maxCurrDataPtr);
  float minCurr = *(localDataPtr->minCurrDataPtr);
  if (curr > maxCurr) {
    *(localDataPtr->maxCurrDataPtr) = curr;
    *(localDataPtr->maxCurrFlag) = true;
  }
  if (curr < minCurr) {
    *(localDataPtr->minCurrDataPtr) = curr;
    *(localDataPtr->minCurrFlag) = true;
  }
  //SOC
  float maxSOC = *(localDataPtr->maxSOC);
  float minSOC = *(localDataPtr->minSOC);
  if (SOC > maxSOC) {
    *(localDataPtr->maxSOC) = SOC;
    *(localDataPtr->maxSOCFlag) = true;
  }
  if (SOC < minSOC) {
    *(localDataPtr->minSOC) = SOC;
    *(localDataPtr->minSOCFlag) = true;
  }
};
