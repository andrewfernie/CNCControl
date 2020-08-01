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
			ResetUIEncoder();
		}
		else 
		{
			menuMode = MenuModes::Status;
		}
		break;

	case '2':
		// Jog +
		ResetJogEncoder();
		if (pendantMode == PendantModes::Control)
		{
			send_jog_command(+1.0 * get_jog_step());
		}
		break;

	case '3':
		// Jog -
		ResetJogEncoder();
		if (pendantMode == PendantModes::Control)
		{
			send_jog_command(-1.0 * get_jog_step());
		}
		break;

	case '4':
		currentJogScaling = 3;
		ResetJogEncoder();
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

		ResetJogEncoder();

		sendGRBLCommandSoftReset();
		grbl_command_count = 0;
		break;

	case '6':
		if (grblCoord == GRBLCoord::MPos)
		{
			sendGRBLCommandWPos();     // Set to WPos
		}
		else if (grblCoord == GRBLCoord::WPos)
		{
			sendGRBLCommandMPos();     // Set to MPos
		}
		else
		{
			sendGRBLCommandMPos();     // Set to MPos
		}
		break;

	case '7':
		incrementJogRateIndex();
		break;

	case '8':
		currentJogScaling = 2;
		ResetJogEncoder();
		break;

	case '9':
		// Home
		sendGRBLCommandHome();
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
		setJogRateIndex(defaultJpogRateIndex);
		break;

	case 'B':
		currentJogScaling = 1;
		ResetJogEncoder();
		break;

	case 'C':
		// Unlock
		sendGRBLCommandUnlock();
		break;

	case 'D':
		if (pendantMode == PendantModes::Control)
		{
			sendGRBLCommandSoftReset();	// GRBL Soft reset (ctrl-x)
			grbl_command_count = 0;
		}
		break;

	case 'E':

		decrementJogRateIndex();
		break;

	case 'F':
		currentJogScaling = 0;
		ResetJogEncoder();
		break;

	case 'G':
		if (grblCoord == GRBLCoord::MPos)
		{
			sendGRBLCommandWPos();     // Set to WPos
		}
		else if (grblCoord == GRBLCoord::WPos)
		{
			sendGRBLCommandMPos();     // Set to MPos
		}
		else
		{
			sendGRBLCommandMPos();     // Set to MPos
		}
		break;

	case 'H':
		currentJogAxis = CNCAxis::Z;
		ResetJogEncoder();
		break;

	case 'I':
		currentJogAxis = CNCAxis::Y;
		ResetJogEncoder();
		break;

	case 'J':
		currentJogAxis = CNCAxis::X;
		ResetJogEncoder();
		break;

		//default:
	}
}


