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
		break;

	case '6':
		if (grblCoord == GRBLCoord::MPos)
		{
			sendGRBLCommand("$10=0\n");     // Set to WPos
		}
		else if (grblCoord == GRBLCoord::WPos)
		{
			sendGRBLCommand("$10=1\n");     // Set to MPos
		}
		else
		{
			sendGRBLCommand("$10=1\n");     // Set to MPos
		}
		break;

	case '7':
		currentJogAxis = CNCAxis::Z;
		ResetJogEncoder();
		break;

	case '8':
		currentJogScaling = 2;
		ResetJogEncoder();
		break;

	case '9':
		// Home
		sendGRBLCommand("$H\n");
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
		currentJogAxis = CNCAxis::Y;
		ResetJogEncoder();
		break;

	case 'B':
		currentJogScaling = 1;
		ResetJogEncoder();
		break;

	case 'C':
		// Unlock
		sendGRBLCommand("$X\n");
		break;

	case 'D':
		if (pendantMode == PendantModes::Control)
		{
			sendGRBLCommand("\x18\n");	// GRBL Soft reset (ctrl-x)
		}
		break;

	case 'E':

		currentJogAxis = CNCAxis::X;
		ResetJogEncoder();
		break;

	case 'F':
		currentJogScaling = 0;
		ResetJogEncoder();
		break;

		//default:
	}
}


