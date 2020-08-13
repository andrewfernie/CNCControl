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
// ReadJogResetButton()
//--------------------------------------------------------------------------------------
bool ReadJogResetButton()
{
	bool sw = false;

	// Rotary Encoder Select
	sw = !jogResetButton.read();

	return sw;
}
