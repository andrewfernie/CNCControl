//=========================================================
//Project: GRBL Pendant
//Module:  console.ino
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

void ParseCommandLine(char* line)
{
	if (line[1] == '?')
	{
		// Display Menu from this device
		debugSerial.print(F("<GRBL Pendant "));
		debugSerial.print(ProgramVersion);
		debugSerial.println(F(" MENU>"));

		debugSerial.println(F("<:f\tShow free RAM>"));

		debugSerial.println(F("<:i ms\tSet or get interval in microseconds>"));

		debugSerial.println(F("<:r\tReset device>"));
		return;
	}
	// Commands handle:
	if (line[1] == 'f') return free_ram();
	if (line[1] == 'i') return setinterval(line);
	if (line[1] == 'r') return resetDevice(0);

	debugSerial.print(F("Cant parse this command: "));
	debugSerial.println(line);
}



void free_ram()
{
	debugSerial.print(F("Free ram: "));
	debugSerial.println(freeMem());
}


void setinterval(char* line)
{
	char spaceChar[] = " ";
	int ms = atoi(split(line, spaceChar, 1));
	if (ms > 0 || ms == -1) {
		setinterval(ms);
	}
	else {
		debugSerial.print(F("<Interval: "));
		debugSerial.print(EEPROMReadInt(EEPROMInterval));
		debugSerial.println(F(">"));
	}
}

void setinterval(int ms)
{
	debugSerial.print(F("<Interval: "));
	debugSerial.print(ms);
	debugSerial.println(F(">"));

	EEPROMWriteInt(EEPROMInterval, ms);

}

void resetDevice(int n)
{
	debugSerial.println(F("Reset device ... "));
	GrblCommWriteChar(0x18);
	delay(100);
	//asm volatile ("jmp 0x0000");
}
