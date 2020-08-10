//=========================================================
//Project: GRBL Pendant
//Module:  lcd_screens.ino        
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//----------------------------------------------------------

void DisplayState()
{

	// Display on LCD ...
	// lcd screen
	// |------------------|
	// State  WPos  
	// 
	// S1 T1          F1000
	// MM LIN XY M1


	char tmpStr[LCDCols];

	// ---------
	// First Row

	// Grbl state
	switch (grblState)
	{
	case GRBLStates::Idle:
		strcpy(tmpStr, "Idle ");
		break;

	case GRBLStates::Run:
		strcpy(tmpStr, "Run  ");
		break;

	case GRBLStates::Hold:
		strcpy(tmpStr, "Hold ");
		break;

	case GRBLStates::Jog:
		strcpy(tmpStr, "Jog  ");
		break;

	case GRBLStates::Alarm:
		strcpy(tmpStr, "Alarm");
		break;

	case GRBLStates::Door:
		strcpy(tmpStr, "Door ");
		break;

	case GRBLStates::Check:
		strcpy(tmpStr, "Check");
		break;

	case GRBLStates::Home:
		strcpy(tmpStr, "Home ");
		break;

	case GRBLStates::Sleep:
		strcpy(tmpStr, "Sleep");
		break;

	default:
		strcpy(tmpStr, "-----");
		break;
	}

	StatusLCD.setCursor(0, 0); // letter, row
	StatusLCD.print(tmpStr);

	// ----------
	// Second Row
	StatusLCD.setCursor(0, 1); // letter, row
	StatusLCD.print(LCDEmpty);
	StatusLCD.setCursor(0, 1); // letter, row
	if (grblState == GRBLStates::Alarm)
	{
		StatusLCD.print("Alm:");
		StatusLCD.print(alarm_message[alarmNumber]);
	}
	else
	{
		if (millis() - lastErrorTime < 5000)
			StatusLCD.print(errorMessage[errorNumber]);
		else
			StatusLCD.print(lastMessage);
	}

	// erase message if more than 5s old
	if (millis() - lastMessageTime > 5000)
	{
		for (int i = 0; i <= 20; i++)
			lastMessage[i] = '\0';
	}

	// erase message if more than 5s old
	if (millis() - lastErrorTime > 5000)
	{
		errorNumber = 0;
	}

	// ---------
	// Third Row
	StatusLCD.setCursor(0, 2); //third row

	switch (currentMotionMode)
	{
	case MotionMode::Rapid:
		StatusLCD.print("RAP ");
		break;
	case MotionMode::Linear:
		StatusLCD.print("LIN ");
		break;
	case MotionMode::CW:
		StatusLCD.print("CW  ");
		break;
	case MotionMode::CCW:
		StatusLCD.print("CCW ");
		break;
	case MotionMode::Probe_2:
		StatusLCD.print("PRB2");
		break;
	case MotionMode::Probe_3:
		StatusLCD.print("PRB3");
		break;
	case MotionMode::Probe_4:
		StatusLCD.print("PRB4");
		break;
	case MotionMode::Probe_5:
		StatusLCD.print("PRB5");
		break;
	default:
		StatusLCD.print("--- ");
	}


	// Feed
	sprintf(tmpStr, "%4.0f", currentFeedRate);
	StatusLCD.setCursor((LCDCols - strlen(tmpStr) - 4), 2);
	StatusLCD.print(strcat(tmpStr, "mm/s"));

	// ---------
	// Fourth Row

	StatusLCD.setCursor(0, 3);


	// Plane
	switch (currentPlaneSelect)
	{
	case PlaneSelect::XY:
		StatusLCD.print("XY");
		break;
	case PlaneSelect::ZX:
		StatusLCD.print("ZX");
		break;
	case PlaneSelect::YZ:
		StatusLCD.print("YZ");
		break;
	default:
		StatusLCD.print("--");
	}

	StatusLCD.print(" ");

	// Tool
	sprintf(tmpStr, "%3.3s", currentTool);
	StatusLCD.print(tmpStr);
	StatusLCD.print(" ");

	// Program Mode
	switch (currentProgramMode)
	{
	case ProgramMode::Stop:
		strcpy(tmpStr, "Pause");
		break;
	case ProgramMode::Optional:
		strcpy(tmpStr, "Pause");
		break;
	case ProgramMode::End:
		strcpy(tmpStr, "End  ");
		break;
	case ProgramMode::Rewind:
		strcpy(tmpStr, "Rwnd ");
		break;
	default:
		strcpy(tmpStr, "-----");
	}

	StatusLCD.setCursor((LCDCols - strlen(tmpStr)), 3);
	StatusLCD.print(tmpStr);
}



void DisplayJogScreen()
{

	// Display on LCD ...
	// lcd screen
	// |------------------|
	// WPos         555.529
	// 000.000      000.000
	// 
	//

	char tmpStr[LCDCols];
	int len;

	// ---------
	// First Row

	// Pendant Mode
	JogLCD.setCursor(0, 0);
	switch (pendantMode)
	{
	case PendantModes::Monitor:
		JogLCD.print("Mntr");
		break;
	case PendantModes::Control:
		JogLCD.print("Ctrl");
		break;
	default:
		JogLCD.print("----");
	}

	// Units
	JogLCD.setCursor(10, 0);
	switch (currentUnitsMode)
	{
	case UnitsMode::Inches:
		JogLCD.print("in");
		break;
	case UnitsMode::mm:
		JogLCD.print("mm");
		break;
	default:
		JogLCD.print("--");
	}

	// Coordinate system
	JogLCD.setCursor((LCDCols - 4), 0);
	switch (grblCoord)
	{
	case GRBLCoord::MPos:
		JogLCD.print("MPos");
		break;

	case GRBLCoord::WPos:
		JogLCD.print("WPos");
		break;

	default:
		JogLCD.print("----");
	}

	// ---------
	// Second Row

	// X position
	sprintf(tmpStr, "X%8.2f", currentPosition.x);
	len = strlen(tmpStr);
	JogLCD.setCursor(0, 1);
	JogLCD.print(tmpStr);

	// Y position
	sprintf(tmpStr, "Y%8.2f", currentPosition.y);
	len = strlen(tmpStr);
	JogLCD.setCursor((LCDCols - len), 1);
	JogLCD.print(tmpStr);


	// ---------
	// Third Row

	// Z position
	sprintf(tmpStr, "Z%8.2f", currentPosition.z);
	len = strlen(tmpStr);
	JogLCD.setCursor(0, 2);
	JogLCD.print(tmpStr);

	// ---------
	// Fourth Row

	JogLCD.setCursor(0, 3);


	// Jog Axis
	switch (currentJogAxis)
	{
	case CNCAxis::X:
		JogLCD.print("Jog: X");
		break;
	case CNCAxis::Y:
		JogLCD.print("Jog: Y");
		break;
	case CNCAxis::Z:
		JogLCD.print("Jog: Z");
		break;
	default:
		JogLCD.print("-");
	}

	// Jog Step
	sprintf(tmpStr, "%5.1f", getJogSize());
	len = strlen(tmpStr);

	JogLCD.setCursor(7, 3);
	JogLCD.print(tmpStr);

	// Jog Rate
	sprintf(tmpStr, "%6.1f", getJogRate());
	len = strlen(tmpStr);
	JogLCD.setCursor((LCDCols - len), 3);
	JogLCD.print(tmpStr);

}