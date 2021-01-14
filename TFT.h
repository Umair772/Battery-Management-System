#include <TouchScreen.h>
#include <Elegoo_TFTLCD.h>
#include <Elegoo_GFX.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******************* UI details */
#define BUTTON_X 40
#define BUTTON_Y 150
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 0.1

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920
// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65
#define MINPRESSURE 10
#define MAXPRESSURE 1000

struct TftDataStruct {
  float* tftSocPtr;
  int* tftTempPtr;
  float* tftCurrPtr;
  float* tftVoltPtr;
  long* tftIsoResPtr;
  int* tftHvilPtr;
  int* tftIsoCktPtr;
  String* tftHvilAlarmPtr;
  String* tftOverCurrPtr;
  String* tftHvorPtr;
  String* tftHvilInterrupt;
  bool* tftTurnOnPtr;
  bool* tftAlarmTrigPtr;
  bool* tftAlarmAckPtr;
  long* tftCounterPtr;
};

typedef struct TftDataStruct TftData;


// declaration of all buttons' labels
char buttonlabels[7][20] = {"Meas", "Alarm", "Batt", "back", "ON", "OFF", "Acknowledge"};

// initialize variables to customize display
Elegoo_TFTLCD screen(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Elegoo_GFX_Button buttons[7];
uint16_t buttoncolor = ILI9341_BLUE;

// keep track of the screen is displayed; 0: main, 1: measurement, 2: alarm, 3: battery
int currScreen;
int prevScreen;

// keep track of the values on the display, so that the display only refreshes when values change
float dispCurr;
float dispVolt;
int dispHvil;
String dispHvilAl;
String dispHvilInt;
String dispOverC;
String dispHvor;

// make sure 2 buttons can't be pressed
bool buttonPress = false;

// store which button was pressed
bool b0 = false;
bool b1 = false;
bool b2 = false;
bool b3 = false;
bool b4 = false;
bool b5 = false;
bool b6 = false;

// display the measurement screen
void measurementScreen(int soc, int temp, float curr, float vol, long isoRes, int isoCkt, int hvil, bool update);

// display the alarm screen
void alarmScreen(String hvilAlarm, String hvilInterrupt, String overcurrent, String hvor, bool update, bool acknowledge, bool addButt);

// display the battery screen
void batteryScreen();

// run just the touch screen part of the display task
void runTouchScreen(bool alarmAck);

// depending on which button was pressed, change to relevant screen, and display relevant values and buttons
void tftTaskFnc(void* arg)
{
  
  TftData* localDataPtr = arg;
  bool alarmAck = *(localDataPtr->tftAlarmAckPtr);
  // if (!buttonPress) {
  runTouchScreen(alarmAck);
  // }
  // if (*(localDataPtr->tftCounterPtr) % 10 == 0) {
  int soc = *(localDataPtr->tftSocPtr);
  int temp = *(localDataPtr->tftTempPtr);
  float curr = *(localDataPtr->tftCurrPtr);
  float vol = *(localDataPtr->tftVoltPtr);
  long isoRes = *(localDataPtr->tftIsoResPtr);
  int isoCkt = *(localDataPtr->tftIsoCktPtr);
  String hvilAlarm = *(localDataPtr->tftHvilAlarmPtr);
  String overcurrent = *(localDataPtr->tftOverCurrPtr);
  String hvor = *(localDataPtr->tftHvorPtr);
  String hvilInter = *(localDataPtr->tftHvilInterrupt );
  int hvil = *(localDataPtr->tftHvilPtr);
  bool alarmTrig = *(localDataPtr->tftAlarmTrigPtr);

    
  if (alarmTrig) {
    // Switch to Acknowledge alarm screen
    if (currScreen != 2) {
      prevScreen = currScreen;
    }
    alarmScreen(hvilAlarm, hvilInter, overcurrent, hvor, (currScreen==2), true, (currScreen==2 && prevScreen != 2));
    currScreen = 2;
    if (buttons[6].justPressed()) { // Acknowledge button was pressed
      *(localDataPtr->tftAlarmTrigPtr) = false;
      *(localDataPtr->tftAlarmAckPtr) = true;
      // change to the previous screen if it wasn't the alarm screen
      if (prevScreen == 0) {
        screen.fillScreen(BLACK);
        for (int i = 0; i < 3; i++) {
          buttons[i].drawButton();
        }
      } else if (prevScreen == 1) {
        measurementScreen(soc, temp, curr, vol, isoRes, isoCkt, hvil, false);
      } else if (prevScreen == 3) {
        batteryScreen();
      }
      currScreen = prevScreen;
      // b6 = false;
    }
  } else if (buttons[0].justPressed()) { // measurement button pressed, switch to the measurment screen
    currScreen = 1;
    measurementScreen(soc, temp, curr, vol, isoRes, isoCkt, hvil, false);
    // b0 = false;
  } else if(buttons[1].justPressed()) { // alarm button pressed, switch to the alarm screen
    currScreen = 2;
    alarmScreen(hvilAlarm, hvilInter, overcurrent, hvor, false, false, false);
    // b1 = false;
  } else if(buttons[2].justPressed()) { // battery button pressed, switch to the battery screen
    currScreen = 3;
    batteryScreen();
    // b2 = false;
  } else if (buttons[3].justPressed()){  // back button pressed, switch back to main screen
    currScreen = 0;
    screen.fillScreen(BLACK);
    // b3 = false;
    for (int i = 0; i < 3; i++) {
      buttons[i].drawButton();
    }
  } else if (buttons[4].justPressed()) { // turn on the battery
    *(localDataPtr->tftTurnOnPtr) = true;
    // b4 = false;
  } else if (buttons[5].justPressed()) { // turn off the battery
    *(localDataPtr->tftTurnOnPtr) = false;
    // b5 = false;
  } else { // no button was pressed, update values being displayed (if on measurement/alarm screens)
    if (currScreen == 1) {
      measurementScreen(soc, temp, curr, vol, isoRes, isoCkt, hvil, true);
    } else if (currScreen == 2) {
      alarmScreen(hvilAlarm, hvilInter, overcurrent, hvor, true, !alarmAck, false);
    }
  }
  // }
};

void runTouchScreen(bool alarmAck) {
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to screen.width
    p.x = map(p.x, TS_MINX, TS_MAXX, screen.width(), 0);
    p.y = (screen.height()-map(p.y, TS_MINY, TS_MAXY, screen.height(), 0));
  }
  if (p.x>40 && p.x<100 && p.y>150 && p.y<180 && currScreen == 0) {
    buttons[0].press(true);
    // b0 = true;
  } else if (p.x>120 && p.x<160 && p.y>150 && p.y<180 && currScreen == 0) {
    buttons[1].press(true);
    // b1 = true;
  } else if (p.x>180 && p.x<220 && p.y>150 && p.y<180 && currScreen == 0) {
    buttons[2].press(true);
    // b2 = true;
  } else if (p.x>40 && p.x<100 && p.y>300 && p.y<330 && currScreen != 0) {
    buttons[3].press(true);
    // b3 = true;
  } else if (p.x>100 && p.x<160 && p.y>150 && p.y<180 && currScreen == 3) {
    buttons[4].press(true);
    // b4 = true;
  } else if (p.x>180 && p.x<240 && p.y>150 && p.y<180 && currScreen == 3) {
    buttons[5].press(true);
    // b5 = true;
  } else if (p.x>130 && p.x<250 && p.y>150 && p.y<180 && currScreen == 2 && !alarmAck) {
    buttons[6].press(true);
    // b6 = true;
  } else {
    buttons[0].press(false);
    buttons[1].press(false);
    buttons[2].press(false);
    buttons[3].press(false);
    buttons[4].press(false);
    buttons[5].press(false);
    buttons[6].press(false);
    // b0 = false;
    // b1 = false;
    // b2 = false;
    // b3 = false;
    // b4 = false;
    // b5 = false;
    // b6 = false;
  }
  // buttonPress = b0 || b1 || b2 || b3 || b4 || b5 || b6;
}

// handles the measurement screen display
void measurementScreen(int soc, int temp, float curr, float vol, long isoRes, int isoCkt, int hvil, bool update) {
  if (update) {
    // only update those variables that have changed since last iteration
    if (curr != dispCurr) {
      dispCurr = curr;
      screen.fillRect(200, 55, 100, 20, BLACK);
    }
    if (vol != dispVolt) {
      dispVolt = vol;
      screen.fillRect(200, 80, 100, 20, BLACK);
    }
    if(hvil != dispHvil) {
      dispHvil = hvil;
      screen.fillRect(200, 155, 100, 20, BLACK);
    }
  } else {
    screen.fillScreen(BLACK);
    buttons[3].drawButton();
  }
  screen.setTextColor(WHITE);  
  screen.setTextSize(1.5);

  // print SOC 
  screen.setCursor(5, 5);
  screen.print("SOC:");
  screen.setCursor(200, 5);
  screen.print(soc);

  // print temp
  screen.setCursor(5, 30);
  screen.print("TEMPERATURE:");
  screen.setCursor(200, 30);
  screen.print(temp);

  // print HV Current
  screen.setCursor(5, 55);
  screen.print("HV Current:");
  screen.setCursor(200, 55);
  screen.print(curr);
  
  // print HV Voltage
  screen.setCursor(5, 80);
  screen.print("HV Voltage:");
  screen.setCursor(200, 80);
  screen.print(vol);

  // print HV Iso Res
  screen.setCursor(5, 105);
  screen.print("HV Iso Res:");
  screen.setCursor(200, 105);
  screen.print(isoRes);

  // print HV Iso CKT
  screen.setCursor(5, 130);
  screen.print("HV Iso CKT:");
  screen.setCursor(200, 130);
  screen.print(isoCkt);

  // print HVIL
  screen.setCursor(5, 155);
  screen.print("HVIL:");
  screen.setCursor(200, 155);
  screen.print(hvil);
}

// handles the alarm screen display
void alarmScreen(String hvilAlarm, String hvilInter, String overcurrent, String hvor, bool update, bool acknowledge, bool addButt) {
  if (update) {
    // only update those alarm states that have changed since last iteration
    if (hvilAlarm != dispHvilAl) {
      dispHvilAl = hvilAlarm;
      screen.fillRect(95, 20, 250, 20, BLACK);
    }
    if (hvilInter != dispHvilInt) {
      dispHvilInt = hvilInter;
      screen.fillRect(95, 50, 250, 20, BLACK);
    }
    if (overcurrent != dispOverC) {
      dispOverC = overcurrent;
      screen.fillRect(95, 80, 250, 20, BLACK);
    }
    if (hvor != dispHvor) {
      dispHvor = hvor;
      screen.fillRect(95, 110, 250, 20, BLACK);
    }
  } else {
    screen.fillScreen(BLACK);
    buttons[3].drawButton();
  }

  // draw the ACKNOWLEDGE button, if alarms triggered, and button not on screen yet
  if (acknowledge && (!update || addButt)) {
    buttons[6].drawButton();
  }

  screen.setTextColor(WHITE);  
  screen.setTextSize(1.5);

  // print HVIL Alarm 
  screen.setCursor(5, 5);
  screen.print("HVIL Alarm:");
  screen.setCursor(95, 20);
  screen.print(hvilAlarm);

  // print HVIL Interlock 
  screen.setCursor(5, 35);
  screen.print("HVIL Interrupt:");
  screen.setCursor(95, 50);
  screen.print(hvilInter);

  // print Overcurrent Alarm 
  screen.setCursor(5, 65);
  screen.print("Overcurrent:");
  screen.setCursor(95, 80);
  screen.print(overcurrent);

  // print HVOR Alarm
  screen.setCursor(5, 95);
  screen.print("HVOR Alarm:");
  screen.setCursor(95, 110);
  screen.print(hvor);
}

// handles battery screen display
void batteryScreen() {
  screen.fillScreen(BLACK);
  buttons[3].drawButton();
  buttons[4].drawButton();
  buttons[5].drawButton();
}