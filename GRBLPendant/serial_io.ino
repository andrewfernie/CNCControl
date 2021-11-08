//=========================================================
//Project: GRBL Pendant
//Module:  serial_io.ino        
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//----------------------------------------------------------


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
