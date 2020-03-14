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

//
// ===============================================
// Defines
// ===============================================
//
constexpr auto ProgramVersion = 0.24;

//
// ===============================================
// Inits
// ===============================================
//

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

#ifdef UI_ENC_A
Encoder uiEncoder(UI_ENC_A, UI_ENC_B);
long uiEncoderPosition = -999;
#endif

#ifdef UI_ENC_S
bool uiEncoderSwitch = false;
#endif

#ifdef JOG_ENC_A
Encoder jogEncoder(JOG_ENC_A, JOG_ENC_B);
long jogEncoderPosition = -999;
Bounce jogResetButton = Bounce();
long   lastJogEncoderPosition = 0;
#endif

#ifdef JOG_ENC_S
bool jogEncoderSwitch = false;
#endif


long lastStatusRXTime = 0;
long lastStateRXTime = 0;


// --------------
// System Timers
// --------------
unsigned long 	fast_loopTimer;				// Time in miliseconds of main control loop
unsigned long 	fast_loopTimeStamp;			// Time Stamp when fast loop was complete
uint8_t 		delta_ms_fast_loop; 		// Delta Time in miliseconds
int 			mainLoop_count;

unsigned long 	medium_loopTimer;			// Time in miliseconds of medium loop
byte 			medium_loopCounter;			// Counters for branching from main control loop to slower loops
uint8_t			delta_ms_medium_loop;

unsigned long 	slow_loopTimer;			// Time in miliseconds of medium loop
byte 			slow_loopCounter;
uint8_t 		delta_ms_slow_loop; 		// Delta Time in miliseconds
byte 			superslow_loopCounter;
byte			counter_one_herz;

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
byte rowPins[rows] = { 14, 15, 16, 17, 12 }; //connect to the row pinouts of the keypad
byte colPins[cols] = { 20, 21, 22, 23 }; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

// ------------
// Pendant Mode
// ------------
enum class PendantModes { Monitor, Control };
PendantModes pendantMode = PendantModes::Monitor;

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

// Spindle State               M3, M4, M5
enum class SpindleState { Undefined, CW, CCW, Off };
SpindleState currentSpindleState = SpindleState::Undefined;

// Coolant State               M7, M8, M9
enum class CoolantState { Undefined, Mist, Flood, Off };
CoolantState currentCoolantState = CoolantState::Undefined;

// GRBL controller states
enum class GRBLStates { Idle, Run, Hold, Jog, Alarm, Door, Check, Home, Sleep };
GRBLStates grblState = GRBLStates::Alarm;

// Co-ordinate system
enum class GRBLCoord { Undefined, MPos, WPos };
GRBLCoord grblCoord = GRBLCoord::Undefined;

// Tool
char currentTool[] = "   ";

// Feed rate
float currentFeedRate;


float currentOvFeedRatePercent;  //Override Percent
float currentOvRapidRatePercent;  //Override Percent

char lastMessage[BufferSize];
unsigned long lastMessageTime=0;
int  alarm_number = 0;			// Alarm message number
unsigned long lastAlarmTime = 0;
int  error_number = 0;			// Error message number
unsigned long lastErrorTime = 0;

struct AxisData
{
	float x;
	float y;
	float z;
};
AxisData currentPosition;

float currentSpindleSpeed;
float currentOvSpindleSpeedPercent; //Override Percent

AxisData currentWCO;

long lastIdleTimeoutCheck = 0;

// --------------
// Jog parameters
// --------------
enum class CNCAxis { X, Y, Z };
CNCAxis currentJogAxis = CNCAxis::X;

float   jogScalings[] = { 1.0, 10.0, 100.0, 10.0 }; // values are mm per encoder revolution. Last value is adjutable
const float MaxJogScaling = 500.0;
uint8_t currentJogScaling = 0;

float   jogRates[] = { 60.0, 90.0, 300.0, 600.0, 900.0, 3000.0 }; // values are "units" (mm or inches) per minute
uint8_t currentJogRate = 2;

int grbl_command_count = 0;
int grbl_last_command_count = 0;
float lastJogCommandPosition;

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

struct MenuParameterItem menuParameters[] = {
	{ParamInMm,  "Units      ",  0,     0.,     0., (void*)&menuVarUnits},
	{ParamFloat, "Jog Spd. XY",  0,     0.,   100., (void*)&menuJogSpeedXY},
	{ParamFloat, "Jog Spd. Z ",  0,     0.,   100., (void*)&menuJogSpeedZ},
	{ParamFloat, "Dummy 1    ",  2,   -20.,   100., (void*)&menuVarDummy1},
	{ParamInt,   "Dummy 2    ",  0,  -999.,  1000., (void*)&menuVarDummy2},
	{ParamOnOff, "Dummy 3    ",  0,     0.,     0., (void*)&menuVarDummy3},
	{ParamYesNo, "Dummy 4    ",  0,     0.,     0., (void*)&menuVarDummy4}
};

Menu menuObject(menuParameters, &StatusLCD, &uiEncoderPosition);


//
// ===============================================
// Main
// ===============================================
 //

 //  ---------- Setup  ----------- //

void setup()
{
	debugSerial.begin(DebugSerialSpeed);   // open serial for debug/status messages

#ifdef UI_ENC_S
	// set Select pin from Rotary Encoder to input
	pinMode(UI_ENC_S, INPUT);      // sets the encoder select digital pin
#endif

#ifdef JOG_ENC_S
	// set Select pin from Rotary Encoder to input
	pinMode(JOG_ENC_S, INPUT);      // sets the encoder select digital pin
#endif

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

	delay(2000);

	// open serial port to G Code senser 
	gsSerial.begin(GSSerialSpeed);

	// open serial port to GRBL
	grblSerial.begin(GRBLSerialSpeed);
	// reset grbl device (ctrl-X) for Universal Gcode Sender
	grblSerial.write(0x18);


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

	serial_io_grbl();
	serial_io_gs();

	if (grbl_command_count != grbl_last_command_count)
	{
		//DEBUG_PRINT("Command Count:");
		//DEBUG_PRINTLN(grbl_command_count);
		grbl_last_command_count = grbl_command_count;
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
	// This is the fast loop
	// ---------------------

	uiEncoderPosition = ReadUIEncoder();


#ifdef UI_ENC_S
	uiEncoderSwitch = ReadUIEncoderSwitch();

	if (uiEncoderSwitch)
	{
	}
#endif

	if (!ReadJogResetButton())
	{
		jogEncoderPosition = ReadJogEncoder();
	}
	else
	{
		ResetJogEncoder();
		jogEncoderPosition = 0;
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
		float jog_move = float(jogEncoderPosition) / JogEncCount * get_jog_scaling();
		if (fabs(jog_move - lastJogCommandPosition) >= 0.001)
		{
			if (grbl_command_count < 3)
			{
				float jogDelta = jog_move - lastJogCommandPosition;
				send_jog_command(jogDelta);
				lastJogCommandPosition = lastJogCommandPosition + jogDelta;
			}
		}


	}
}

void medium_loop()
{
	// This is the start of the medium loop 
	// -----------------------------------------

	char key;

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
}

void slow_loop()
{
	// This is the slow (2Hz) loop pieces
	//----------------------------------------
	switch (slow_loopCounter) {
	case 0:
		slow_loopCounter++;

		delta_ms_slow_loop = millis() - slow_loopTimer;
		slow_loopTimer = millis();

		if (menuMode == MenuModes::Status)
			display_state();
		else
			menuObject.Draw();

		break;

	case 1:
		slow_loopCounter++;

		display_jogscreen();

		break;

	case 2:
		slow_loopCounter++;

		if ((pendantMode == PendantModes::Control) && (millis() - lastStatusRXTime > 1000))
		{
			sendGRBLCommand_NoCount("?");
		}
		break;

	case 3:
		slow_loopCounter++;

		if ((pendantMode == PendantModes::Control) && (millis() - lastStateRXTime > 1000))
		{
			sendGRBLCommand("$G\n");
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
				grbl_command_count = 0;
			}
		}
		else
		{
			lastIdleTimeoutCheck = millis();
		}
	}
	else
	{
		grbl_command_count = 0;
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

float get_jog_step()
{
	float step;
	switch (currentJogScaling)
	{
	case 0:
		step = jogScalings[0];
		break;
	case 1:
		step = jogScalings[1];
		break;
	case 2:
		step = jogScalings[2];
		break;
	case 3:
		step = jogScalings[3];
		break;
	default:
		step = 0.0;
	}

	return step;
}

float get_jog_rate()
{
	//	float   jogRates[] = { 60.0, 90.0, 300.0, 600.0, 900.0, 3000.0 }; // values are "units" (mm or inches) per minute
	return jogRates[currentJogRate];
}

float get_jog_scaling()
{
	return jogScalings[currentJogScaling];
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