// config.h

#ifndef _CONFIG_h
#define _CONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// Normal serial for debug ----------------------------------
#define debugSerial     Serial3
constexpr auto DebugSerialSpeed = 115200  ;

// Serial to GRBL
#define grblSerial      Serial2
constexpr auto GRBLSerialSpeed = 115200 ;

// G-Code sender. Must be as fast as grbl!
#define gsSerial        Serial
constexpr auto GSSerialSpeed = 115200 ;

const int BufferSize = 100;

// LCD -------------------------------------------
constexpr auto StatusLCDAddress = 0x23;  // I2C LCD Address
constexpr auto JogLCDAddress = 0x27;  // I2C LCD Address

constexpr auto LCDCols = 20;
constexpr auto LCDRows = 4;

#if(LCDCols==16)
constexpr auto LCDEmpty("                ");
#else
constexpr auto LCDEmpty = ("                    ");
#endif

// LCD controller pins
// Note that as we are using an I2C controller these are the pins
// on the controller, not the pins on the Teensy that is running this
// code.
constexpr auto LCD_EN = 2;
constexpr auto LCD_RW = 1;
constexpr auto LCD_RS = 0;
constexpr auto LCD_D4 = 4;
constexpr auto LCD_D5 = 5;
constexpr auto LCD_D6 = 6;
constexpr auto LCD_D7 = 7;


// UI Rotary Encoder --------------------------------
#define UI_ENC_A				4     // Encoder interrupt pin
#define UI_ENC_B				5     // Encoder second pin
#define UI_ENC_S				6     // Encoder select pin
const int UI_ENC_COUNT = 80;  // encoder count per rotation

// Rotary Encoder --------------------------------
#define JOG_ENC_A				2     // Encoder interrupt pin
#define JOG_ENC_B				3     // Encoder second pin
//#define JOG_ENC_S				4     // Encoder select pin
constexpr auto JogEncCount = 400;  // encoder count per rotation

constexpr auto JogResetPin = 11;


// EEPROM addresses
constexpr auto EEPROMButtons = 100;
constexpr auto EEPROMInterval = 150;

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

// Macros
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#endif

