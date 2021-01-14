#include <TimerOne.h>
#include "TaskStruct.h"
#include "SOC.h"
#include "TFT.h"
#include "Measurement.h"
#include "Contactor.h"
#include "Alarm.h"
#include "RemoteTerminal.h"
#include "DataLogging.h"

// pin for the HVIL Interrupt Alarm
const byte interruptPin = 21;

// period with which the timeBaseFlag should be set (100000ms = 10hz); how often the tasks shall run
const int interruptTime = 200000;


// Global variables declarations:
// flags:
bool turnOnReq;
bool alarmTrig;
bool alarmAck;
volatile bool timeBaseFlag;
bool maxCurrFlag = false;
bool minCurrFlag = false;
bool maxVolFlag = false;
bool minVolFlag = false;
bool maxSocFlag = false;
bool minSocFlag = false;
bool resetLog = false;
//contractor flags
bool hvilAlarmFlag = false;
bool hvilInterruptAlarmFlag = false;
bool overCurrentAlarmFlag = false;
bool outOfRangeVoltageFlag = false;
//Remote terminal flag
bool resetEEProm = false;

// data:
int hvilData = 0;
int tempData;
float currentData;
float voltageData;
long isoResData;
int isoCKTData;
float socData;
// max/min history:
float maxCurrent;
float minCurrent;
float maxVoltage;
float minVoltage;
float maxSOC;
float minSOC;
// alarms:
String hvilAlarm;
String overcurrentAlarm;
String hvorAlarm;
String hvilInterrupt;
// for timing of specific tasks 
long counter = 0;


// Task Declarations
TCB tft;
TftData tftTaskData;
TCB measurement;
MeasurementData measurementTaskData;
TCB soc;
SocData socTaskData;
TCB contactor;
ContactorData contactorTaskData;
TCB alarm;
AlarmData alarmTaskData;
TCB remoteTerminal;
RemoteTerminalData remoteTerminalTaskData;
TCB dataLog;
DataLoggingData dataLogTaskData;

// Reference to top of the task queue and the task currently being executed
TCB* topOfQueue = &tft;
TCB* currentTask = topOfQueue;
TCB* lastTask = &alarm;

// Initialize the variables
void initVars(){
  tempData = 0;
  hvilData = 1;
  currentData = 10;
  voltageData = 335;
  isoResData = 0;
  isoCKTData = 0;
  socData = 0;
  maxCurrent = 0;
  minCurrent = 0;
  maxVoltage = -1;
  minVoltage = -1;
  maxSOC = -1;
  minSOC = -1;
  hvilAlarm = "NOT ACTIVE";
  overcurrentAlarm = "NOT ACTIVE";
  hvorAlarm = "NOT ACTIVE";
  hvilInterrupt = "NOT ACTIVE";
  turnOnReq = false;
  alarmTrig = false;
  alarmAck = false;
  pinMode(interruptPin, INPUT_PULLUP);
  timeBaseFlag = 0;
};

// Initialize task and their fields
void initTasks(){
  // TFT
  tft.myTask = &tftTaskFnc;
  tft.taskDataPtr = &tftTaskData;
  tftTaskData.tftSocPtr = &socData;
  tftTaskData.tftTempPtr = &tempData;
  tftTaskData.tftCurrPtr = &currentData;
  tftTaskData.tftVoltPtr = &voltageData;
  tftTaskData.tftIsoResPtr = &isoResData;
  tftTaskData.tftIsoCktPtr = &isoCKTData;
  tftTaskData.tftHvilAlarmPtr = &hvilAlarm;
  tftTaskData.tftOverCurrPtr = &overcurrentAlarm;
  tftTaskData.tftHvorPtr = &hvorAlarm;
  tftTaskData.tftHvilInterrupt = &hvilInterrupt;
  tftTaskData.tftTurnOnPtr = &turnOnReq;
  tftTaskData.tftHvilPtr = &hvilData;
  tftTaskData.tftAlarmTrigPtr = &alarmTrig;
  tftTaskData.tftAlarmAckPtr = &alarmAck;
  tftTaskData.tftCounterPtr = &counter;
  
  // MEASUREMENT
  measurement.myTask = &measurementTaskFnc;
  measurement.taskDataPtr = &measurementTaskData;
  measurementTaskData.currentDataPtr = &currentData;
  measurementTaskData.voltageDataPtr = &voltageData;
  measurementTaskData.maxCurrentPtr = &maxCurrent;
  measurementTaskData.maxVoltagePtr = &maxVoltage;
  measurementTaskData.minCurrentPtr = &minCurrent;
  measurementTaskData.minVoltagePtr = &minVoltage;
  measurementTaskData.maxCurrFlagPtr = &maxCurrFlag;
  measurementTaskData.maxVolFlagPtr = &maxVolFlag;
  measurementTaskData.minVolFlagPtr = &minVolFlag;
  measurementTaskData.minCurrFlagPtr = &minCurrFlag;

  // SOC
  soc.myTask = &socTaskFnc;
  soc.taskDataPtr = &socTaskData;
  // socTaskData.dataPtr = &socData;
  socTaskData.voltageDataPtr = &voltageData;
  socTaskData.currentDataPtr = &currentData;
  socTaskData.SOCDataPtr = &socData;
  socTaskData.maxCurrDataPtr = &maxCurrent;
  socTaskData.minCurrDataPtr = &minCurrent;
  socTaskData.maxCurrFlag = &maxCurrFlag;
  socTaskData.minCurrFlag = &minCurrFlag;
  socTaskData.maxVoltDataPtr = &maxVoltage;
  socTaskData.minVoltDataPtr = &minVoltage;
  socTaskData.maxVolFlag = &maxVolFlag;
  socTaskData.minVolFlag = &minVolFlag;
  socTaskData.maxSOC = &maxSOC;
  socTaskData.minSOC = &minSOC;
  socTaskData.maxSOCFlag = &maxSocFlag;
  socTaskData.minSOCFlag = &minSocFlag;

  // CONTACTOR
  contactor.myTask = &contactorTaskFnc;
  contactor.taskDataPtr = &contactorTaskData;
  contactorTaskData.turnOnReqPtr = &turnOnReq;
  //contactorTaskData.hvilInterruptPtr = &hvilInterrupt;
  contactorTaskData.hvilAlarmFlagPtr = &hvilAlarmFlag;
  contactorTaskData.hvilInterruptAlarmFlagPtr = &hvilInterruptAlarmFlag;
  contactorTaskData.overCurrentAlarmFlagPtr = &overCurrentAlarmFlag;
  contactorTaskData.outOfRangeVoltageFlagPtr = &outOfRangeVoltageFlag;

  // ALARM
  alarm.myTask = &alarmTaskFnc;
  alarm.taskDataPtr = &alarmTaskData;
  alarmTaskData.hvilAlarmPtr = &hvilAlarm;
  alarmTaskData.overcurrentAlarmPtr = &overcurrentAlarm;
  alarmTaskData.hvorAlarmPtr = &hvorAlarm;
  alarmTaskData.hvilInterruptPtr = &hvilInterrupt;
  alarmTaskData.alarmTriggerPtr = &alarmTrig; 
  alarmTaskData.alarmAckPtr = &alarmAck;
  alarmTaskData.hvilPtr = &hvilData;
  alarmTaskData.currentPtr = &currentData;
  alarmTaskData.voltagePtr = &voltageData;
  alarmTaskData.hvilAlarmFlagPtr = &hvilAlarmFlag;
  alarmTaskData.hvilInterruptAlarmFlagPtr = &hvilInterruptAlarmFlag;
  alarmTaskData.overCurrentAlarmFlagPtr = &overCurrentAlarmFlag;
  alarmTaskData.outOfRangeVoltageFlagPtr = &outOfRangeVoltageFlag;

  // REMOTE TERMINAL
  remoteTerminal.myTask = &remoteTaskFnc;
  remoteTerminal.taskDataPtr = &remoteTerminalTaskData;
  remoteTerminalTaskData.resetEEpromPtr = &resetLog;
  remoteTerminalTaskData.maxCurrDataPtr = &maxCurrent;
  remoteTerminalTaskData.minCurrDataPtr = &minCurrent;
  remoteTerminalTaskData.maxVoltDataPtr = &maxVoltage;
  remoteTerminalTaskData.minVoltDataPtr = &minVoltage;

  // DATA LOGGING
  dataLog.myTask = &dataLoggingTaskFnc;
  dataLog.taskDataPtr = &dataLogTaskData;
  dataLogTaskData.maxCurrentPtr = &maxCurrent;
  dataLogTaskData.minCurrentPtr = &minCurrent;
  dataLogTaskData.maxVoltagePtr = &maxVoltage;
  dataLogTaskData.minVoltagePtr = &minVoltage;
  dataLogTaskData.maxSOCPtr = &maxSOC;
  dataLogTaskData.minSOCPtr = &minSOC;
  dataLogTaskData.maxCurrFlagPtr = &maxCurrFlag;
  dataLogTaskData.minCurrFlagPtr = &minCurrFlag;
  dataLogTaskData.maxVolFlagPtr = &maxVolFlag;
  dataLogTaskData.minVolFlagPtr = &minVolFlag;
  dataLogTaskData.maxSocFlagPtr = &maxSocFlag;
  dataLogTaskData.minSocFlagPtr = &minSocFlag;
  dataLogTaskData.resetFlagPtr = &resetLog;
};

// Initialize the queue
void initQueue() {
  tft.prev = NULL;
  tft.next = &measurement;
  measurement.prev = &tft;
  measurement.next = &soc;
  soc.prev = &measurement;
  soc.next = &contactor;
  contactor.prev = &soc;
  contactor.next = &alarm;
  alarm.prev = &contactor;
  alarm.next = &remoteTerminal;
  remoteTerminal.prev = &alarm;
  remoteTerminal.next = &dataLog;
  dataLog.prev = &remoteTerminal;
  dataLog.next = NULL;
};

// Initialize the screen display
void initDisplay() {
  screen.reset();
  uint16_t identifier = screen.readID();
  if(identifier != 0x9325 && identifier != 0x9328 && identifier != 0x4535 && identifier != 0x7575 && identifier != 0x9341  && identifier != 0x8357) {
    identifier=0x9341;
  }
  screen.begin(identifier);
  screen.setRotation(2);
  screen.fillScreen(BLACK);
  
  // create main screen buttons
  for (uint8_t col=0; col<3; col++) {
    buttons[col].initButton(&screen, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                 BUTTON_Y,    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolor, ILI9341_WHITE,
                  buttonlabels[col], BUTTON_TEXTSIZE); 
    buttons[col].drawButton();
  }
  // create the back button
  buttons[3].initButton(&screen, BUTTON_X, 
                 300,    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolor, ILI9341_WHITE,
                  buttonlabels[3], BUTTON_TEXTSIZE); 
  // create the battery on button
  
  buttons[4].initButton(&screen, 100, 
                 150,    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolor, ILI9341_WHITE,
                  buttonlabels[4], BUTTON_TEXTSIZE);  
  // create the battery off button
  buttons[5].initButton(&screen, 180, 
                 150,    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolor, ILI9341_WHITE,
                  buttonlabels[5], BUTTON_TEXTSIZE); 

  // create the acknowledge button
  buttons[6].initButton(&screen, 130, 
                 150,    // x, y, w, h, outline, fill, text
                  120, BUTTON_H, ILI9341_WHITE, buttoncolor, ILI9341_WHITE,
                  buttonlabels[6], BUTTON_TEXTSIZE); 
                  
  // keep track of last screen displayed
  currScreen = 0;
  prevScreen = 0;
};

// function for the timed interrupt
void interruptScheduler() {
  timeBaseFlag = 1;  
};

// function for the pin interrupt
// puts it in Active, Not Acknowledged state, triggers the alarm, and makes sure the contactors are OFF
void interruptAlarm() { 
   hvilInterrupt = "ACTIVE, NOT ACKNOWLEDGED";
   hvilInterruptAlarmFlag = true;
   hvilInterState = 1;
   hvilInterTrig = true;
   digitalWrite(24, LOW);       
};

// startup task
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial1.setTimeout(1000);
  initVars();
  initTasks();
  initQueue();
  initDisplay();
  attachInterrupt(digitalPinToInterrupt(interruptPin), interruptAlarm, RISING);
  Timer1.initialize(interruptTime);
  Timer1.attachInterrupt(interruptScheduler);
}

// the scheduler; loops through the tasks if prompted by the timed interrupt, until it reached the last task
// measures HVIL input at every iteration
void loop() {
  if(1 == timeBaseFlag) {
    timeBaseFlag = 0; 
    currentTask = topOfQueue;
    while (currentTask != NULL) {
      currentTask->myTask(currentTask->taskDataPtr);
      currentTask = currentTask->next;
    }
  }
  hvilData = !digitalRead(interruptPin);
}
