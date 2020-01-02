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
		currentJogScaling = 0;
		break;

	case '2':
		currentJogAxis = CNCAxis::X;
		break;

	case '3':
		if (pendantMode == PendantModes::Control)
		{
			spindle_on(100);
		}
		break;

	case '4':
		// Unlock
		sendGRBLCommand("$X\n");
		break;

	case '5':
		currentJogScaling = 1;
		break;

	case '6':
		currentJogAxis = CNCAxis::Y;
		break;

	case '7':
		if (pendantMode == PendantModes::Control)
		{
			spindle_off();
		}
		break;

	case '8':
		// Home
		sendGRBLCommand("$H\n");
		break;

	case '9':
		currentJogScaling = 2;
		break;

	case '0':
		currentJogAxis = CNCAxis::Z;
		break;

	case 'A':
		break;

	case 'B':
		if (pendantMode == PendantModes::Control)
		{
			pendantMode = PendantModes::Monitor;
		}
		else
		{
			pendantMode = PendantModes::Control;
		}

		break;

	case 'C':
		currentJogScaling = 3;
		break;

	case 'D':
		// Jog -
		if (pendantMode == PendantModes::Control)
		{
			send_jog_command(-1.0 * get_jog_step());
		}
		break;

	case 'E':
		// Jog +
		if (pendantMode == PendantModes::Control)
		{
			send_jog_command(+1.0 * get_jog_step());
		}
		break;

	case 'F':
		break;

		//default:
	}
}


