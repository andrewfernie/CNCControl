//=========================================================
//Project: GRBL Pendant
//Module:  key_commands.ino        
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//---------------------------------------------------------

void ProcessKey(char key)
{
	
    switch (key)
	{
	case '1':
		goto_zero(getJogRate());
		break;

	case '2':
		zero_axis(currentJogAxis);
		break;

	case '3':
		zero_axis(CNCAxis::X);
		zero_axis(CNCAxis::Y);
		zero_axis(CNCAxis::Z);
		break;

	case '4':
		if (menuMode == MenuModes::Status)
		{
			StatusLCD.clear();
			menuMode = MenuModes::Menu;
			uiEncoder.Reset();
		}
		else
		{
			StatusLCD.clear();
			menuMode = MenuModes::Status;
		}
		break;

	//case 'z':
	//	// Jog +
	//	jogEncoder.Reset();
	//	lastJogCommandPosition = 0.0;
	//	if (pendantMode == PendantModes::Control)
	//	{
	//		SendJogCommand(+1.0 * getJogSize());
	//	}
	//	break;

	//case 'y':
	//	// Jog -
	//	jogEncoder.Reset();
	//	lastJogCommandPosition = 0.0;
	//	if (pendantMode == PendantModes::Control)
	//	{
	//		SendJogCommand(-1.0 * getJogSize());
	//	}
	//	break;

	//case 'x':
	//	enableAdjustableJogSize = true;
	//	jogEncoder.Reset();
	//	lastJogCommandPosition = 0.0;
	//	break;

	case '5':
		switch (currentSpindleState)
		{
			case SpindleState::Off:
				spindle_on(SpindleState::CW);
				break;

			case SpindleState::CW:
				spindle_off();
				break;

			case SpindleState::CCW:
				spindle_off();
				break;

			case SpindleState::Undefined:
				spindle_off();
				break;

			default:
				spindle_off();
				break;
		}
		break;

	case '6':
		if (pendantMode == PendantModes::Control)
		{
			pendantMode = PendantModes::Monitor;
		}
		else
		{
			pendantMode = PendantModes::Control;
		}

		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;

		SendGRBLCommandSoftReset();
		ResetGRBLCommandCount();
		break;

	//case '6':
	//	if (grblCoord == GRBLCoord::MPos)
	//	{
	//		SendGRBLCommandWPos();     // Set to WPos
	//	}
	//	else if (grblCoord == GRBLCoord::WPos)
	//	{
	//		SendGRBLCommandMPos();     // Set to MPos
	//	}
	//	else
	//	{
	//		SendGRBLCommandMPos();     // Set to MPos
	//	}
	//	break;

	case '7':
		currentSetMode = SetMode::Spindle;
		break;

	case '8':
		switch (currentSetMode)
		{
		case SetMode::Feed:
			incrementJogRateIndex();
			break;

		case SetMode::Move:
			incrementJogSizeIndex();
			jogEncoder.Reset();
			lastJogCommandPosition = 0.0;
			break;

		case SetMode::Spindle:
			incrementSpindleRPMIndex();
			switch (currentSpindleState)
			{
			case SpindleState::CW:
				spindle_on(SpindleState::CW);
				break;
			case SpindleState::CCW:
				spindle_on(SpindleState::CCW);
				break;
			case SpindleState::Off:
				spindle_off();
				break;
			case SpindleState::Undefined:
				spindle_off();
				break;
			default:
				break;
			}
			break;
		}
		break;

	case '9':
		// Home
		SendGRBLCommandHome();
		break;

	case '0':
		// Unlock
		SendGRBLCommandUnlock();
		break;

	case 'A':
		currentSetMode = SetMode::Move;
		break;

	case 'B':
		switch (currentSetMode)
		{
		case SetMode::Feed:
			setJogRateDefault();
			break;

		case SetMode::Move:
			setJogSizeDefault();
			jogEncoder.Reset();
			lastJogCommandPosition = 0.0;
			break;

		case SetMode::Spindle:
			setSpindleRPMDefault();
			switch (currentSpindleState)
			{
			case SpindleState::CW:
				spindle_on(SpindleState::CW);
				break;
			case SpindleState::CCW:
				spindle_on(SpindleState::CCW);
				break;
			case SpindleState::Off:
				spindle_off();
				break;
			case SpindleState::Undefined:
				spindle_off();
				break;
			default:
				break;
			}
			break;
		}
		break;

	case 'C':
		// Stop
		if (pendantMode == PendantModes::Control)
		{
			SendGRBLCommandSoftReset();	// GRBL Soft reset (ctrl-x)
			ResetGRBLCommandCount();
			spindle_off();
		}
		break;

	case 'D':
		if (pendantMode == PendantModes::Control)
		{
			SendGRBLCommandSoftReset();	// GRBL Soft reset (ctrl-x)
			ResetGRBLCommandCount();
		}
		break;

	case 'E':
		currentSetMode = SetMode::Feed;
		break;


	case 'F':
		switch (currentSetMode)
		{
		case SetMode::Feed:
			decrementJogRateIndex();
			break;

		case SetMode::Move:
			decrementJogSizeIndex();
			jogEncoder.Reset();
			lastJogCommandPosition = 0.0;
			break;

		case SetMode::Spindle:
			decrementSpindleRPMIndex();
			switch (currentSpindleState)
			{
			case SpindleState::CW:
				spindle_on(SpindleState::CW);
				break;
			case SpindleState::CCW:
				spindle_on(SpindleState::CCW);
				break;
			case SpindleState::Off:
				spindle_off();
				break;
			case SpindleState::Undefined:
				spindle_off();
				break;
			default:
				break;
			}
		}
		break;

	case 'G':
		SendGRBLCommandFeedHold();
		break;

	case 'H':
		currentJogAxis = CNCAxis::X;
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

	case 'I':
		currentJogAxis = CNCAxis::Y;
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

	case 'J':
		currentJogAxis = CNCAxis::Z;
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

		//default:
	}
}


