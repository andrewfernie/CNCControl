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

#include <Keypad.h>
#include <Key.h>
#include <EEPROM.h>
#include <simpleThread.h>
#include <Encoder.h>

//
// ===============================================
// Defines
// ===============================================
//

// MODE ------------------------------------------
#define MODE_PROXY   // works as proxy between PC and GRBL
//#define MODE_SPY   // works only as spy on the TX line 
					 // (no buttons, no serial console, no interaction with grbl)

// Normal serial for debug ----------------------------------
#define debugSerial     Serial3
#define DEBUG_SERIAL    115200  

// Serial to GRBL
#define grblSerial      Serial2
#define GRBL_SERIAL     115200 

// G-Code sender. Must be as fast as grbl!
#define gsSerial        Serial
#define GS_SERIAL       115200 

const int BUFFER_SIZE = 100;

// LCD -------------------------------------------
#define LCD_ADDR		   0x27  // I2C LCD Address
#define LCD2_ADDR		   0x23  // I2C LCD Address
//#define LCD_4BIT

#define LCD_cols			20
#define LCD_rows			4

#if(LCD_cols==16)
#define LCD_EMPTY   ("                ")
#else
#define LCD_EMPTY   ("                    ")
#endif

#if defined(LCD_4BIT)
#define LCD_EN          12
#define LCD_RW          11
#define LCD_D4          4
#define LCD_D5          5
#define LCD_D6          6
#define LCD_D7          7
#elif defined(LCD_ADDR)
#define LCD_EN          2
#define LCD_RW          1
#define LCD_RS          0
#define LCD_D4          4
#define LCD_D5          5
#define LCD_D6          6
#define LCD_D7          7
#endif


// UI Rotary Encoder --------------------------------
#define UI_ENC_A				4     // Encoder interrupt pin
#define UI_ENC_B				5     // Encoder second pin
#define UI_ENC_S				6     // Encoder select pin
const int UI_ENC_COUNT = 80;  // encoder count per rotation

#if defined(MODE_PROXY)
   // Rotary Encoder --------------------------------
#define JOG_ENC_A				2     // Encoder interrupt pin
#define JOG_ENC_B				3     // Encoder second pin
//#define JOG_ENC_S				4     // Encoder select pin
const int JOG_ENC_COUNT =	400;  // encoder count per rotation

// EEPROM addresses
#define EEPROM_BUTTONS		100
#define EEPROM_INTERVAL		150
#endif

// ========================== END Defines ========

// only for debugging
#define DEBUG

#define DEBUG_IO

#ifdef DEBUG_IO
#define DEBUG_PRINT(str) debugSerial.print(str)
#define DEBUG_PRINTLN(str) debugSerial.println(str)
#else
#define DEBUG_PRINT(str)
#define DEBUG_PRINTLN(str)
#endif

#define VERSION         0.1

// Makros
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// GRBL Serial connect pins ----------------------
#define GRBL_RX			8
#define GRBL_TX			9 

enum class GRBLStates { Idle, Run, Hold, Jog, Alarm, Door, Check, Home, Sleep };
// State set or get state from machine
GRBLStates grblState = GRBLStates::Alarm;

//
// ===============================================
// Inits
// ===============================================
//


 // Add Threads to refresh status informations from GRBL
#define _sT_cnt  _sT_cnt_3    // count of threads(?, $G)
simpleThread_init(_sT_cnt);   // init threads
simpleThread_new_timebased_dynamic(_sT_P1, _sT_millis, 5000, _sT_start, getPositions);	// get position info (?)
simpleThread_new_timebased_dynamic(_sT_P2, _sT_millis, 5000, _sT_start, getStates);	// get state info ($G) (not supported from UniversalGcodeSender)
simpleThread_new_timebased_dynamic(_sT_P3, _sT_millis, 200, _sT_start, readButtons);	// get button value

// make a group
simpleThread_group_init(group_one, 2) {
	simpleThread_group(getPositions),
		simpleThread_group(getStates)
};

// -------------------------
// All inits for LCD control
// -------------------------
#if defined(LCD_4BIT)
#include <LiquidCrystal.h>   // LCD
LiquidCrystal JogLCD(LCD_EN, LCD_RW, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
#elif defined(LCD_ADDR)
#include <Wire.h>                // I2C Communication
#include <LiquidCrystal_I2C.h>   // LCD over I2C
// Set the pins on the I2C chip used for LCD connections:
//                         addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C StatusLCD(LCD_ADDR, LCD_EN, LCD_RW, LCD_RS, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 3, POSITIVE);  // Set the LCD I2C address
LiquidCrystal_I2C JogLCD(LCD2_ADDR, LCD_EN, LCD_RW, LCD_RS, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 3, POSITIVE);  // Set the LCD I2C address
#endif

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
#endif

#ifdef JOG_ENC_S
bool jogEncoderSwitch = false;
#endif

// LCD Menu
// look in lcd_menu.ino for build menus
// LCDMenu menu(&JogLCD, LCD_cols, LCD_rows);

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
const byte rows = 4; //four rows
const byte cols = 4; //four columns
char keys[rows][cols] = {
  {'1','2','3','4'},
  {'5','6','7','8'},
  {'9','0','A','B'},
  {'C','D','E','F'}
};
byte rowPins[rows] = { 14, 15, 16, 17 }; //connect to the row pinouts of the keypad
byte colPins[cols] = { 20, 21, 22, 23 }; //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

// ------------
// Pendant Mode
// ------------
enum class PendantModes { Monitor, Control };
PendantModes pendantMode = PendantModes::Monitor;

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

// Tool
char currentTool[] = "   ";

// Feed rate
float currentFeedRate;

float currentOvFeedRatePercent;  //Override Percent
float currentOvRapidRatePercent;  //Override Percent

char lastMessage[LCD_cols];

struct AxisData
{
	float x;
	float y;
	float z;
};
AxisData currentPosition;
char     positionCoordSystem[10];

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

//
// ===============================================
// Main
// ===============================================
 //



 //  ---------- Setup  ----------- //

void setup()
{
	debugSerial.begin(DEBUG_SERIAL);   // open serial for debug/status messages

#ifdef UI_ENC_S
// set Select pin from Rotary Encoder to input
	pinMode(UI_ENC_S, INPUT);      // sets the encoder select digital pin
#endif

#ifdef JOG_ENC_S
	// set Select pin from Rotary Encoder to input
	pinMode(JOG_ENC_S, INPUT);      // sets the encoder select digital pin
#endif

   // init threads //
	simpleThread_initSetup(_sT_cnt);
	//simpleThread_dynamic_setLoopTime(getPositions,	EEPROMReadInt(EEPROM_INTERVAL));
	//simpleThread_dynamic_setLoopTime(getStates,		EEPROMReadInt(EEPROM_INTERVAL));

	JogLCD.begin(LCD_cols, LCD_rows);
	StatusLCD.begin(LCD_cols, LCD_rows);

	// This is the serial connect to PC, we get some commands
	// but we can also print some additional information about this module
	// and the parser from Client program will ignore this
	debugSerial.print(F("<GRBLPendant "));
	debugSerial.print(VERSION);
	debugSerial.println(F(">"));
	debugSerial.println(F("<All commands start with a colon ':'>"));
	debugSerial.println(F("<Call help with ':?'>"));


	JogLCD.setCursor(0, 0); // letter, row
	JogLCD.print(F("GRBL Pendant "));
	JogLCD.print(VERSION);
	JogLCD.setCursor(0, 1); // letter, row
	JogLCD.print(F("Connect ... "));
	JogLCD.setCursor(0, 2); // letter, row
	JogLCD.print(F("LCD #1 "));

	StatusLCD.setCursor(0, 0); // letter, row
	StatusLCD.print(F("GRBL Pendant "));
	StatusLCD.print(VERSION);
	StatusLCD.setCursor(0, 1); // letter, row
	StatusLCD.print(F("Connect ... "));	
	StatusLCD.setCursor(0, 2); // letter, row
	StatusLCD.print(F("LCD #2 "));

	delay(2000);

	// open serial port to G Code senser 
	gsSerial.begin(GS_SERIAL);

	// open serial port to GRBL
	grblSerial.begin(GRBL_SERIAL);
	// reset grbl device (ctrl-X) for Universal Gcode Sender
	grblSerial.write(0x18);


	JogLCD.clear();
	StatusLCD.clear();

}//SETUP



//  ---------- Loop ----------- //
int pc = 0;
int gr = 0;
int pc_chars = 0;
int gr_chars = 0;
int loopCount = 0;
char pcserial[BUFFER_SIZE];
char grserial[BUFFER_SIZE];
char lcdRowString[LCD_cols];
uint32_t  lastLCDOut = 0;


void loop()
{
	// Jobs
	// simpleThread_run(_sT_priority);

	serial_io_grbl();
	serial_io_gs();

	if (grbl_command_count != grbl_last_command_count)
	{
		//DEBUG_PRINT("Command Count:");
		//DEBUG_PRINTLN(grbl_command_count);
		grbl_last_command_count = grbl_command_count;
	}

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

	jogEncoderPosition = ReadJogEncoder();

#ifdef JOG_ENC_S
	jogEncoderSwitch = ReadJogEncoderSwitch();

	if (jogEncoderSwitch)
	{
		ResetJogEncoderCount();
		lastJogCommandPosition = 0.0;
	}
#endif

	if (pendantMode == PendantModes::Control)
	{
		float jog_move = float(jogEncoderPosition) / JOG_ENC_COUNT * get_jog_scaling();
		if (fabs(jog_move - lastJogCommandPosition) >= 0.001)
		{
			if (grbl_command_count < 5)
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

		display_state();

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

		DEBUG_PRINTLN(ReadUIEncoder());

		if (ReadUIEncoderSwitch())
			DEBUG_PRINTLN("TRUE");
		else
			DEBUG_PRINTLN("FALSE");


		break;
	}
}

void one_second_loop()
{
	//char tmpStr[80];

	//sprintf(tmpStr, "Current Position: %f, %f, %f\n", currentPosition.x, currentPosition.y, currentPosition.z);
	//debugSerial.print(tmpStr);


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

	char buffer[BUFFER_SIZE];
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