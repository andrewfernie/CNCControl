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
		Serial.print(F("<XLCD "));
		Serial.print(VERSION);
		Serial.println(F(" MENU>"));

		Serial.println(F("<:f\tShow free RAM>"));

		Serial.println(F("<:i ms\tSet or get interval in microseconds>"));

		Serial.println(F("<:r\tReset device>"));
		return;
	}
	// Commands handle:
	if (line[1] == 'f') return free_ram();
	if (line[1] == 'i') return setinterval(line);
	if (line[1] == 'r') return resetDevice(0);

	Serial.print(F("Cant parse this command: "));
	Serial.println(line);
}



void free_ram()
{
	Serial.print(F("Free ram: "));
	Serial.println(freeMem());
}


void setinterval(char* line)
{
	char spaceChar[] = " ";
	int ms = atoi(split(line, spaceChar, 1));
	if (ms > 0 || ms == -1) {
		setinterval(ms);
	}
	else {
		Serial.print(F("<Interval: "));
		Serial.print(EEPROMReadInt(EEPROM_INTERVAL));
		Serial.println(F(">"));
	}
}

void setinterval(int ms)
{
	Serial.print(F("<Interval: "));
	Serial.print(ms);
	Serial.println(F(">"));

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
	Serial.println(F("Reset device ... "));
	grblSerial.write(0x18);
	delay(100);
	//asm volatile ("jmp 0x0000");
}
