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

		break;

	case '2':
		// Jog +
		ResetEncoderJog();
		if (pendantMode == PendantModes::Control)
		{
			send_jog_command(+1.0 * get_jog_step());
		}
		break;

	case '3':
		// Jog -
		ResetEncoderJog();
		if (pendantMode == PendantModes::Control)
		{
			send_jog_command(-1.0 * get_jog_step());
		}
		break;

	case '4':

		currentJogScaling = 3;
		ResetEncoderJog();
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
		ResetEncoderJog();
		break;

	case '6':

		break;

	case '7':
		currentJogAxis = CNCAxis::Z;
		ResetEncoderJog();
		break;

	case '8':
		currentJogScaling = 2;
		ResetEncoderJog();
		break;

	case '9':
		// Home
		sendGRBLCommand("$H\n");
		break;

	case '0':
		if (pendantMode == PendantModes::Control)
		{
			spindle_on(100);
		}
		break;

	case 'A':
		currentJogAxis = CNCAxis::Y;
		ResetEncoderJog();
		break;

	case 'B':
		currentJogScaling = 1;
		ResetEncoderJog();
		break;

	case 'C':
		// Unlock
		sendGRBLCommand("$X\n");
		break;

	case 'D':
		if (pendantMode == PendantModes::Control)
		{
			spindle_off();
		}
		break;

	case 'E':

		currentJogAxis = CNCAxis::X;
		ResetEncoderJog();
		break;

	case 'F':
		currentJogScaling = 0;
		ResetEncoderJog();
		break;

		//default:
	}
}


