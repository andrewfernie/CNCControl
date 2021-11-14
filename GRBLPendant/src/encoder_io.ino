//=========================================================
//Project: GRBL Pendant
//Module:  encoder.ino
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like
//
//=========================================================

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
