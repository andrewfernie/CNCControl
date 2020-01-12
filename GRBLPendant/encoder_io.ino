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

	return newEncoder;
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
#ifdef UI_ENC_S
	// Rotary Encoder Select
	return !digitalRead(UI_ENC_S);
#endif
}

void ResetUIEncoder()
{
	ResetUIEncoderCount();
}

// ==============================

//--------------------------------------------------------------------------------------
// ReadJogEncoder()
//--------------------------------------------------------------------------------------
long ReadJogEncoder()
{

#ifdef JOG_ENC_A
	long newEncoder = jogEncoder.read();

	return newEncoder;
#endif
}

//--------------------------------------------------------------------------------------
// ResetJogEncoderCount()
//--------------------------------------------------------------------------------------
void ResetJogEncoderCount()
{
#ifdef JOG_ENC_A
	jogEncoder.write(0);
#endif
}

//--------------------------------------------------------------------------------------
// ReadJogEncoderSwitch(()
//--------------------------------------------------------------------------------------
bool ReadJogEncoderSwitch()
{
#ifdef JOG_ENC_S
	// Rotary Encoder Select
	return !digitalRead(JOG_ENC_S);
#endif
}

void ResetJogEncoder()
{
	ResetJogEncoderCount();
	lastJogCommandPosition = 0.0;
}

