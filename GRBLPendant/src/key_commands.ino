//=========================================================
//Project: GRBL Pendant
//Module:  key_commands.ino
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

#include "GRBLPendant.h"
#include "GRBL_io.h"

void ProcessKey(char key)
{
    switch (key) {
        case '1':
            SendGRBLCommandCancelJog();
            break;

        case '2':       
            // Stop, even if not in control mode
            SendGRBLCommandSoftReset();  // GRBL Soft reset (ctrl-x)
            ResetGRBLCommandCount();
            spindle_off();
            break;

        case '3':
            // Home
            if (pendantMode == PendantModes::Control) {
                SendGRBLCommandHome();
            }
            break;

        case '4':
            switch (currentSpindleState) {
                case SpindleState::Off:
                    if (pendantMode == PendantModes::Control) {
                        spindle_on(SpindleState::CW);
                    }
                    break;

                case SpindleState::CW:
                    spindle_off();
                    break;

                case SpindleState::CCW:
                    spindle_off();
                    break;

                case SpindleState::Undefined:
                    spindle_off();
                    break;

                default:
                    spindle_off();
                    break;
            }
            break;

        case '5':
            if (pendantMode == PendantModes::Control) {
                goto_zero(getJogRate());
            }
            break;

        case '6':
            currentJogAxis = CNCAxis::X;
            jogEncoder.Reset();
            lastJogCommandPosition = 0.0;
            break;

        case '7':
            if (pendantMode == PendantModes::Control) {
                SendGRBLCommandSoftReset();  // GRBL Soft reset (ctrl-x)
                ResetGRBLCommandCount();
            }
            break;

        case '8':
            // Unlock
            if (pendantMode == PendantModes::Control) {
                SendGRBLCommandUnlock();
            }
            break;

        case '9':
            if (pendantMode == PendantModes::Control) {
                pendantMode = PendantModes::Monitor;
            }
            else {
                pendantMode = PendantModes::Control;
            }

            jogEncoder.Reset();
            lastJogCommandPosition = 0.0;

            ResetGRBLCommandCount();
            break;

            //case '6':
            //	if (grblCoord == GRBLCoord::MPos)
            //	{
            //		SendGRBLCommandWPos();     // Set to WPos
            //	}
            //	else if (grblCoord == GRBLCoord::WPos)
            //	{
            //		SendGRBLCommandMPos();     // Set to MPos
            //	}
            //	else
            //	{
            //		SendGRBLCommandMPos();     // Set to MPos
            //	}
            //	break;

        case '0':
            if (pendantMode == PendantModes::Control) {
                zero_axis(currentJogAxis);
            }
            break;

        case 'A':
            currentJogAxis = CNCAxis::Y;
            jogEncoder.Reset();
            lastJogCommandPosition = 0.0;
            break;

        case 'B':
            currentSetMode = SetMode::Feed;
            break;

        case 'C':
            currentSetMode = SetMode::Move;
            break;

        case 'D':
            currentSetMode = SetMode::Spindle;
            break;

        case 'E':
            if (pendantMode == PendantModes::Control) {
                zero_axis(CNCAxis::X);
                zero_axis(CNCAxis::Y);
                zero_axis(CNCAxis::Z);
            }
            break;

        case 'F':
            currentJogAxis = CNCAxis::Z;
            jogEncoder.Reset();
            lastJogCommandPosition = 0.0;
            break;

        case 'G':
            switch (currentSetMode) {
                case SetMode::Feed:
                    decrementJogRateIndex();
                    break;

                case SetMode::Move:
                    decrementJogSizeIndex();
                    jogEncoder.Reset();
                    lastJogCommandPosition = 0.0;
                    break;

                case SetMode::Spindle:
                    decrementSpindleRPMIndex();
                    switch (currentSpindleState) {
                        case SpindleState::CW:
                            if (pendantMode == PendantModes::Control) {
                                spindle_on(SpindleState::CW);
                            }
                            break;
                        case SpindleState::CCW:
                            if (pendantMode == PendantModes::Control) {
                                spindle_on(SpindleState::CCW);
                            }
                            break;
                        case SpindleState::Off:
                            spindle_off();
                            break;
                        case SpindleState::Undefined:
                            spindle_off();
                            break;
                        default:
                            break;
                    }
            }
            break;

        case 'H':
            switch (currentSetMode) {
                case SetMode::Feed:
                    setJogRateDefault();
                    break;

                case SetMode::Move:
                    setJogSizeDefault();
                    jogEncoder.Reset();
                    lastJogCommandPosition = 0.0;
                    break;

                case SetMode::Spindle:
                    setSpindleRPMDefault();
                    switch (currentSpindleState) {
                        case SpindleState::CW:
                            if (pendantMode == PendantModes::Control) {
                                spindle_on(SpindleState::CW);
                            }
                            break;
                        case SpindleState::CCW:
                            if (pendantMode == PendantModes::Control) {
                                spindle_on(SpindleState::CCW);
                            }
                            break;
                        case SpindleState::Off:
                            spindle_off();
                            break;
                        case SpindleState::Undefined:
                            spindle_off();
                            break;
                        default:
                            break;
                    }
                    break;
            }
            break;

        case 'I':
            switch (currentSetMode) {
                case SetMode::Feed:
                    incrementJogRateIndex();
                    break;

                case SetMode::Move:
                    incrementJogSizeIndex();
                    jogEncoder.Reset();
                    lastJogCommandPosition = 0.0;
                    break;

                case SetMode::Spindle:
                    incrementSpindleRPMIndex();
                    switch (currentSpindleState) {
                        case SpindleState::CW:
                            if (pendantMode == PendantModes::Control) {
                                spindle_on(SpindleState::CW);
                            }
                            break;
                        case SpindleState::CCW:
                            if (pendantMode == PendantModes::Control) {
                                spindle_on(SpindleState::CCW);
                            }
                            break;
                        case SpindleState::Off:
                            spindle_off();
                            break;
                        case SpindleState::Undefined:
                            spindle_off();
                            break;
                        default:
                            break;
                    }
                    break;
            }
            break;

        case 'J':
            if (menuMode == MenuModes::Status) {
                StatusLCD.clear();
                menuMode = MenuModes::Menu;
                uiEncoder.Reset();
            }
            else {
                StatusLCD.clear();
                menuMode = MenuModes::Status;
            }
            break;

            //case 'z':
            //	// Jog +
            //	jogEncoder.Reset();
            //	lastJogCommandPosition = 0.0;
            //	if (pendantMode == PendantModes::Control)
            //	{
            //		SendJogCommand(+1.0 * getJogSize());
            //	}
            //	break;

            //case 'y':
            //	// Jog -
            //	jogEncoder.Reset();
            //	lastJogCommandPosition = 0.0;
            //	if (pendantMode == PendantModes::Control)
            //	{
            //		SendJogCommand(-1.0 * getJogSize());
            //	}
            //	break;

            //case 'x':
            //	enableAdjustableJogSize = true;
            //	jogEncoder.Reset();
            //	lastJogCommandPosition = 0.0;
            //	break;
        default:
            break;
    }
}
