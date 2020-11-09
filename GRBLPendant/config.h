// config.h

#ifndef _CONFIG_h
#define _CONFIG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <USBHost_t36.h>

// Define one of the following depending on which model of Teensy is being used
// Changes pin defines and availability of USB Host mode (not supported by Teensy 3.2)
//#define TEENSY32
#define TEENSY41

// Normal serial for debug ----------------------------------
#define debugSerial     Serial3
constexpr auto DebugSerialSpeed = 115200  ;

// G-Code sender. Must be as fast as grbl!
#define gsSerial        Serial
constexpr auto GSSerialSpeed = 115200 ;

const int BufferSize = 100;

#ifdef TEENSY41
#define  GRBL_COMM_USB
const uint32_t USBBAUD = 115200;
const uint32_t USBFORMAT = USBHOST_SERIAL_8N1;
//#define USB_COMM_DEBUG
#else
#define  GRBL_COMM_UART
// Serial to GRBL
#define grblSerial      Serial2
constexpr auto GRBLSerialSpeed = 115200; 
#endif

// LCD -------------------------------------------
constexpr auto StatusLCDAddress = 0x26;  // I2C LCD Address
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


// Debug Pins --------------------------------
#ifdef TEENSY32
constexpr auto DEBUG_BROWN  = 24;
constexpr auto DEBUG_RED    = 25;
constexpr auto DEBUG_ORANGE = 26;
constexpr auto DEBUG_YELLOW = 27;
#else
constexpr auto DEBUG_BROWN = 29;
constexpr auto DEBUG_RED = 30;
constexpr auto DEBUG_ORANGE = 31;
constexpr auto DEBUG_YELLOW = 32; 
#endif

// EEPROM addresses
constexpr auto EEPROMButtons = 100;
constexpr auto EEPROMInterval = 150;

// only for debugging
#define DEBUG

#define DEBUG_IO

#ifdef DEBUG_IO
#define DEBUG_PRINT(str) debugSerial.print(str)
#define DEBUG_PRINTLN(str) debugSerial.println(str)
#define DEBUG_DIGITALWRITE(pin,value) digitalWrite(pin,value);
#define DEBUG_DIGITALWRITE_HIGH(pin) digitalWrite(pin,HIGH);
#define DEBUG_DIGITALWRITE_LOW(pin) digitalWrite(pin,LOW);
#else
#define DEBUG_PRINT(str)
#define DEBUG_PRINTLN(str)
#define DEBUG_DIGITALWRITE(pin,value)
#define DEBUG_DIGITALWRITE_HIGH(pin)
#define DEBUG_DIGITALWRITE_LOW(pin)
#endif

// Macros
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#endif

