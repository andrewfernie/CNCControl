//=========================================================
//Project: GRBL Pendant
//Module:  GRBLPendant.h      
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like
//
//=========================================================
#pragma once

#include "config.h"

enum class CNCAxis { X, Y, Z };

struct AxisData
{
	float x;
	float y;
	float z;
};


// ------------
// Pendant Mode
// ------------
enum class PendantModes { Monitor, Control };

// ------------
// Set mode
// ------------
enum class SetMode { Feed, Move, Spindle };

// ------------
// Menu Mode
// ------------
enum class MenuModes { Menu, Status };

// State codes for Grbl 1.1.   Source https://github.com/gnea/grbl/wiki/Grbl-v1.1-Commands
// 
// Motion Mode                 G0, G1, G2, G3, G38.2, G38.3, G38.4, G38.5, G80
enum class MotionMode { Undefined, Rapid, Linear, CW, CCW, Probe_2, Probe_3, Probe_4, Probe_5, Cancel };

// Coordinate System Select    G54, G55, G56, G57, G58, G59
enum class CoordinateSystemSelect { Undefined, WCS1, WCS2, WCS3, WCS4, WCS5, WCS6 };

// Plane Select                G17, G18, G19
enum class PlaneSelect { Undefined, XY, ZX, YZ };

// Distance Mode               G90, G91
enum class DistanceMode { Undefined, Absolute, Incremental };

// Arc IJK Distance Mode       G91.1
enum class ArcDistanceMode { Off, On };

// Feed Rate Mode              G93, G94
enum class FeedRateMode { Undefined, Inverse, Normal };

// Units Mode                  G20, G21
enum class UnitsMode { Undefined, Inches, mm };

// Cutter Radius Compensation  G40
enum class CutterRadiusCompensation { Off, On };

// Tool Length Offset          G43.1, G49
enum class ToolLengthOffsetMode { Undefined, Dynamic, Cancel };

// Program Mode                M0, M1, M2, M30
enum class ProgramMode { Undefined, Stop, Optional, End, Rewind };

// Coolant State               M7, M8, M9
enum class CoolantState { Undefined, Mist, Flood, Off };

// GRBL controller states
enum class GRBLStates { Undefined, Idle, Run, Hold, Jog, Alarm, Door, Check, Home, Sleep };

// Co-ordinate system
enum class GRBLCoord { Undefined, MPos, WPos };

// Spindle State               M3, M4, M5
enum class SpindleState { Undefined, CW, CCW, Off };

void fast_loop();

void medium_loop();

void slow_loop();

void one_second_loop();
char* split(char* string, char* delimiter, int index);
void set_grblState_from_chars(char* tmp);

float getJogRate();

uint8_t setJogRateIndex(uint8_t index);
uint8_t setJogRateDefault();
uint8_t incrementJogRateIndex();

uint8_t decrementJogRateIndex();

float getJogSize();
uint8_t setJogSizeIndex(uint8_t index);

uint8_t setJogSizeDefault();
uint8_t incrementJogSizeIndex();
uint8_t decrementJogSizeIndex();
float getSpindleRPM();
uint8_t setSpindleRPMIndex(uint8_t index);
uint8_t setSpindleRPMDefault();
uint8_t incrementSpindleRPMIndex();

uint8_t decrementSpindleRPMIndex();
uint8_t findClosestSpindleRPMIndex(float rpm);
uint32_t freeMem();

extern int pc;
extern int gr;
extern int pc_chars;
extern int gr_chars;
extern int loopCount;
extern char pcserial[BufferSize];
extern char grserial[BufferSize];
extern char lcdRowString[LCDCols];
extern uint32_t lastLCDOut;

extern PendantModes pendantMode;
extern SetMode currentSetMode;
extern MenuModes menuMode;
extern unsigned long lastCall;
extern MotionMode currentMotionMode;
extern CoordinateSystemSelect currentCoordinateSystemSelect;
extern PlaneSelect currentPlaneSelect;
extern DistanceMode currentDistanceMode;
extern ArcDistanceMode currentArcDistanceMode;
extern FeedRateMode currentFeedRateMode;
extern UnitsMode currentUnitsMode;
extern CutterRadiusCompensation currentCutterRadiusCompensation;
extern ToolLengthOffsetMode currentToolLengthOffsetMode;
extern ProgramMode currentProgramMode;
extern CoolantState currentCoolantState;
extern GRBLStates grblState;
extern GRBLCoord grblCoord;

extern char currentTool[];

extern float currentFeedRate;

extern float spindleSpeed;


extern float currentOvFeedRatePercent;  //Override Percent
extern float currentOvRapidRatePercent;  //Override Percent

extern char lastMessage[];
extern unsigned long lastMessageTime;
extern int  alarmNumber;			// Alarm message number
extern unsigned long lastAlarmTime;
extern int  errorNumber;			// Error message number
extern unsigned long lastErrorTime;

extern AxisData currentPosition;

extern AxisData currentWCO;

extern uint32_t  lastIdleTimeoutCheck;

extern CNCAxis currentJogAxis;

extern float   jogSize[];
extern const uint8_t defaultJogSizeIndex;
extern uint8_t maxJogSizeIndex;
extern uint8_t currentJogSizeIndex;
extern float   adjustableJogSize;
extern uint8_t enableAdjustableJogSize;
extern bool stopJogCommand;
extern float   jogRate[];
extern const uint8_t defaultJogRateIndex;
extern uint8_t maxJogRateIndex;
extern uint8_t currentJogRateIndex;
extern SpindleState currentSpindleState;
extern float currentSpindleSpeed;

extern float   spindleRPM[];
extern const uint8_t defaultSpindleRPMIndex;
extern uint8_t maxSpindleRPMIndex;
extern uint8_t commandSpindleRPMIndex;

extern int grblCommandCount;
extern int grbl_last_command_count;
extern float lastJogCommandPosition;
extern uint8_t menuVarUnits;
extern float   menuJogSpeedXY;
extern float   menuJogSpeedZ;

extern LiquidCrystal_I2C JogLCD;
extern LiquidCrystal_I2C StatusLCD;

extern long lastStatusRXTime;
extern long lastStateRXTime;

#ifdef GRBL_COMM_USB
extern USBHost grblUSB;
extern USBSerial grblUSBSerial;
extern USBDriver* drivers[];
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
extern const char* driver_names[];
extern bool driver_active[];
#endif