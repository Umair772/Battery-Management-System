struct RemoteTerminalDataStruct {
    bool* resetEEpromPtr;
    float* maxCurrDataPtr;
    float* maxVoltDataPtr;
    float* minCurrDataPtr;
    float* minVoltDataPtr;
};

typedef struct RemoteTerminalDataStruct RemoteTerminalData;
char input = 0;
bool print = true;

void remoteTaskFnc(void* arg)
{
    RemoteTerminalData* localDataPtr = arg;
    //Serial.begin(9600);
  if (Serial.available() > 0 ) {
        input = Serial.read();
        //Serial.print(input);
        if (input == '1') {
            print = true;
            *(localDataPtr->resetEEpromPtr) = true;
            input = 0;
        }else if(input == '2') {
            print = true;
            //Display max and min values for current(ask TA for clarification)   
        }else if(input == '3') {
            print = true;
            //Display max and min values for voltage
        } 
    }


  if(print) {
    print = false;
    Serial.println("[1] Reset EEPROM Values");
    Serial.println("[2] Operating HV Current Range [Hi, Lo]");
    Serial.println("[3] Operating HV voltage Range [Hi, Lo]");
    Serial.println(" ");
    Serial.println("Enter your menu choice [1-3]: ");
  }
}









