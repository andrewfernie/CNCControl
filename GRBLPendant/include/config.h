// config.h
#pragma once
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
// Select one of the following for communication with GRBL
#define GRBL_COMM_UART
// #define GRBL_COMM_USB  // Not supported by Teensy 3.2

#ifdef GRBL_COMM_USB
#include <USBHost_t36.h>
#endif // GRBL_COMM_USB

// Define one of the following depending on which model of Teensy is being used
// Changes pin defines and availability of USB Host mode (not supported by Teensy 3.2)
// #define TEENSY32
#define TEENSY41

// Normal serial for debug ----------------------------------
#define debugSerial Serial
constexpr auto DebugSerialSpeed = 115200;

// #define SENDER_COMMS
#ifdef SENDER_COMMS
// G-Code sender. Must be as fast as grbl!
#define gsSerial Serial
constexpr auto GSSerialSpeed = 115200;
#endif

const int BufferSize = 200;

#ifdef TEENSY41
#ifdef GRBL_COMM_UART
// Serial to GRBL
#define grblSerial Serial2
constexpr auto GRBLSerialSpeed = 115200;
#else
#define GRBL_COMM_USB
const uint32_t USBBAUD = 115200;
const uint32_t USBFORMAT = USBHOST_SERIAL_8N1;
// #define USB_COMM_DEBUG
#endif
#else
#define GRBL_COMM_UART
// Serial to GRBL
#define grblSerial Serial2
constexpr auto GRBLSerialSpeed = 115200;
#endif

// Command parameters

constexpr auto SpindleMaxSpeed = 24000; // Max speed for spindle when turned on to 100%

constexpr auto SpindleDefaultSpeedPercent = 50; // Default spindle speed as percentage of SpindleMaxSpeed

// LCD -------------------------------------------
constexpr auto StatusLCDAddress = 0x26; // I2C LCD Address
constexpr auto JogLCDAddress = 0x27;    // I2C LCD Address

constexpr auto LCDCols = 20;
constexpr auto LCDRows = 4;

#if (LCDCols == 16)
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
#define UI_ENC_A 4           // Encoder interrupt pin
#define UI_ENC_B 5           // Encoder second pin
#define UI_ENC_S 6           // Encoder select pin
const int UI_ENC_COUNT = 80; // encoder count per rotation

// Rotary Encoder --------------------------------
#define JOG_ENC_A 2 // Encoder interrupt pin
#define JOG_ENC_B 3 // Encoder second pin
// #define JOG_ENC_S				4     // Encoder select pin
constexpr auto JogEncCount = 400; // encoder count per rotation

constexpr auto JogResetPin = 11;



// EEPROM addresses
constexpr auto EEPROMButtons = 100;
constexpr auto EEPROMInterval = 150;

