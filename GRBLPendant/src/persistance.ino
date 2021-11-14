//=========================================================
//Project: GRBL Pendant
//Module:  persistance.ino
//=========================================================
//
// GRBLPendant CNC control Copyright(C) 2021 Andrew Fernie
//
// This program is free software : you can redistribute it and /
// or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//=========================================================
//
// This project includes code from the  XLCD/SerialSpy
// project by Frank Herrmann.
// https://github.com/xpix/XLCD/tree/master/serialspy
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
