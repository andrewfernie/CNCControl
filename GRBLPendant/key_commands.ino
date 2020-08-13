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
		if (menuMode == MenuModes::Status)
		{
			menuMode = MenuModes::Menu;
			uiEncoder.Reset();
		}
		else 
		{
			menuMode = MenuModes::Status;
		}
		break;

	case '2':
		// Jog +
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		if (pendantMode == PendantModes::Control)
		{
			SendJogCommand(+1.0 * getJogSize());
		}
		break;

	case '3':
		// Jog -
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		if (pendantMode == PendantModes::Control)
		{
			SendJogCommand(-1.0 * getJogSize());
		}
		break;

	case '4':
		enableAdjustableJogSize = true;
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

	case '5':
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

	case '6':
		if (grblCoord == GRBLCoord::MPos)
		{
			SendGRBLCommandWPos();     // Set to WPos
		}
		else if (grblCoord == GRBLCoord::WPos)
		{
			SendGRBLCommandMPos();     // Set to MPos
		}
		else
		{
			SendGRBLCommandMPos();     // Set to MPos
		}
		break;

	case '7':
		incrementJogRateIndex();
		break;

	case '8':
		incrementJogSizeIndex();
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

	case '9':
		// Home
		SendGRBLCommandHome();
		break;

	case '0':
		if (pendantMode == PendantModes::Control)
		{
			if (currentSpindleState == SpindleState::Off)
			{
				spindle_on(100);
			}
			else
			{
				spindle_off();
			}

			break;
		}
		break;

	case 'A':
		setJogRateDefault();
		break;

	case 'B':
		setJogSizeDefault();
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

	case 'C':
		// Unlock
		SendGRBLCommandUnlock();
		break;

	case 'D':
		if (pendantMode == PendantModes::Control)
		{
			SendGRBLCommandSoftReset();	// GRBL Soft reset (ctrl-x)
			ResetGRBLCommandCount();
		}
		break;

	case 'E':
		decrementJogRateIndex();
		break;

	case 'F':
		decrementJogSizeIndex();
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

	case 'G':
		SendGRBLCommandFeedHold();
		break;

	case 'H':
		currentJogAxis = CNCAxis::Z;
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

	case 'I':
		currentJogAxis = CNCAxis::Y;
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

	case 'J':
		currentJogAxis = CNCAxis::X;
		jogEncoder.Reset();
		lastJogCommandPosition = 0.0;
		break;

		//default:
	}
}


