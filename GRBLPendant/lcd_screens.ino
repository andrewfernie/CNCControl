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


void display_state()
{
	
// Display on LCD ...
// lcd screen
// |------------------|
// State  WPos  
// 
// S1 T1          F1000
// MM LIN XY M1


	char tmpStr[LCD_cols];

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
	JogLCD.setCursor(0, 0); // letter, row
	JogLCD.print(tmpStr);

	// ----------
	// Second Row
	JogLCD.setCursor(0, 1); // letter, row
	JogLCD.print(LCD_EMPTY);
	JogLCD.setCursor(0, 1); // letter, row
	JogLCD.print(lastMessage);

	// ---------
	// Third Row
	JogLCD.setCursor(0, 2); //third row

	switch (currentMotionMode)
	{
	case MotionMode::Rapid:
		JogLCD.print("RAP ");
		break;
	case MotionMode::Linear:
		JogLCD.print("LIN ");
		break;
	case MotionMode::CW:
		JogLCD.print("CW  ");
		break;
	case MotionMode::CCW:
		JogLCD.print("CCW ");
		break;
	case MotionMode::Probe_2:
		JogLCD.print("PRB2");
		break;
	case MotionMode::Probe_3:
		JogLCD.print("PRB3");
		break;
	case MotionMode::Probe_4:
		JogLCD.print("PRB4");
		break;
	case MotionMode::Probe_5:
		JogLCD.print("PRB5");
		break;
	default:
		JogLCD.print("--- ");
	}

	// Coordinates ..
	JogLCD.setCursor(7, 2); // letter, row
	sprintf(tmpStr, "%4s", positionCoordSystem);
	JogLCD.print(tmpStr);


	// Feed
	sprintf(tmpStr, "%4.0f", currentFeedRate);
	JogLCD.setCursor((LCD_cols - strlen(tmpStr) - 4), 2);
	JogLCD.print(strcat(tmpStr, "mm/s"));
	
	// ---------
    // Fourth Row
	
	JogLCD.setCursor(0, 3);


	// Plane
	switch (currentPlaneSelect)
	{
	case PlaneSelect::XY:
		JogLCD.print("XY");
		break;
	case PlaneSelect::ZX:
		JogLCD.print("ZX");
		break;
	case PlaneSelect::YZ:
		JogLCD.print("YZ");
		break;
	default:
		JogLCD.print("--");
	}

	JogLCD.print(" ");

	// Tool
	sprintf(tmpStr, "%3.3s", currentTool);
	JogLCD.print(tmpStr);
	JogLCD.print(" ");

	// Program Mode
	switch (currentProgramMode)
	{
	case ProgramMode::Stop:
		strcpy(tmpStr,"Pause");
		break;
	case ProgramMode::Optional:
		strcpy(tmpStr,"Pause");
		break;
	case ProgramMode::End:
		strcpy(tmpStr,"End  ");
		break;
	case ProgramMode::Rewind:
		strcpy(tmpStr,"Rwnd ");
		break;
	default:
		strcpy(tmpStr,"-----");
	}

    JogLCD.setCursor((LCD_cols - strlen(tmpStr)), 3);
    JogLCD.print(tmpStr);
}



void display_jogscreen()
{

	// Display on LCD ...
	// lcd screen
	// |------------------|
	// WPos         555.529
	// 000.000      000.000
	// 
	//

	char tmpStr[LCD_cols];
	int len;

	// ---------
	// First Row

	

	// Pendant Mode
	StatusLCD.setCursor(0, 0);
	switch (pendantMode)
	{
	case PendantModes::Monitor:
		StatusLCD.print("Mntr");
		break;
	case PendantModes::Control:
		StatusLCD.print("Ctrl");
		break;
	default:
		StatusLCD.print("----");
	}

	// Units
	StatusLCD.setCursor(10, 0);
	switch (currentUnitsMode)
	{
	case UnitsMode::Inches:
		StatusLCD.print("in");
		break;
	case UnitsMode::mm:
		StatusLCD.print("mm");
		break;
	default:
		StatusLCD.print("--");
	}

	// Coordinate system
	sprintf(tmpStr, "%4s", positionCoordSystem);
	len = strlen(tmpStr);
	StatusLCD.setCursor((LCD_cols - len), 0);
	StatusLCD.print(tmpStr);
	
	// ---------
	// Second Row

	// X position
	sprintf(tmpStr, "X%8.2f", currentPosition.x);
	len = strlen(tmpStr);
	StatusLCD.setCursor(0, 1);
	StatusLCD.print(tmpStr);

	// Y position
	sprintf(tmpStr, "Y%8.2f", currentPosition.y);
	len = strlen(tmpStr);
	StatusLCD.setCursor((LCD_cols - len), 1);
	StatusLCD.print(tmpStr);


	// ---------
	// Third Row

	// Z position
	sprintf(tmpStr, "Z%8.2f", currentPosition.z);
	len = strlen(tmpStr);
	StatusLCD.setCursor(0, 2);
	StatusLCD.print(tmpStr);

	// ---------
	// Fourth Row

	StatusLCD.setCursor(0, 3);


	// Jog Axis
	switch (currentJogAxis)
	{
	case CNCAxis::X:
		StatusLCD.print("Jog: X");
		break;
	case CNCAxis::Y:
		StatusLCD.print("Jog: Y");
		break;
	case CNCAxis::Z:
		StatusLCD.print("Jog: Z");
		break;
	default:
		StatusLCD.print("-");
	}

	// Jog Step
	sprintf(tmpStr, "%5.1f", jogScalings[currentJogScaling]);
	len = strlen(tmpStr);

	StatusLCD.setCursor(7, 3);
	StatusLCD.print(tmpStr);
	

	// Jog Rate
	sprintf(tmpStr, "%6.1f", jogRates[currentJogRate]);
	len = strlen(tmpStr);
	StatusLCD.setCursor((LCD_cols - len), 3);
	StatusLCD.print(tmpStr);

	
	
}