struct ContactorDataStruct {
  bool* turnOnReqPtr;
  //String* hvilInterruptPtr;
  bool* hvilAlarmFlagPtr;
  bool* hvilInterruptAlarmFlagPtr;
  bool* overCurrentAlarmFlagPtr;
  bool* outOfRangeVoltageFlagPtr;
};

typedef struct ContactorDataStruct ContactorData;


// toggle the LED based on the press of the battery on/off button
void contactorTaskFnc(void* arg)
{
  ContactorData* localDataPtr = arg;
  //Saving the pointer values into variables for readibility
  bool hvilAlarmFlag = *(localDataPtr->hvilAlarmFlagPtr);
  bool hvilInterruptAlarmFlag = *(localDataPtr->hvilInterruptAlarmFlagPtr);
  bool overCurrentAlarmFlag = *(localDataPtr->overCurrentAlarmFlagPtr);
  bool outOfRangeVoltageFlag = *(localDataPtr->outOfRangeVoltageFlagPtr);
  bool turnOn = *(localDataPtr->turnOnReqPtr);
  if(hvilAlarmFlag || hvilInterruptAlarmFlag || overCurrentAlarmFlag || outOfRangeVoltageFlag) {
    digitalWrite(24, LOW);  // don't turn on the battery/LED on if any of the alarms were triggered
  } else if (turnOn) {
    digitalWrite(24, HIGH); // handle the turn on request
  } else {
    digitalWrite(24, LOW);  // handle the turn off request
  }
};
