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
// State  WPos  555.529
// 000.000      000.000
// S1 T1          F1000
// MM LIN XY M1


	char tmpStr[LCD_cols];
	int len;

	// ---------
	// First Row

	// Grbl state
	switch (grblState)
	{
	case GRBLStates::Idle:
		strcpy(tmpStr, "Idle ");
		break;

	case GRBLStates::Queue:
		strcpy(tmpStr, "Queue");
		break;

	case GRBLStates::Run:
		strcpy(tmpStr, "Run  ");
		break;

	case GRBLStates::Hold:
		strcpy(tmpStr, "Hold ");
		break;

	case GRBLStates::Home:
		strcpy(tmpStr, "Home ");
		break;

	case GRBLStates::Alarm:
		strcpy(tmpStr, "Alarm");
		break;
		
	case GRBLStates::Check:
		strcpy(tmpStr, "Check");
		break;

	default:
		strcpy(tmpStr, "-----");
		break;
	}
	myLCD.setCursor(0, 0); // letter, row
	myLCD.print(tmpStr);

	// Pendant Mode
	myLCD.setCursor(7, 0); 
	switch (pendantMode)
	{
	case PendantModes::Monitor:
		myLCD.print("Mntr");
		break;
	case PendantModes::Control:
		myLCD.print("Ctrl");
		break;
	default:
		myLCD.print("----");
	}

	// Z position
	sprintf(tmpStr, "%9.3f", currentPosition.z);
	len = strlen(tmpStr);
	myLCD.setCursor((LCD_cols - len), 0);
	myLCD.print(tmpStr);

	// ---------
	// Second Row
	// X position
	sprintf(tmpStr, "%-9.3f", currentPosition.x);
	len = strlen(tmpStr);
	myLCD.setCursor(0, 1);
	myLCD.print(tmpStr);

	// Y position
	sprintf(tmpStr, "%9.3f", currentPosition.y);
	len = strlen(tmpStr);
	myLCD.setCursor((LCD_cols - len), 1);
	myLCD.print(tmpStr);


	// ---------
	// Third Row
	myLCD.setCursor(0, 2); //third row

	switch (currentMotionMode)
	{
	case MotionMode::Rapid:
		myLCD.print("RAP ");
		break;
	case MotionMode::Linear:
		myLCD.print("LIN ");
		break;
	case MotionMode::CW:
		myLCD.print("CW  ");
		break;
	case MotionMode::CCW:
		myLCD.print("CCW ");
		break;
	case MotionMode::Probe_2:
		myLCD.print("PRB2");
		break;
	case MotionMode::Probe_3:
		myLCD.print("PRB3");
		break;
	case MotionMode::Probe_4:
		myLCD.print("PRB4");
		break;
	case MotionMode::Probe_5:
		myLCD.print("PRB5");
		break;
	default:
		myLCD.print("--- ");
	}

	// Coordinates ..
	myLCD.setCursor(7, 2); // letter, row
	sprintf(tmpStr, "%4s", positionCoordSystem);
	myLCD.print(tmpStr);


	// Feed
	sprintf(tmpStr, "%4.4s", currentFeedRate);
	myLCD.setCursor((LCD_cols - strlen(tmpStr) - 4), 2);
	myLCD.print(strcat(tmpStr, "mm/s"));
	
	// ---------
    // Fourth Row
	
	myLCD.setCursor(0, 3);

	// Units
	switch (currentUnitsMode)
	{
	case UnitsMode::Inches:
		myLCD.print("in");
		break;
	case UnitsMode::mm:
		myLCD.print("mm");
		break;
	default:
		myLCD.print("--");
	}

	myLCD.print(" ");

	// Plane
	switch (currentPlaneSelect)
	{
	case PlaneSelect::XY:
		myLCD.print("XY");
		break;
	case PlaneSelect::ZX:
		myLCD.print("ZX");
		break;
	case PlaneSelect::YZ:
		myLCD.print("YZ");
		break;
	default:
		myLCD.print("--");
	}

	myLCD.print(" ");

	// Tool
	sprintf(tmpStr, "%3.3s", currentTool);
	myLCD.print(tmpStr);
	myLCD.print(" ");

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
		strcpy(tmpStr,"End");
		break;
	case ProgramMode::Rewind:
		strcpy(tmpStr,"Rwnd");
		break;
	default:
		strcpy(tmpStr,"-----");
	}

    myLCD.setCursor((LCD_cols - strlen(tmpStr)), 3);
    myLCD.print(tmpStr);
}
