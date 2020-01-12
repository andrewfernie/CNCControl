//=========================================================
//Project: GRBL Pendant
//Module:  console.ino         
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//----------------------------------------------------------

void parse_command_line(char* line)
{
	if (line[1] == '?')
	{
		// Display Menu from this device
		debugSerial.print(F("<GRBL Pendant "));
		debugSerial.print(VERSION);
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
		debugSerial.print(EEPROMReadInt(EEPROM_INTERVAL));
		debugSerial.println(F(">"));
	}
}

void setinterval(int ms)
{
	debugSerial.print(F("<Interval: "));
	debugSerial.print(ms);
	debugSerial.println(F(">"));

	EEPROMWriteInt(EEPROM_INTERVAL, ms);

	if (ms == -1)
	{
		simpleThread_group_stop(group_one);
		return;
	}
	if (ms > 0)
	{
		simpleThread_dynamic_setLoopTime(getPositions, ms);
		simpleThread_dynamic_setLoopTime(getStates, ms);

		simpleThread_group_restart(group_one);
	}
}

void resetDevice(int n)
{
	debugSerial.println(F("Reset device ... "));
	grblSerial.write(0x18);
	delay(100);
	//asm volatile ("jmp 0x0000");
}
