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

		gsSerial.print(c);

		gr_chars++;

		// wait for a complete line 
		// and parse it
		if (c == '\n') {
			parseGrblLine(grserial);

			gr = 0;
			memset(&grserial[0], 0, sizeof(grserial));
			grserial[0] = '\0';
		}
		else {
			if (gr < BUFFER_SIZE)
				grserial[gr++] = c;
		}

		//// dont send data from $G to Serial, 
		//// cuz UGS don't understand this
		//// dont send data if string empty
		//if(grserial[0] != '['){
		//   Serial.print(c);
		//}

	}
}

// Get data from PC ==> GRBL
void serial_io_gs()
{
	while (gsSerial.available())
	{
		char c = gsSerial.read();

		grblSerial.print(c);

		pc_chars++;


		//   // wait for a complete line
		//   // and parse it
		//   if(c == '\n'){
		//      parsePCCommand(pcserial);
		//      pc = 0;
		//      memset(&pcserial[0], 0, sizeof(pcserial));
		//      pcserial[0] = '\0';
		//   } else {
		//      // if to big ...
		//      if(pc < BUFFER_SIZE){
		//      	pcserial[pc++] = c;
		//      }

		//   }

		//   // dont send serial commands (:char) to grbl
		//   if(pcserial[0] != ':'){
		   //    grblSerial.print(c);
		//   }

	}
}


// Analyze every command (from PC => Xlcd) and choose an action
void parsePCCommand(char* line)
{
	char* c2 = strrchr(line, '\r');
	*c2 = ' ';

	// All commands with an ':' at start can control XLCD 
	if (line[0] == ':')  parse_command_line(line);
}

// Analyze every line and choose an action
void parseGrblLine(char* line)
{
	char* c2 = strrchr(line, '\r');
	*c2 = ' ';
	//	Serial.println(line);

	if (line[0] == '<')
	{
		parse_status_line(line);
	}
	else if (line[0] == '[')
	{
		parse_state_line(line);
	}

}


void parse_status_line(char* line)
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

	lastStatusRXTime = millis();

	// First Line
	// State ..
	temp = split(line, delim, 0);
	set_grblState_from_chars(temp);


	// Coordinates ..
	temp = split(line, delim, 1);
	strncpy(positionCoordSystem, temp, sizeof(positionCoordSystem) - 1);


	// X Machine position ...
	temp = split(line, delim, 2);
	n = sscanf(temp, "%f", &tmpFloat);

	if (n == 1)
	{
		currentPosition.x = tmpFloat;
	}

	// Y Machine position ...
	temp = split(line, delim, 3);
	n = sscanf(temp, "%f", &tmpFloat);

	if (n == 1)
	{
		currentPosition.y = tmpFloat;
	}

	// Z Machine position ...
	temp = split(line, delim, 4);
	n = sscanf(temp, "%f", &tmpFloat);

	if (n == 1)
	{
		currentPosition.z = tmpFloat;
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
			strcpy(currentFeedRate, &(thisToken[1]));
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
	
	sprintf(tmpStr, "%d", int(displacement * get_jog_step()));
	strcat(commandStr, tmpStr);

	sprintf(tmpStr, "F%d", int(get_jog_rate()));
	strcat(commandStr, tmpStr);

	grblSerial.println(commandStr);
	Serial.println(commandStr);
}

void spindle_on(int speed)
{
	char commandStr[30];
	char tmpStr[30];

	strcpy(commandStr, "M3S");
	
	sprintf(tmpStr, "%d", speed);
	strcat(commandStr, tmpStr);

	grblSerial.println(commandStr);
}

void spindle_off()
{
	grblSerial.println("M5");
}
