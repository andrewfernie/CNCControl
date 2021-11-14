//=========================================================
//Project: GRBL Pendant
//Module:  serial_io.ino
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

// Get data from PC ==> GRBL
void SerialIOGS()
{
	while (gsSerial.available())
	{
		char c = gsSerial.read();

		if (c == 0x18)   //  ctrl-x (Soft-Reset), just send it on. 
		{
			GrblCommWriteChar(c);

			// flush the buffer
			pc = 0;
			memset(&pcserial[0], 0, sizeof(pcserial));
			pcserial[0] = '\0';
		}
		else if (c == '?')           //Status Report Query, just send it on. 
		{
			GrblCommWriteChar(c);
		}
		else if (c == '~')           //Cycle Start / Resume, just send it on. 
		{
			GrblCommWriteChar(c);
		}
		else if (c == '!')           //Feed Hold, just send it on. 
		{
			GrblCommWriteChar(c);
		}
		else if (c == '\n')
		{
			// wait for a complete line
			// and parse it
			ParsePCCommand(pcserial);

			pc = 0;
			memset(&pcserial[0], 0, sizeof(pcserial));
			pcserial[0] = '\0';
		}
		else
		{
			if (pc < BufferSize) {
				pcserial[pc++] = c;
			}
		}

	}
}


// Analyze every command (from PC => pendant) and choose an action
void ParsePCCommand(char* line)
{
	// All commands with an ':' at start can control GRBLPendant
	// dont send them to grbl
	if (line[0] == ':')
	{
		ParseCommandLine(line);
	}
	else
	{
		SendGRBLCommand_NoCount(line);  // count will be handled by the newline
		SendGRBLCommand("\n");
	}
}
