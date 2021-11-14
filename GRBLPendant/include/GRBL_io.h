//=========================================================
//Project: GRBL Pendant
//Module:  serial_io.h
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//----------------------------------------------------------
void SerialIOGRBL();
void SendGRBLCommand_NoCount(char* command);
void SendGRBLCommand(char* command);
void SendGRBLCommand_NoCount(const char command[]);
void SendGRBLCommand(const char command[]);
void ResetGRBLCommandCount();
void SendGRBLCommandSoftReset();
void SendGRBLCommandUnlock();
void SendGRBLCommandFeedHold();
void SendGRBLCommandCancelJog();
void SendGRBLCommandHome();
void SendGRBLCommandWPos();
void SendGRBLCommandMPos();
void ParseGrblLine(char* line_in);
void ParseStatusLine(char* line_in);
void ParseStateLine(char* stateLine);
void SendJogCommand(float displacement);
void spindle_on_speed(int speed, SpindleState state);
void spindle_on(SpindleState state);
void spindle_off();
void zero_axis(CNCAxis axis);
void goto_axis(CNCAxis axis, float position, float feed);
void goto_zero(float feed);
size_t GrblCommPrintCharArray(const char* charArrayIn);
size_t GrblCommWriteChar(const char charIn);
int GrblCommRead();
int GrblCommAvailable();
void USBDeviceCheck(uint32_t usbbaud, uint32_t usbformat);