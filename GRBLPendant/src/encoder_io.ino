//=========================================================
//Project: GRBL Pendant
//Module:  encoder.ino
//=========================================================
//
// GRBLPendant CNC control Copyright(C) 2021 Andrew Fernie
//
// This program is free software : you can redistribute it and /
// or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
//=========================================================
//
// This project includes code from the  XLCD/SerialSpy
// project by Frank Herrmann.
// https://github.com/xpix/XLCD/tree/master/serialspy
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
