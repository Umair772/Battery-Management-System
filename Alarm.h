struct AlarmDataStruct {
  String* hvilAlarmPtr;
  String* overcurrentAlarmPtr;
  String* hvorAlarmPtr;
  String* hvilInterruptPtr;
  int*    hvilPtr;
  float*  currentPtr;
  float*  voltagePtr;
  bool*   alarmTriggerPtr;
  bool*   alarmAckPtr;
  bool*   hvilAlarmFlagPtr;
  bool*   hvilInterruptAlarmFlagPtr;
  bool*   overCurrentAlarmFlagPtr;
  bool*   outOfRangeVoltageFlagPtr;
};

typedef struct AlarmDataStruct AlarmData;

// states coding: 0 = NOT ACTIVE; 1 = ACTIVE, NOT ACKNOWLEDGED, 3 = ACTIVE, ACKNOWLEDGED
int hvilState = 0;
int overCurrState = 0;
int voltState = 0;
int hvilInterState = 0;

// local trigger variables for each state, to determine if the main trigger should be triggered
bool hvilTrig = false;
bool overCurrTrig = false;
bool voltTrig = false;
bool hvilInterTrig = false;

// changes Alarm states as descirbed in the lab spec, and depicted in the diagrams in the lab report
// contains 4 state machines, one for each alarm
void alarmTaskFnc(void* arg)
{
  AlarmData* localDataPtr = arg;
  bool alarmAck = *(localDataPtr->alarmAckPtr);
  bool alarmTrig = *(localDataPtr->alarmTriggerPtr);

   // temporary storage for the HVIL value
  int hvilData = *(localDataPtr->hvilPtr);

  // state machine for the HVIL Alarm
  switch (hvilState) {
    case 0:
      if (hvilData == 0) {
        hvilState = 1;
        *(localDataPtr->hvilAlarmPtr) = "ACTIVE, NOT ACKNOWLEDGED";
        *(localDataPtr->hvilAlarmFlagPtr) = true;
        hvilTrig = true;
      }
      break;
    case 1:
      if (hvilData == 1) {
        *(localDataPtr->hvilAlarmPtr) = "NOT ACTIVE";
        *(localDataPtr->hvilAlarmFlagPtr) = false;
        hvilState = 0;
        hvilTrig = false;
      } else if (hvilData == 0 && alarmAck) {
        hvilState = 2;
        *(localDataPtr->hvilAlarmPtr) = "ACTIVE, ACKNOWLEDGED";
        hvilTrig = false;
      }
      break;
    case 2:
      if (hvilData == 1) {
        *(localDataPtr->hvilAlarmPtr) = "NOT ACTIVE";
        *(localDataPtr->hvilAlarmFlagPtr) = false;
        hvilState = 0;
      }
      break;
    default:
      *(localDataPtr->hvilAlarmPtr) = "NOT ACTIVE";
      *(localDataPtr->hvilAlarmFlagPtr) = false;
      hvilState = 0;
      break;
  }

  // temporary storage for the current value
  float curr = *(localDataPtr->currentPtr);

  // state machine for the Overcurrent Alarm
  switch (overCurrState) {
    case 0:
      if (curr <= -5.0 || curr >= 20.0) {
        overCurrState = 1;
        *(localDataPtr->overcurrentAlarmPtr) = "ACTIVE, NOT ACKNOWLEDGED";
        *(localDataPtr->overCurrentAlarmFlagPtr) = true;
        overCurrTrig = true;
      }
      break;
    case 1:
      if (curr > 5.0 && curr < 20.0) {
        *(localDataPtr->overcurrentAlarmPtr) = "NOT ACTIVE";
        *(localDataPtr->overCurrentAlarmFlagPtr) = false;
        overCurrState = 0;
        overCurrTrig = false;
      } else if ((curr <= -5.0 || curr >= 20.0) && alarmAck) {
        overCurrState = 2;
        *(localDataPtr->overcurrentAlarmPtr) = "ACTIVE, ACKNOWLEDGED";
        overCurrTrig = false;
      }
      break;
    case 2:
      if (curr > 5.0 && curr < 20.0) {
        *(localDataPtr->overcurrentAlarmPtr) = "NOT ACTIVE";
        *(localDataPtr->overCurrentAlarmFlagPtr) = false;
        overCurrState = 0;
      }
      break;
    default: 
        *(localDataPtr->overcurrentAlarmPtr) = "NOT ACTIVE";
        *(localDataPtr->overCurrentAlarmFlagPtr) = false;
        overCurrState = 0;
        break;
  }

  // temporary storage for the voltage value
  float volt = *(localDataPtr->voltagePtr);

  // state machine for the High Voltage Out of Range Alarm
  switch (voltState) {
    case 0:
      if (volt <= 280.0 || volt >= 405.0) {
        voltState = 1;
        *(localDataPtr->hvorAlarmPtr) = "ACTIVE, NOT ACKNOWLEDGED";
        *(localDataPtr->outOfRangeVoltageFlagPtr) = true;
        voltTrig = true;
      }
      break;
    case 1:
      if (volt > 280.0 && volt < 405.0) {
        *(localDataPtr->hvorAlarmPtr) = "NOT ACTIVE";
        *(localDataPtr->outOfRangeVoltageFlagPtr) = false;
        voltState = 0;
        voltTrig = false;
      } else if ((volt <= 280.0 || volt >= 405.0) && alarmAck) {
        voltState = 2;
        *(localDataPtr->hvorAlarmPtr) = "ACTIVE, ACKNOWLEDGED";
        voltTrig = false;
      }
      break;
    case 2:
      if (volt > 280.0 && volt < 405.0) {
        *(localDataPtr->hvorAlarmPtr) = "NOT ACTIVE";
        *(localDataPtr->outOfRangeVoltageFlagPtr) = false;
        voltState = 0;
      }
      break;
    default:
      *(localDataPtr->hvorAlarmPtr) = "NOT ACTIVE";
      *(localDataPtr->outOfRangeVoltageFlagPtr) = false;
      voltState = 0;
      break;
  }

  // state machine for the HVIL interrupt alarm
  // gets triggered in BMS.ino, the transitions are handled here
  switch (hvilInterState) {
    case 0: break;
    case 1: 
      if (hvilData == 1) {
        *(localDataPtr->hvilInterruptPtr) = "NOT ACTIVE";
        *(localDataPtr->hvilInterruptAlarmFlagPtr) = false;
        hvilInterState = 0;
        hvilInterTrig = false;
      } else if (alarmAck) {
        hvilInterState = 2;
        hvilInterTrig = false;
        *(localDataPtr->hvilInterruptPtr) = "ACTIVE, ACKNOWLEDGED";
      }
      break;
    case 2:
      if (hvilData == 1) {
        *(localDataPtr->hvilInterruptPtr) = "NOT ACTIVE";
        *(localDataPtr->hvilInterruptAlarmFlagPtr) = false;
        hvilInterState = 0;
      }
      break;
    default: 
      *(localDataPtr->hvilInterruptPtr) = "NOT ACTIVE";
      *(localDataPtr->hvilInterruptAlarmFlagPtr) = false;
      hvilInterState = 0;
      hvilInterTrig = false;
      break;
  }

  // if any alarm was triggered, make sure the main trigger is also triggered
  *(localDataPtr->alarmTriggerPtr) = hvilTrig || overCurrTrig || voltTrig || hvilInterTrig;

  // if any alarm is still triggered, make sure they still need to be acknowledged
  *(localDataPtr->alarmAckPtr) = (hvilTrig || overCurrTrig || voltTrig || hvilInterTrig) ? false : true;
};