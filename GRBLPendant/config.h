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
#define DEBUG_SERIAL    115200  

// Serial to GRBL
#define grblSerial      Serial2
#define GRBL_SERIAL     115200 

// G-Code sender. Must be as fast as grbl!
#define gsSerial        Serial
#define GS_SERIAL       115200 

const int BUFFER_SIZE = 100;

// LCD -------------------------------------------
#define STATUS_LCD_ADDR		   0x23  // I2C LCD Address
#define JOG_LCD_ADDR		   0x27  // I2C LCD Address

const int LCD_cols = 20;
const int LCD_rows = 4;

#if(LCD_cols==16)
#define LCD_EMPTY   ("                ")
#else
#define LCD_EMPTY   ("                    ")
#endif

// LCD controller pins
// Note that as we are using an I2C controller these are the pins
// on the controller, not the pins on the Teensy that is running this
// code.
#define LCD_EN          2
#define LCD_RW          1
#define LCD_RS          0
#define LCD_D4          4
#define LCD_D5          5
#define LCD_D6          6
#define LCD_D7          7


// UI Rotary Encoder --------------------------------
#define UI_ENC_A				4     // Encoder interrupt pin
#define UI_ENC_B				5     // Encoder second pin
#define UI_ENC_S				6     // Encoder select pin
const int UI_ENC_COUNT = 80;  // encoder count per rotation

// Rotary Encoder --------------------------------
#define JOG_ENC_A				2     // Encoder interrupt pin
#define JOG_ENC_B				3     // Encoder second pin
//#define JOG_ENC_S				4     // Encoder select pin
const int JOG_ENC_COUNT = 400;  // encoder count per rotation

// EEPROM addresses
#define EEPROM_BUTTONS		100
#define EEPROM_INTERVAL		150

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

