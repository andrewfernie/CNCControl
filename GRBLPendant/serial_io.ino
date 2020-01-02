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


// Get data from GRBL ==> PC
void serial_io_grbl()
{
	while (grblSerial.available())
	{
		char c = grblSerial.read();

		// wait for a complete line 
		// and parse it
		if (c == '\n') {

			DEBUG_PRINTLN(grserial);
			parseGrblLine(grserial);

			gr = 0;
			memset(&grserial[0], 0, sizeof(grserial));
			grserial[0] = '\0';
		}
		else {
			if (gr < BUFFER_SIZE)
				grserial[gr++] = c;
		}
	}
}

// Get data from PC ==> GRBL
void serial_io_gs()
{
	while (gsSerial.available())
	{
		char c = gsSerial.read();


		if (c == 0x18)
		{
			// If a ctrl-x is received from UGS, just send it on. 
			grblSerial.write(c);

			// flush the buffer
			pc = 0;
			memset(&pcserial[0], 0, sizeof(pcserial));
			pcserial[0] = '\0';
		}
		else if (c == '?')
		{
			// If a ctrl-x is received from UGS, just send it on. 
			grblSerial.write(c);
		}
		else if (c == '\n') 
		{
			// wait for a complete line
			// and parse it
			parsePCCommand(pcserial);
		
			pc = 0;
			memset(&pcserial[0], 0, sizeof(pcserial));
			pcserial[0] = '\0';
		}
		else 
		{
			if (pc < BUFFER_SIZE) {
				pcserial[pc++] = c;
			}
		}



	}
}

void sendGRBLCommand_NoCount(char* command)
{
	grblSerial.print(command);
}
void sendGRBLCommand(char* command)
{
	sendGRBLCommand_NoCount(command);
	grbl_command_count++;
}
void sendGRBLCommand_NoCount(const char command[])
{
	grblSerial.print(command);
}
void sendGRBLCommand(const char command[])
{
	sendGRBLCommand_NoCount(command);
	grbl_command_count++;
}

// Analyze every command (from PC => Xlcd) and choose an action
void parsePCCommand(char* line)
{
	// All commands with an ':' at start can control GRBLPendant
	// dont send them to grbl
	if (line[0] == ':')
	{
		parse_command_line(line);
	}
	else 
	{
		sendGRBLCommand(line);
		sendGRBLCommand("\n");
	}
}

// Analyze every line and choose an action
void parseGrblLine(char* line_in)
{
	char line[BUFFER_SIZE];

	strcpy(line, line_in);

	char* c2 = strrchr(line, '\r');
	*c2 = '\0';

	if (line[0] == '<')
	{
		parse_status_line(line);
		gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
		gsSerial.print("\n");
	}
	else if (line[0] == '[')
	{
		parse_state_line(line);

		if (pendantMode != PendantModes::Control)
		{
			gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
			gsSerial.print("\n");
		}
	}
	else if (strncmp(line, "ok", 2) == 0)
	{
		if (pendantMode == PendantModes::Control)
		{
			grbl_command_count--;
		}
		else
		{
			gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
			gsSerial.print("\n");
		}
	}
	else if (strncmp(line, "error", 5) == 0)
	{
		if (pendantMode != PendantModes::Control)
		{
			gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
			gsSerial.print("\n");
		}
	}
	else if (strncmp(line, "ALARM", 5) == 0)
	{
		if (pendantMode != PendantModes::Control)
		{
			gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
			gsSerial.print("\n");
		}
	}
	else
	{
		gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
		gsSerial.print("\n");
	}


}


//// If the pendant is in monitor mode send everything on to the sender.
//// If not, dont send data from $G or empty strings as UGS objects.
//if ((pendantMode == PendantModes::Monitor) || ((grserial[0] != '[') && (grserial[0] != 0)))
//{
//	gsSerial.print(grserial);     // send line from the grbl controller to the g-code sender
//	gsSerial.print(c);
//}
void parse_status_line(char* line_in)
{
	// Typical status lines formats
	//
	// <Idle | WPos:73.000, 0.000, -20.000 | FS : 0, 0>
	// <Idle | WPos:73.000, 0.000, -20.000 | FS : 0, 0 | WCO : -75.000, 0.000, -358.000>
	// <Idle | WPos:73.000, 0.000, -20.000 | FS : 0, 0 | Ov : 100, 100, 100>

	char delim[] = "<,:|";
	char* temp;
	float tmpFloat;
	int   n;
	char line[BUFFER_SIZE];

	lastStatusRXTime = millis();

	strcpy(line, line_in);	// work with local copy as strtok modifies the source

	// First Line
	// State ..
	temp = strtok(line, delim);
	set_grblState_from_chars(temp);


	// Coordinates ..
	temp = strtok(NULL, delim);
	strncpy(positionCoordSystem, temp, sizeof(positionCoordSystem) - 1);


	// X Machine position ...
	temp = strtok(NULL, delim);
	n = sscanf(temp, "%f", &tmpFloat);

	if (n == 1)
	{
		currentPosition.x = tmpFloat;
	}

	// Y Machine position ...
	temp = strtok(NULL, delim);
	n = sscanf(temp, "%f", &tmpFloat);

	if (n == 1)
	{
		currentPosition.y = tmpFloat;
	}

	// Z Machine position ...
	temp = strtok(NULL, delim);
	n = sscanf(temp, "%f", &tmpFloat);

	if (n == 1)
	{
		currentPosition.z = tmpFloat;
	}

	temp = strtok(NULL, delim);
	while (temp != NULL)
	{
		if (strcmp(temp, "F") == 0)
		{
			if (sscanf(temp, "%f", &tmpFloat) == 1)
			{
				currentFeedRate = tmpFloat;
			}

		}
		else if (strcmp(temp, "FS") == 0)
		{
			temp = strtok(NULL, delim);
			if (sscanf(temp, "%f", &tmpFloat) == 1)
			{
				currentFeedRate = tmpFloat;
			}

			temp = strtok(NULL, delim);
			if (sscanf(temp, "%f", &tmpFloat) == 1)
			{
				currentSpindleSpeed = tmpFloat;
			}
		}
		else if (strcmp(temp, "WCO") == 0)
		{
			temp = strtok(NULL, delim);
			if (sscanf(temp, "%f", &tmpFloat) == 1)
			{
				currentWCO.x = tmpFloat;
			}

			temp = strtok(NULL, delim);
			if (sscanf(temp, "%f", &tmpFloat) == 1)
			{
				currentWCO.y = tmpFloat;
			}

			temp = strtok(NULL, delim);
			if (sscanf(temp, "%f", &tmpFloat) == 1)
			{
				currentWCO.z = tmpFloat;
			}
		}
		else if (strcmp(temp, "Ov") == 0)
		{
			temp = strtok(NULL, delim);
			if (sscanf(temp, "%f", &tmpFloat) == 1)
			{
				currentOvFeedRatePercent = tmpFloat;
			}

			temp = strtok(NULL, delim);
			if (sscanf(temp, "%f", &tmpFloat) == 1)
			{
				currentOvRapidRatePercent = tmpFloat;
			}

			temp = strtok(NULL, delim);
			if (sscanf(temp, "%f", &tmpFloat) == 1)
			{
				currentOvSpindleSpeedPercent = tmpFloat;
			}
		}
		temp = strtok(NULL, delim);
	}

	lastCall = millis();
}

// send every second the command $G
void parse_state_line(char* stateLine)
{
	// Typical state line
	// [GC:G0 G54 G17 G21 G90 G94 M5 M9 T0 F0 S0]
	//


	char delim[] = "[ ]:";

	//             mm                   TNr Feed
	// G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 F500.000

	char* thisToken;

	char buffer[BUFFER_SIZE];

	lastStateRXTime = millis();

	strcpy(buffer, stateLine);

	// init and create first cut
	thisToken = strtok(buffer, delim);

	while (thisToken != NULL)
	{
		if (thisToken[0] == 'G')
		{
			// Motion Mode
			if (strcmp(thisToken, "G0") == 0)
				currentMotionMode = MotionMode::Rapid;
			else if (strcmp(thisToken, "G1") == 0)
				currentMotionMode = MotionMode::Linear;
			else if (strcmp(thisToken, "G2") == 0)
				currentMotionMode = MotionMode::CW;
			else if (strcmp(thisToken, "G3") == 0)
				currentMotionMode = MotionMode::CCW;
			else if (strcmp(thisToken, "G38.2") == 0)
				currentMotionMode = MotionMode::Probe_2;
			else if (strcmp(thisToken, "G38.3") == 0)
				currentMotionMode = MotionMode::Probe_3;
			else if (strcmp(thisToken, "G38.4") == 0)
				currentMotionMode = MotionMode::Probe_4;
			else if (strcmp(thisToken, "G38.5") == 0)
				currentMotionMode = MotionMode::Probe_5;
			else if (strcmp(thisToken, "G80") == 0)
				currentMotionMode = MotionMode::Cancel;

			// Plane Select
			else if (strcmp(thisToken, "G17") == 0)
				currentPlaneSelect = PlaneSelect::XY;
			else if (strcmp(thisToken, "G18") == 0)
				currentPlaneSelect = PlaneSelect::ZX;
			else if (strcmp(thisToken, "G19") == 0)
				currentPlaneSelect = PlaneSelect::YZ;

			// Units Mode
			else if (strcmp(thisToken, "G20") == 0)
				currentUnitsMode = UnitsMode::Inches;
			else if (strcmp(thisToken, "G21") == 0)
				currentUnitsMode = UnitsMode::mm;

			// Coordinate System Select    
			else if (strcmp(thisToken, "G54") == 0)
				currentCoordinateSystemSelect = CoordinateSystemSelect::WCS1;
			else if (strcmp(thisToken, "G55") == 0)
				currentCoordinateSystemSelect = CoordinateSystemSelect::WCS2;
			else if (strcmp(thisToken, "G56") == 0)
				currentCoordinateSystemSelect = CoordinateSystemSelect::WCS3;
			else if (strcmp(thisToken, "G57") == 0)
				currentCoordinateSystemSelect = CoordinateSystemSelect::WCS4;
			else if (strcmp(thisToken, "G58") == 0)
				currentCoordinateSystemSelect = CoordinateSystemSelect::WCS5;
			else if (strcmp(thisToken, "G59") == 0)
				currentCoordinateSystemSelect = CoordinateSystemSelect::WCS6;

			// Distance Mode
			else if (strcmp(thisToken, "G90") == 0)
				currentDistanceMode = DistanceMode::Absolute;
			else if (strcmp(thisToken, "G91") == 0)
				currentDistanceMode = DistanceMode::Incremental;

			// Arc Distance Mode
			// Doesn't seem to make sense - need to look into this one in more detail
			else if (strcmp(thisToken, "G91.1") == 0)
				currentArcDistanceMode = ArcDistanceMode::On;

			// FeedRate Mode
			else if (strcmp(thisToken, "G93") == 0)
				currentFeedRateMode = FeedRateMode::Inverse;
			else if (strcmp(thisToken, "G94") == 0)
				currentFeedRateMode = FeedRateMode::Normal;

			// Cutter Radius Compensation
			else if (strcmp(thisToken, "G40") == 0)
				currentCutterRadiusCompensation = CutterRadiusCompensation::On;


			// Tool Length Offset          G43.1, G49
			else if (strcmp(thisToken, "G43.1") == 0)
				currentToolLengthOffsetMode = ToolLengthOffsetMode::Dynamic;
			else if (strcmp(thisToken, "G49") == 0)
				currentToolLengthOffsetMode = ToolLengthOffsetMode::Cancel;

		}
		else if (thisToken[0] == 'M')
		{
			// Program Mode                M0, M1, M2, M30
			if (strcmp(thisToken, "M0") == 0)
				currentProgramMode = ProgramMode::Stop;
			else if (strcmp(thisToken, "M1") == 0)
				currentProgramMode = ProgramMode::Optional;
			else if (strcmp(thisToken, "M2") == 0)
				currentProgramMode = ProgramMode::End;
			else if (strcmp(thisToken, "M30") == 0)
				currentProgramMode = ProgramMode::Rewind;

			// Spindle
			else if (strcmp(thisToken, "M3") == 0)
				currentSpindleState = SpindleState::CW;
			else if (strcmp(thisToken, "M4") == 0)
				currentSpindleState = SpindleState::CCW;
			else if (strcmp(thisToken, "M5") == 0)
				currentSpindleState = SpindleState::Off;

			// Coolant
			else if (strcmp(thisToken, "M7") == 0)
				currentCoolantState = CoolantState::Mist;
			else if (strcmp(thisToken, "M8") == 0)
				currentCoolantState = CoolantState::Flood;
			else if (strcmp(thisToken, "M9") == 0)
				currentCoolantState = CoolantState::Off;
		}
		else if (thisToken[0] == 'T')
		{
			strcpy(currentTool, thisToken);
		}
		else if (thisToken[0] == 'F')
		{
			float tmpFloat;

			if (sscanf(thisToken, "%f", &tmpFloat) == 1)
			{
				currentFeedRate = tmpFloat;
			}
		}

		// next one
		thisToken = strtok(NULL, delim);
	}
}

void send_jog_command(float displacement)
{
	char commandStr[30];
	char tmpStr[30];

	strcpy(commandStr, "$J=G21G91");

	switch (currentJogAxis)
	{
	case CNCAxis::X:
		strcat(commandStr, "X");
		break;
	case CNCAxis::Y:
		strcat(commandStr, "Y");
		break;
	case CNCAxis::Z:
		strcat(commandStr, "Z");
		break;
	}

	sprintf(tmpStr, "%f", displacement);
	strcat(commandStr, tmpStr);

	sprintf(tmpStr, "F%d\n", int(get_jog_rate()));
	strcat(commandStr, tmpStr);

	sendGRBLCommand(commandStr);

//	DEBUG_PRINTLN(commandStr);
}

void spindle_on(int speed)
{
	char commandStr[30];
	char tmpStr[30];

	strcpy(commandStr, "M3S");

	sprintf(tmpStr, "%d\n", speed);
	strcat(commandStr, tmpStr);

	sendGRBLCommand(commandStr);
}

void spindle_off()
{
	sendGRBLCommand("M5\n");
}
