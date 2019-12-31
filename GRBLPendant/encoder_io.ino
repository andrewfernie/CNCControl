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
// ReadEncoder()
//--------------------------------------------------------------------------------------
long ReadEncoder()
{

#ifdef ENC_A
	long newEncoder = encoder.read();

	return newEncoder;
#endif
}

//--------------------------------------------------------------------------------------
// ReadEncoderSwitch()
//--------------------------------------------------------------------------------------
bool ReadEncoderSwitch()
{
#ifdef ENC_S
	// Rotary Encoder Select
	return digitalRead(ENC_S);
#endif
}

