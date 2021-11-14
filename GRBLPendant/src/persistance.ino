//=========================================================
//Project: GRBL Pendant
//Module:  persistance.ino
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like
//
//=========================================================

// --------------- EEPROM Utilities ----------
//	From:	http://blog.ncode.ca/?p=42
//
#include <EEPROM.h>
#include <stdint.h>

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
{
	uint8_t lowuint8_t = ((p_value >> 0) & 0xFF);
	uint8_t highuint8_t = ((p_value >> 8) & 0xFF);

	EEPROM.write(p_address, lowuint8_t);
	EEPROM.write(p_address + 1, highuint8_t);
}

//This function will read a 2 uint8_t integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
{
	uint8_t lowuint8_t = EEPROM.read(p_address);
	uint8_t highuint8_t = EEPROM.read(p_address + 1);

	return ((lowuint8_t << 0) & 0xFF) + ((highuint8_t << 8) & 0xFF00);
}
