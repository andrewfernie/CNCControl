//=========================================================
//Project: GRBL Pendant
//Module:  encoder.ino         
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//----------------------------------------------------------

//--------------------------------------------------------------------------------------
// ReadUIEncoder()
//--------------------------------------------------------------------------------------
long ReadUIEncoder()
{

#ifdef UI_ENC_A
	long newEncoder = uiEncoder.read();

	return -newEncoder;
#endif
}

//--------------------------------------------------------------------------------------
// ResetUIEncoderCount()
//--------------------------------------------------------------------------------------
void ResetUIEncoderCount()
{
#ifdef UI_ENC_A
	uiEncoder.write(0);
#endif
}

//--------------------------------------------------------------------------------------
// ReadUIEncoderSwitch(()
//--------------------------------------------------------------------------------------
bool ReadUIEncoderSwitch()
{
	bool sw = false;
#ifdef UI_ENC_S
	// Rotary Encoder Select
	sw = !digitalRead(UI_ENC_S);
#endif
	return sw;
}

void ResetUIEncoder()
{
	ResetUIEncoderCount();
}

// ==============================


//--------------------------------------------------------------------------------------
// ReadJogResetButton()
//--------------------------------------------------------------------------------------
bool ReadJogResetButton()
{
	bool sw = false;

	// Rotary Encoder Select
	sw = !jogResetButton.read();

	return sw;
}
