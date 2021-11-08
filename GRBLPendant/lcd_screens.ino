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
	// State           MMod
	// Sp. 14000   Feedmm/s
	// XY Tl#         PMode
	// ---Alarm Message----

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

	switch (currentMotionMode)
	{
	case MotionMode::Rapid:
		strcpy(tmpStr, "RAP ");
		break;
	case MotionMode::Linear:
		strcpy(tmpStr, "LIN ");
		break;
	case MotionMode::CW:
		strcpy(tmpStr, "CW  ");
		break;
	case MotionMode::CCW:
		strcpy(tmpStr, "CCW ");
		break;
	case MotionMode::Probe_2:
		strcpy(tmpStr, "PRB2");
		break;
	case MotionMode::Probe_3:
		strcpy(tmpStr, "PRB3");
		break;
	case MotionMode::Probe_4:
		strcpy(tmpStr, "PRB4");
		break;
	case MotionMode::Probe_5:
		strcpy(tmpStr, "PRB5");
		break;
	default:
		strcpy(tmpStr, "----");
	}

	StatusLCD.setCursor((LCDCols - strlen(tmpStr)), 0);
	StatusLCD.print(tmpStr);

	// ----------
	// Second Row
	StatusLCD.setCursor(0, 1); //third row


	//sprintf(tmpStr, "Sp.%6d", int(getSpindleRPM()));
	switch (currentSpindleState)
	{
	case SpindleState::Off:
		sprintf(tmpStr, "Sp.%6d", int(getSpindleRPM()));
		break;

	case SpindleState::CW:
		sprintf(tmpStr, "Sp.%6d", int(getSpindleRPM()));
		break;

	case SpindleState::CCW:
		sprintf(tmpStr, "Sp.%6d", int(getSpindleRPM()));
		break;

	case SpindleState::Undefined:
		sprintf(tmpStr, "Sp.%6d", int(getSpindleRPM()));
		break;

	default:
		sprintf(tmpStr, "---------");
		break;
	}
	

	StatusLCD.print(tmpStr);

	// Feed
	sprintf(tmpStr, "%4.0f", currentFeedRate);
	StatusLCD.setCursor((LCDCols - strlen(tmpStr) - 4), 1);
	StatusLCD.print(strcat(tmpStr, "mm/s"));


	// ---------
	// Third Row
	StatusLCD.setCursor(0, 2); // letter, row

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

	StatusLCD.setCursor((LCDCols - strlen(tmpStr)), 2);
	StatusLCD.print(tmpStr);

	// ---------
	// Fourth Row

	StatusLCD.setCursor(0, 3); // letter, row
	StatusLCD.print(LCDEmpty);
	StatusLCD.setCursor(0, 3); // letter, row
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
}



void DisplayJogScreen()
{

	// Display on LCD ...
	// lcd screen
	// |------------------|
	// Mntr     mm     WPos 
	// X00000.00  Y00000.00
	// Z00000.00    AdjustV
	// Jog: X 100.0    1000 

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
	JogLCD.setCursor(0, 1);
	JogLCD.print(tmpStr);

	// Y position
	sprintf(tmpStr, "Y%8.2f", currentPosition.y);
	JogLCD.setCursor((LCDCols - strlen(tmpStr)), 1);
	JogLCD.print(tmpStr);


	// ---------
	// Third Row

	// Z position
	sprintf(tmpStr, "Z%8.2f", currentPosition.z);
	len = strlen(tmpStr);
	JogLCD.setCursor(0, 2);
	JogLCD.print(tmpStr);
	
	// SetMode
	switch (currentSetMode)
	{
	case SetMode::Feed:
		strcpy(tmpStr, "   Feed");
		break;
	case SetMode::Move:
		strcpy(tmpStr, "   Move");
		break;
	case SetMode::Spindle:
		strcpy(tmpStr, "Spindle");
		break;
	default:
		strcpy(tmpStr, "-------");
	}
	JogLCD.setCursor((LCDCols - strlen(tmpStr)), 2);
	JogLCD.print(tmpStr);


	// GRBL command counter - for debug only, but useful when debugging communication problems.
//#ifdef DEBUG
////	sprintf(tmpStr, "%2.2d", grblCommandCount);
//	sprintf(tmpStr, "%4.3ld", uiEncoderPosition);
//	len = strlen(tmpStr);
//	JogLCD.setCursor((LCDCols - len), 2);
//	JogLCD.print(tmpStr);
//#endif

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
		JogLCD.print("------");
	}

	// Jog Step
	sprintf(tmpStr, "%5.1f", getJogSize());
	len = strlen(tmpStr);

	JogLCD.setCursor(7, 3);
	JogLCD.print(tmpStr);

	// Jog Rate
	sprintf(tmpStr, "%4d", int(getJogRate()));
	len = strlen(tmpStr);
	JogLCD.setCursor((LCDCols - len), 3);
	JogLCD.print(tmpStr);

}