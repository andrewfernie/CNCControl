//=========================================================
//Project: GRBL Pendant
//Module:  serialspy.ino        
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//----------------------------------------------------------

//
// ===============================================
// Includes
// ===============================================
//


#include <USBHost_t36.h>
#include <antplusdefs.h>
#include <Bounce2.h>
#include <Keypad.h>
#include <Key.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <Wire.h>                // I2C Communication
#include <LiquidCrystal_I2C.h>   // LCD over I2C
#include "config.h"
#include "menu.h"
#include "lcd_screens.h"
#include "Encoder2.h"

//
// ===============================================
// Defines
// ===============================================
//
constexpr auto ProgramVersion = 0.29;

//
// ===============================================
// Inits
// ===============================================
//
// -------------------------
// Inits for USB Host
// -------------------------

#ifdef GRBL_COMM_USB
USBHost grblUSB;
USBHub hub1(grblUSB);
USBHub hub2(grblUSB);
USBHIDParser hid1(grblUSB);
USBHIDParser hid2(grblUSB);
USBHIDParser hid3(grblUSB);
USBSerial grblUSBSerial(grblUSB);

USBDriver* drivers[] = { &hub1, &hub2, &hid1, &hid2, &hid3, &grblUSBSerial };
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char* driver_names[CNT_DEVICES] = { "Hub1", "Hub2",  "HID1", "HID2", "HID3", "grblUSBSerial1" };
bool driver_active[CNT_DEVICES] = { false, false, false, false };
#endif

#ifdef GRBL_COMM_UART

#endif


// -------------------------
// Inits for LCD control
// -------------------------
// Set the pins on the I2C chip used for LCD connections:
//                         addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C JogLCD(JogLCDAddress, LCD_EN, LCD_RW, LCD_RS, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 3, POSITIVE);  // Set the LCD I2C address
LiquidCrystal_I2C StatusLCD(StatusLCDAddress, LCD_EN, LCD_RW, LCD_RS, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 3, POSITIVE);  // Set the LCD I2C address

// -------------
// RotaryEncoder
// -------------

CEncoder2 uiEncoder(UI_ENC_A, UI_ENC_B, -4, UI_ENC_S);

long uiEncoderPosition = 0;
bool uiEncoderSwitch = false;


#ifdef JOG_ENC_A

CEncoder2 jogEncoder(JOG_ENC_A, JOG_ENC_B, 1);
Bounce jogResetButton = Bounce();
#endif

long lastStatusRXTime = 0;
long lastStateRXTime = 0;


// --------------
// System Timers
// --------------
uint32_t 		fast_loopTimer = 0;				// Time in miliseconds of main control loop
uint32_t 	    fast_loopTimeStamp = 0;			// Time Stamp when fast loop was complete
uint8_t 		delta_ms_fast_loop = 0; 		// Delta Time in miliseconds
int 			mainLoop_count = 0;

uint32_t 		medium_loopTimer = 0;			// Time in miliseconds of medium loop
byte 			medium_loopCounter = 0;			// Counters for branching from main control loop to slower loops
uint8_t			delta_ms_medium_loop = 0;

uint32_t 		slow_loopTimer = 0;				// Time in miliseconds of medium loop
byte 			slow_loopCounter = 0;
uint8_t 		delta_ms_slow_loop = 0; 		// Delta Time in miliseconds
byte 			superslow_loopCounter = 0;
byte			counter_one_herz = 0;

// ------------
// Keypad Setup
// ------------
const byte rows = 5; //four rows
const byte cols = 4; //four columns
char keys[rows][cols] = {
  {'1','2','3','4'},
  {'5','6','7','8'},
  {'9','0','A','B'},
  {'C','D','E','F'},
  {'G','H','I','J'}
};
#ifdef TEENSY32
byte rowPins[rows] = { 14, 15, 16, 17, 12 }; //connect to the row pinouts of the keypad
byte colPins[cols] = { 20, 21, 22, 23 }; //connect to the column pinouts of the keypad
#else
byte rowPins[rows] = { 33, 34, 35, 36, 37 }; //connect to the row pinouts of the keypad
byte colPins[cols] = { 38, 39, 40, 41 }; //connect to the column pinouts of the keypad
#endif

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

// ------------
// Pendant Mode
// ------------
enum class PendantModes { Monitor, Control };
PendantModes pendantMode = PendantModes::Monitor;

// ------------
// Set mode
// ------------
enum class SetMode { Feed, Move, Spindle };
SetMode currentSetMode = SetMode::Move;

// ------------
// Menu Mode
// ------------
enum class MenuModes { Menu, Status };
MenuModes menuMode = MenuModes::Status;

// --------------------------
// CNC Controller Status Data 
// --------------------------
unsigned long lastCall = 0;

// State codes for Grbl 1.1.   Source https://github.com/gnea/grbl/wiki/Grbl-v1.1-Commands
// 
// Motion Mode                 G0, G1, G2, G3, G38.2, G38.3, G38.4, G38.5, G80
enum class MotionMode { Undefined, Rapid, Linear, CW, CCW, Probe_2, Probe_3, Probe_4, Probe_5, Cancel };
MotionMode currentMotionMode = MotionMode::Undefined;

// Coordinate System Select    G54, G55, G56, G57, G58, G59
enum class CoordinateSystemSelect { Undefined, WCS1, WCS2, WCS3, WCS4, WCS5, WCS6 };
CoordinateSystemSelect currentCoordinateSystemSelect = CoordinateSystemSelect::Undefined;

// Plane Select                G17, G18, G19
enum class PlaneSelect { Undefined, XY, ZX, YZ };
PlaneSelect currentPlaneSelect = PlaneSelect::Undefined;

// Distance Mode               G90, G91
enum class DistanceMode { Undefined, Absolute, Incremental };
DistanceMode currentDistanceMode = DistanceMode::Undefined;

// Arc IJK Distance Mode       G91.1
enum class ArcDistanceMode { Off, On };
ArcDistanceMode currentArcDistanceMode = ArcDistanceMode::On;

// Feed Rate Mode              G93, G94
enum class FeedRateMode { Undefined, Inverse, Normal };
FeedRateMode currentFeedRateMode = FeedRateMode::Undefined;

// Units Mode                  G20, G21
enum class UnitsMode { Undefined, Inches, mm };
UnitsMode currentUnitsMode = UnitsMode::Undefined;

// Cutter Radius Compensation  G40
enum class CutterRadiusCompensation { Off, On };
CutterRadiusCompensation currentCutterRadiusCompensation = CutterRadiusCompensation::Off;

// Tool Length Offset          G43.1, G49
enum class ToolLengthOffsetMode { Undefined, Dynamic, Cancel };
ToolLengthOffsetMode currentToolLengthOffsetMode = ToolLengthOffsetMode::Undefined;

// Program Mode                M0, M1, M2, M30
enum class ProgramMode { Undefined, Stop, Optional, End, Rewind };
ProgramMode currentProgramMode = ProgramMode::Undefined;


// Coolant State               M7, M8, M9
enum class CoolantState { Undefined, Mist, Flood, Off };
CoolantState currentCoolantState = CoolantState::Undefined;

// GRBL controller states
enum class GRBLStates { Undefined, Idle, Run, Hold, Jog, Alarm, Door, Check, Home, Sleep };
GRBLStates grblState = GRBLStates::Undefined;

// Co-ordinate system
enum class GRBLCoord { Undefined, MPos, WPos };
GRBLCoord grblCoord = GRBLCoord::Undefined;

// Tool
char currentTool[] = "   ";

// Feed rate
float currentFeedRate = 0.0;

// Spindle speed
float spindleSpeed = 0.0;


float currentOvFeedRatePercent;  //Override Percent
float currentOvRapidRatePercent;  //Override Percent

char lastMessage[BufferSize];
unsigned long lastMessageTime = 0;
int  alarmNumber = 0;			// Alarm message number
unsigned long lastAlarmTime = 0;
int  errorNumber = 0;			// Error message number
unsigned long lastErrorTime = 0;

struct AxisData
{
    float x;
    float y;
    float z;
};
AxisData currentPosition;

AxisData currentWCO;

uint32_t  lastIdleTimeoutCheck = 0;

// --------------
// Jog parameters
// --------------
enum class CNCAxis { X, Y, Z };
CNCAxis currentJogAxis = CNCAxis::X;

float   jogSize[] = { 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0 }; // values are mm per encoder revolution. 
constexpr auto defaultJogSizeIndex = 3;
uint8_t maxJogSizeIndex = sizeof(jogSize) / sizeof(jogSize[0]) - 1;
uint8_t currentJogSizeIndex = defaultJogSizeIndex;
float   adjustableJogSize = 10.0;
uint8_t enableAdjustableJogSize = false;
bool stopJogCommand = false;



float   jogRate[] = { 50.0, 100.0, 200.0, 500.0, 1000.0, 2000.0 }; // values are "units" (mm or inches) per minute
constexpr auto defaultJogRateIndex = 3;
uint8_t maxJogRateIndex = sizeof(jogRate) / sizeof(jogRate[0]) - 1;
uint8_t currentJogRateIndex = defaultJogRateIndex;


// --------------
// Jog parameters
// --------------

// Spindle State               M3, M4, M5
enum class SpindleState { Undefined, CW, CCW, Off };
SpindleState currentSpindleState = SpindleState::Off;
float currentSpindleSpeed = 0.0;

float   spindleRPM[] = { 8000.0, 10000.0, 12000.0, 14000.0, 16000.0, 18000.0, 20000.0, 22000.0, 24000.0 };
constexpr auto defaultSpindleRPMIndex = 3;
uint8_t maxSpindleRPMIndex = sizeof(spindleRPM) / sizeof(spindleRPM[0]) - 1;
uint8_t currentSpindleRPMIndex = defaultSpindleRPMIndex;

int grblCommandCount = 0;
int grbl_last_command_count = 0;
float lastJogCommandPosition = 0.0;

// --------------
// Menu 
// --------------

uint8_t menuVarUnits;
float   menuJogSpeedXY = 1.0;
float   menuJogSpeedZ = 1.0;

float menuVarDummy1;
int menuVarDummy2;
uint8_t menuVarDummy3;
uint8_t menuVarDummy4;

uint8_t menuEditMode;

struct MenuParameterItem menuParameters[] = {
    {ParamInMm,  "Units      ",  0,     0.,     0., (void*)&menuVarUnits},
    {ParamFloat, "Jog Spd. XY",  0,     0.,   100., (void*)&menuJogSpeedXY},
    {ParamFloat, "Jog Spd. Z ",  0,     0.,   100., (void*)&menuJogSpeedZ},
    {ParamFloat, "Dummy 1    ",  2,   -20.,   100., (void*)&menuVarDummy1},
    {ParamInt,   "Dummy 2    ",  0,  -999.,  1000., (void*)&menuVarDummy2},
    {ParamOnOff, "Dummy 3    ",  0,     0.,     0., (void*)&menuVarDummy3},
    {ParamYesNo, "Dummy 4    ",  0,     0.,     0., (void*)&menuVarDummy4}
};

Menu menuObject(menuParameters, sizeof(menuParameters) / sizeof(MenuParameterItem), &StatusLCD, &uiEncoderPosition);

// ===============================================
// Main
// ===============================================
 //

 //  ---------- Setup  ----------- //

void setup()
{
    debugSerial.begin(DebugSerialSpeed);   // open serial for debug/status messages

    pinMode(DEBUG_BROWN, OUTPUT);
    pinMode(DEBUG_RED, OUTPUT);
    pinMode(DEBUG_ORANGE, OUTPUT);
    pinMode(DEBUG_YELLOW, OUTPUT);

#ifdef UI_ENC_Sxxx
    // set Select pin from Rotary Encoder to input
    pinMode(UI_ENC_S, INPUT);      // sets the encoder select digital pin
#endif

    uiEncoder.SetMinMaxPosition(0, 7);

    // Setup the first button with an internal pull-up :
    pinMode(JogResetPin, INPUT_PULLUP);
    // After setting up the button, setup the Bounce instance :
    jogResetButton.attach(JogResetPin);
    jogResetButton.interval(5); // interval in ms

   // init LCD Displays //

    StatusLCD.begin(LCDCols, LCDRows);
    JogLCD.begin(LCDCols, LCDRows);

    // This is the serial connect to PC, we get some commands
    // but we can also print some additional information about this module
    // and the parser from Client program will ignore this
    debugSerial.print(F("<GRBLPendant "));
    debugSerial.print(ProgramVersion);
    debugSerial.println(F(">"));
    debugSerial.println(F("<All commands start with a colon ':'>"));
    debugSerial.println(F("<Call help with ':?'>"));


    StatusLCD.setCursor(0, 0); // letter, row
    StatusLCD.print(F("GRBL Pendant "));
    StatusLCD.print(ProgramVersion);
    StatusLCD.setCursor(0, 1); // letter, row
    StatusLCD.print(F("Connect ... "));
    StatusLCD.setCursor(0, 2); // letter, row
    StatusLCD.print(F("Status LCD "));

    JogLCD.setCursor(0, 0); // letter, row
    JogLCD.print(F("GRBL Pendant "));
    JogLCD.print(ProgramVersion);
    JogLCD.setCursor(0, 1); // letter, row
    JogLCD.print(F("Connect ... "));
    JogLCD.setCursor(0, 2); // letter, row
    JogLCD.print(F("Jog LCD "));

    uiEncoder.SetMinMaxPosition(0, menuObject.GetNumMenuItems() - 1);

    delay(2000);


#ifdef GRBL_COMM_USB
    // open USB host port to GRBL
    grblUSB.begin();
    //	grblUSBSerial.begin(USBBAUD, USBFORMAT);
#endif

#ifdef GRBL_COMM_UART
    // open serial port to GRBL
    grblSerial.begin(GRBLSerialSpeed);
#endif

    // reset grbl device (ctrl-X) for Universal Gcode Sender
    GrblCommWriteChar(0x18);

    // open serial port to G Code senser 
    gsSerial.begin(GSSerialSpeed);

    StatusLCD.clear();
    JogLCD.clear();

}//SETUP



//  ---------- Loop ----------- //
int pc = 0;
int gr = 0;
int pc_chars = 0;
int gr_chars = 0;
int loopCount = 0;
char pcserial[BufferSize];
char grserial[BufferSize];
char lcdRowString[LCDCols];
uint32_t  lastLCDOut = 0;


void loop()
{
    // Jobs

    USBDeviceCheck(USBBAUD, USBFORMAT);

    DEBUG_DIGITALWRITE_HIGH(DEBUG_BROWN);
    SerialIOGRBL();
    DEBUG_DIGITALWRITE_LOW(DEBUG_BROWN);

    DEBUG_DIGITALWRITE_HIGH(DEBUG_RED);
    SerialIOGS();
    DEBUG_DIGITALWRITE_LOW(DEBUG_RED);


    if (grblCommandCount != grbl_last_command_count)
    {
        //DEBUG_PRINT("Command Count:");
        //DEBUG_PRINTLN(grblCommandCount);
        grbl_last_command_count = grblCommandCount;
    }

    // Update the Bounce instance
    jogResetButton.update();

    // We want this to execute at 50Hz if possible
    // -------------------------------------------
    if (millis() - fast_loopTimer > 19) {

        delta_ms_fast_loop = millis() - fast_loopTimer;

        fast_loopTimer = millis();

        mainLoop_count++;

        // Execute the fast (nominal 50Hz) loop
        // ------------------------------------
        fast_loop();

        // Execute the medium loop (internally sub-banded by 5 to a nominal 10Hz)
        // -----------------------------------------------------------------------
        medium_loop();

        // Execute the slow (nominal 1Hz) loop
        // ------------------------------------
        counter_one_herz++;
        if (counter_one_herz >= 50)
        {
            one_second_loop();
            counter_one_herz = 0;
        }

        fast_loopTimeStamp = millis();

    }

}//LOOP


// Main loop
void fast_loop()
{
    long jogPosition = 0;

    // This is the fast loop
    // ---------------------
    DEBUG_DIGITALWRITE_HIGH(DEBUG_ORANGE);

    uiEncoderPosition = uiEncoder.GetPosition();
    uiEncoderSwitch = uiEncoder.ReadSwitch();

    if (uiEncoderSwitch)
    {
        if (menuMode == MenuModes::Menu)
        {
            if (!menuEditMode)
            {
                menuObject.EditItem(uiEncoderPosition);
                menuEditMode = false;
            }
        }
        else
        {
            menuEditMode = false;
        }
    }

    long jogResetButton = ReadJogResetButton();

    if (!jogResetButton)
    {
        jogPosition = jogEncoder.GetPosition();
    }
    else
    {
        jogPosition = 0;
        lastJogCommandPosition = 0.0;
        jogEncoder.Reset();
    }


#ifdef JOG_ENC_S
    jogEncoderSwitch = ReadJogEncoderSwitch();

    if (jogEncoderSwitch)
    {
        ResetJogEncoderCount();
    }
#endif

    if (pendantMode == PendantModes::Control)
    {
        float jog_move;

        if (!jogResetButton)
            jog_move = float(jogPosition) / JogEncCount * getJogSize();
        else
            jog_move = 0.0;

        if (stopJogCommand)
        {
            lastJogCommandPosition = jog_move;
        }
        else {
            if (fabs(jog_move - lastJogCommandPosition) >= 0.001)
            {
                if (grblCommandCount < 3)
                {
                    float jogDelta = jog_move - lastJogCommandPosition;
                    SendJogCommand(jogDelta);
                    lastJogCommandPosition = lastJogCommandPosition + jogDelta;
                }
            }
        }
    }
    stopJogCommand = false;
    DEBUG_DIGITALWRITE_LOW(DEBUG_ORANGE);

}

void medium_loop()
{
    // This is the start of the medium loop 
    // -----------------------------------------
    // The routine is called at 50Hz, so each of the five subbanded 
    // blocks is called at 10Hz

    char key;
    DEBUG_DIGITALWRITE_HIGH(DEBUG_YELLOW);

    switch (medium_loopCounter)
    {
    case 0:
        medium_loopCounter++;

        delta_ms_medium_loop = millis() - medium_loopTimer;
        medium_loopTimer = millis();

        break;

        //---------------------------------------------
    case 1:
        medium_loopCounter++;

        key = keypad.getKey();

        if (key != NO_KEY)
        {
            //char tmpstr[2];
            //tmpstr[0] = key;
            //tmpstr[1] = '\0';
            //DEBUG_PRINT(tmpstr);
            ProcessKey(key);
        }
        break;

        //------------------------------
    case 2:
        medium_loopCounter++;

        break;

        //--------------------------------------------
    case 3:
        medium_loopCounter++;

        break;

        // This case controls the slow loop
        //---------------------------------
    case 4:
        medium_loopCounter = 0;

        slow_loop();

        break;
    }
    DEBUG_DIGITALWRITE_LOW(DEBUG_YELLOW);

}

void slow_loop()
{
    // This is the slow (2Hz) loop pieces
    //----------------------------------------
    // The routine is called at 10Hz, so each of the five subbanded 
    // blocks is called at 2Hz


    switch (slow_loopCounter) {
    case 0:
        slow_loopCounter++;

        delta_ms_slow_loop = millis() - slow_loopTimer;
        slow_loopTimer = millis();

        if (menuMode == MenuModes::Status)
            DisplayState();
        else
            menuObject.Draw();

        break;

    case 1:
        slow_loopCounter++;

        DisplayJogScreen();

        break;

    case 2:
        slow_loopCounter++;

        if ((pendantMode == PendantModes::Control) && (millis() - lastStatusRXTime > 1000))
        {
            SendGRBLCommand_NoCount("?");
        }
        break;

    case 3:
        slow_loopCounter++;

        if ((pendantMode == PendantModes::Control) && (millis() - lastStateRXTime > 1000))
        {
            SendGRBLCommand("$G\n");
        }

        break;

    case 4:
        slow_loopCounter = 0;

        //		DEBUG_PRINTLN(ReadUIEncoder());

        //      if (ReadUIEncoderSwitch())
        //	       DEBUG_PRINTLN("TRUE");
        //      else
        //	       DEBUG_PRINTLN("FALSE");

        break;
    }

}

void one_second_loop()
{
    if (pendantMode == PendantModes::Control)
    {
        if (grblState == GRBLStates::Idle)
        {
            if (millis() - lastIdleTimeoutCheck > 5000)
            {
                ResetGRBLCommandCount();
            }
        }
        else
        {
            lastIdleTimeoutCheck = millis();
        }
    }
    else
    {
        grblCommandCount = 0;
    }
}

// ---------- Subroutines -----------

//
// split a string on one or more delimiter
// and return split at index
// split(char* string[] = "1,2;3")
//
char* split(char* string, char* delimiter, int index)
{
    char* ptr;

    char buffer[BufferSize];
    strcpy(buffer, string);

    // init and create first cut
    ptr = strtok(buffer, delimiter);
    int x = 0;
    while (ptr != NULL) {
        if (x++ >= index)
            break;
        // next one
        ptr = strtok(NULL, delimiter);
    }

    return ptr;
}


void set_grblState_from_chars(char* tmp)
{
    if (strcmp(tmp, "Idle") == 0)    grblState = GRBLStates::Idle;
    if (strcmp(tmp, "Run") == 0)     grblState = GRBLStates::Run;
    if (strcmp(tmp, "Hold") == 0)    grblState = GRBLStates::Hold;
    if (strcmp(tmp, "Jog") == 0)     grblState = GRBLStates::Jog;
    if (strcmp(tmp, "Alarm") == 0)   grblState = GRBLStates::Alarm;
    if (strcmp(tmp, "Door") == 0)    grblState = GRBLStates::Door;
    if (strcmp(tmp, "Check") == 0)   grblState = GRBLStates::Check;
    if (strcmp(tmp, "Home") == 0)    grblState = GRBLStates::Home;
    if (strcmp(tmp, "Sleep") == 0)   grblState = GRBLStates::Sleep;
}

float getJogRate()
{
    return jogRate[currentJogRateIndex];
}

uint8_t setJogRateIndex(uint8_t index)
{
    currentJogRateIndex = max(0, min(index, maxJogRateIndex));
    return currentJogRateIndex;
}

uint8_t setJogRateDefault()
{
    currentJogRateIndex = defaultJogRateIndex;
    return currentJogRateIndex;
}

uint8_t incrementJogRateIndex()
{
    if (currentJogRateIndex < maxJogRateIndex)
    {
        currentJogRateIndex++;
    }
    return currentJogRateIndex;
}

uint8_t decrementJogRateIndex()
{
    if (currentJogRateIndex > 0)
    {
        currentJogRateIndex--;
    }
    return currentJogRateIndex;
}


float getJogSize()
{
    return jogSize[currentJogSizeIndex];
}

uint8_t setJogSizeIndex(uint8_t index)
{
    currentJogSizeIndex = max(0, min(index, maxJogSizeIndex));
    enableAdjustableJogSize = false;
    return currentJogSizeIndex;
}

uint8_t setJogSizeDefault()
{
    currentJogSizeIndex = defaultJogSizeIndex;
    enableAdjustableJogSize = false;
    return currentJogSizeIndex;
}

uint8_t incrementJogSizeIndex()
{
    if (currentJogSizeIndex < maxJogSizeIndex)
    {
        currentJogSizeIndex++;
    }
    enableAdjustableJogSize = false;
    return currentJogSizeIndex;
}

uint8_t decrementJogSizeIndex()
{
    if (currentJogSizeIndex > 0)
    {
        currentJogSizeIndex--;
    }
    enableAdjustableJogSize = false;
    return currentJogSizeIndex;
}



float getSpindleRPM()
{
    return spindleRPM[currentSpindleRPMIndex];
}

uint8_t setSpindleRPMIndex(uint8_t index)
{
    currentSpindleRPMIndex = max(0, min(index, maxSpindleRPMIndex));
    return currentSpindleRPMIndex;
}

uint8_t setSpindleRPMDefault()
{
    currentSpindleRPMIndex = defaultSpindleRPMIndex;
    return currentSpindleRPMIndex;
}

uint8_t incrementSpindleRPMIndex()
{
    if (currentSpindleRPMIndex < maxSpindleRPMIndex)
    {
        currentSpindleRPMIndex++;
    }
    return currentSpindleRPMIndex;
}

uint8_t decrementSpindleRPMIndex()
{
    if (currentSpindleRPMIndex > 0)
    {
        currentSpindleRPMIndex--;
    }
    return currentSpindleRPMIndex;
}

uint8_t findClosestSpindleRPMIndex(float rpm)
{
    float delta = 9999999.;
    uint8_t closestIndex = 0;

    for (int i = 0; i <= maxSpindleRPMIndex; i++)
    {
        if (fabs(rpm - spindleRPM[i]) < delta)
        {
            closestIndex = i;
        }
    }
    closestIndex = max(0, min(closestIndex, maxSpindleRPMIndex));

    return closestIndex;
}


uint32_t freeMem()
{
    // for Teensy 3.2
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t)&stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t)hTop;
    free(hTop);

    // The difference is (approximately) the free, available ram.
    return stackTop - heapTop;
}