//=========================================================
//Project: GRBL Pendant
//Module:  GRBL_io.ino        
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//----------------------------------------------------------
#include "GRBLPendant.h"
#include "errors_alarms.h"

// Get data from GRBL ==> PC
void SerialIOGRBL()
{
    while (GrblCommAvailable())
    {
        char c = GrblCommRead();

        // wait for a complete line 
        // and parse it
        if (c == '\n') {

            DEBUG_PRINTLN(grserial);
            ParseGrblLine(grserial);

            gr = 0;
            memset(&grserial[0], 0, sizeof(grserial));
            grserial[0] = '\0';
        }
        else {
            if (gr < BufferSize)
                grserial[gr++] = c;
        }
    }
}


// Analyze every line and choose an action
void ParseGrblLine(char* line_in)
{
    char line[BufferSize];

    strcpy(line, line_in);

    char* c2 = strrchr(line, '\r');
    *c2 = '\0';

    if (line[0] == '<')
    {
        ParseStatusLine(line);
        gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
        gsSerial.print("\n");
    }
    else if (strncmp(line, "[MSG:", 5) == 0)
    {
        if (pendantMode != PendantModes::Control)
        {
            gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
            gsSerial.print("\n");
        }
        for (int i = 0; i <= 20; i++)
            lastMessage[i] = '\0';

        strncpy(lastMessage, line_in, 20);
        lastMessage[20] = '\0';
        lastMessageTime = millis();

        DEBUG_PRINTLN(line_in);
    }
    else if (line[0] == '[')
    {
        ParseStateLine(line);

        if (pendantMode != PendantModes::Control)
        {
            gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
            gsSerial.print("\n");
        }
        DEBUG_PRINTLN(line_in);
    }
    else if (strncmp(line, "ok", 2) == 0)
    {
        if (pendantMode == PendantModes::Control)
        {
            if (grblCommandCount > 0)    // Prevent count from going below zero
                grblCommandCount--;
            else
                ResetGRBLCommandCount();
        }
        else
        {
            gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
            gsSerial.print("\n");
        }
    }
    else if (strncmp(line, "error", 5) == 0)
    {
        if (pendantMode != PendantModes::Control)
        {
            gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
            gsSerial.print("\n");
        }
        DEBUG_PRINTLN(line_in);

        char errCode[3] = { 0,0,0 };
        errCode[0] = line[6];
        if (line[7] >= '0' && line[7] <= '9')
            errCode[1] = line[7];

        errorNumber = atoi(errCode);

        if (errorNumber < errorNumMin)
        {
            errorNumber = 0;
        }
        else if (errorNumber > errorNumMax)
        {
            errorNumber = 0;
        }
        lastErrorTime = millis();

    }
    else if (strncmp(line, "ALARM", 5) == 0)
    {
        if (pendantMode != PendantModes::Control)
        {
            gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
            gsSerial.print("\n");
        }

        lastAlarmTime = millis();

        //DEBUG_PRINTLN(line_in);

        alarmNumber = line[6] - '0';
        if (alarmNumber < alarm_num_min)
        {
            alarmNumber = 0;
        }
        else if (alarmNumber > alarm_num_max)
        {
            alarmNumber = 0;
        }
    }
    else
    {
        gsSerial.print(line_in);     // send line from the grbl controller to the g-code sender
        gsSerial.print("\n");
    }

}


//// If the pendant is in monitor mode send everything on to the sender.
//// If not, dont send data from $G or empty strings as UGS objects.
//if ((pendantMode == PendantModes::Monitor) || ((grserial[0] != '[') && (grserial[0] != 0)))
//{
//	gsSerial.print(grserial);     // send line from the grbl controller to the g-code sender
//	gsSerial.print(c);
//}
void ParseStatusLine(char* line_in)
{
    // Typical status lines formats
    //
    // <Idle | WPos:73.000, 0.000, -20.000 | FS : 0, 0>
    // <Idle | WPos:73.000, 0.000, -20.000 | FS : 0, 0 | WCO : -75.000, 0.000, -358.000>
    // <Idle | WPos:73.000, 0.000, -20.000 | FS : 0, 0 | Ov : 100, 100, 100>

    char delim[] = "<,:|";
    char* temp;
    float tmpFloat;
    int   n;
    char line[BufferSize];

    lastStatusRXTime = millis();

    strcpy(line, line_in);	// work with local copy as strtok modifies the source

    // First Line
    // State ..
    temp = strtok(line, delim);

    if (temp != NULL)
        set_grblState_from_chars(temp);
    else
        grblState = GRBLStates::Undefined;


    // Coordinates ..
    temp = strtok(NULL, delim);

    if (temp != NULL)
    {
        if (strcmp(temp, "MPos") == 0)
        {
            grblCoord = GRBLCoord::MPos;
        }
        else if (strcmp(temp, "WPos") == 0)
        {
            grblCoord = GRBLCoord::WPos;
        }
        else
        {
            grblCoord = GRBLCoord::Undefined;
        }
    }
    else
    {
        grblCoord = GRBLCoord::Undefined;
    }


    // X Machine position ...
    temp = strtok(NULL, delim);
    if (temp != NULL)
    {
        n = sscanf(temp, "%f", &tmpFloat);

        if (n == 1)
        {
            currentPosition.x = tmpFloat;
        }
    }


    // Y Machine position ...
    temp = strtok(NULL, delim);
    if (temp != NULL)
    {
        n = sscanf(temp, "%f", &tmpFloat);

        if (n == 1)
        {
            currentPosition.y = tmpFloat;
        }
    }

    // Z Machine position ...
    temp = strtok(NULL, delim);
    if (temp != NULL)
    {
        n = sscanf(temp, "%f", &tmpFloat);

        if (n == 1)
        {
            currentPosition.z = tmpFloat;
        }
    }

    temp = strtok(NULL, delim);
    while (temp != NULL)
    {
        if (strcmp(temp, "F") == 0)
        {
            if (sscanf(temp, "%f", &tmpFloat) == 1)
            {
                currentFeedRate = tmpFloat;
            }

        }
        else if (strcmp(temp, "FS") == 0)
        {
            temp = strtok(NULL, delim);
            if (sscanf(temp, "%f", &tmpFloat) == 1)
            {
                currentFeedRate = tmpFloat;
            }

            temp = strtok(NULL, delim);
            if (sscanf(temp, "%f", &tmpFloat) == 1)
            {
                currentSpindleSpeed = tmpFloat;
                currentSpindleRPMIndex = findClosestSpindleRPMIndex(tmpFloat);
            }
        }
        else if (strcmp(temp, "WCO") == 0)
        {
            temp = strtok(NULL, delim);
            if (sscanf(temp, "%f", &tmpFloat) == 1)
            {
                currentWCO.x = tmpFloat;
            }

            temp = strtok(NULL, delim);
            if (sscanf(temp, "%f", &tmpFloat) == 1)
            {
                currentWCO.y = tmpFloat;
            }

            temp = strtok(NULL, delim);
            if (sscanf(temp, "%f", &tmpFloat) == 1)
            {
                currentWCO.z = tmpFloat;
            }
        }
        else if (strcmp(temp, "Ov") == 0)
        {
            temp = strtok(NULL, delim);
            if (sscanf(temp, "%f", &tmpFloat) == 1)
            {
                currentOvFeedRatePercent = tmpFloat;
            }

            temp = strtok(NULL, delim);
            if (sscanf(temp, "%f", &tmpFloat) == 1)
            {
                currentOvRapidRatePercent = tmpFloat;
            }

            temp = strtok(NULL, delim);
            if (sscanf(temp, "%f", &tmpFloat) == 1)
            {
                //TODO				currentOvSpindleSpeedPercent = tmpFloat;
            }
        }
        temp = strtok(NULL, delim);
    }

    lastCall = millis();
}

// send every second the command $G
void ParseStateLine(char* stateLine)
{
    // Typical state line
    // [GC:G0 G54 G17 G21 G90 G94 M5 M9 T0 F0 S0]
    //


    char delim[] = "[ ]:";

    //             mm                   TNr Feed
    // G0 G54 G17 G21 G90 G94 M0 M5 M9 T0 F500.000

    char* thisToken;

    char buffer[BufferSize];

    lastStateRXTime = millis();

    strcpy(buffer, stateLine);

    // init and create first cut
    thisToken = strtok(buffer, delim);

    while (thisToken != NULL)
    {
        if (thisToken[0] == 'G')
        {
            // Motion Mode
            if (strcmp(thisToken, "G0") == 0)
                currentMotionMode = MotionMode::Rapid;
            else if (strcmp(thisToken, "G1") == 0)
                currentMotionMode = MotionMode::Linear;
            else if (strcmp(thisToken, "G2") == 0)
                currentMotionMode = MotionMode::CW;
            else if (strcmp(thisToken, "G3") == 0)
                currentMotionMode = MotionMode::CCW;
            else if (strcmp(thisToken, "G38.2") == 0)
                currentMotionMode = MotionMode::Probe_2;
            else if (strcmp(thisToken, "G38.3") == 0)
                currentMotionMode = MotionMode::Probe_3;
            else if (strcmp(thisToken, "G38.4") == 0)
                currentMotionMode = MotionMode::Probe_4;
            else if (strcmp(thisToken, "G38.5") == 0)
                currentMotionMode = MotionMode::Probe_5;
            else if (strcmp(thisToken, "G80") == 0)
                currentMotionMode = MotionMode::Cancel;

            // Plane Select
            else if (strcmp(thisToken, "G17") == 0)
                currentPlaneSelect = PlaneSelect::XY;
            else if (strcmp(thisToken, "G18") == 0)
                currentPlaneSelect = PlaneSelect::ZX;
            else if (strcmp(thisToken, "G19") == 0)
                currentPlaneSelect = PlaneSelect::YZ;

            // Units Mode
            else if (strcmp(thisToken, "G20") == 0)
                currentUnitsMode = UnitsMode::Inches;
            else if (strcmp(thisToken, "G21") == 0)
                currentUnitsMode = UnitsMode::mm;

            // Coordinate System Select    
            else if (strcmp(thisToken, "G54") == 0)
                currentCoordinateSystemSelect = CoordinateSystemSelect::WCS1;
            else if (strcmp(thisToken, "G55") == 0)
                currentCoordinateSystemSelect = CoordinateSystemSelect::WCS2;
            else if (strcmp(thisToken, "G56") == 0)
                currentCoordinateSystemSelect = CoordinateSystemSelect::WCS3;
            else if (strcmp(thisToken, "G57") == 0)
                currentCoordinateSystemSelect = CoordinateSystemSelect::WCS4;
            else if (strcmp(thisToken, "G58") == 0)
                currentCoordinateSystemSelect = CoordinateSystemSelect::WCS5;
            else if (strcmp(thisToken, "G59") == 0)
                currentCoordinateSystemSelect = CoordinateSystemSelect::WCS6;

            // Distance Mode
            else if (strcmp(thisToken, "G90") == 0)
                currentDistanceMode = DistanceMode::Absolute;
            else if (strcmp(thisToken, "G91") == 0)
                currentDistanceMode = DistanceMode::Incremental;

            // Arc Distance Mode
            // Doesn't seem to make sense - need to look into this one in more detail
            else if (strcmp(thisToken, "G91.1") == 0)
                currentArcDistanceMode = ArcDistanceMode::On;

            // FeedRate Mode
            else if (strcmp(thisToken, "G93") == 0)
                currentFeedRateMode = FeedRateMode::Inverse;
            else if (strcmp(thisToken, "G94") == 0)
                currentFeedRateMode = FeedRateMode::Normal;

            // Cutter Radius Compensation
            else if (strcmp(thisToken, "G40") == 0)
                currentCutterRadiusCompensation = CutterRadiusCompensation::On;


            // Tool Length Offset          G43.1, G49
            else if (strcmp(thisToken, "G43.1") == 0)
                currentToolLengthOffsetMode = ToolLengthOffsetMode::Dynamic;
            else if (strcmp(thisToken, "G49") == 0)
                currentToolLengthOffsetMode = ToolLengthOffsetMode::Cancel;

        }
        else if (thisToken[0] == 'M')
        {
            // Program Mode                M0, M1, M2, M30
            if (strcmp(thisToken, "M0") == 0)
                currentProgramMode = ProgramMode::Stop;
            else if (strcmp(thisToken, "M1") == 0)
                currentProgramMode = ProgramMode::Optional;
            else if (strcmp(thisToken, "M2") == 0)
                currentProgramMode = ProgramMode::End;
            else if (strcmp(thisToken, "M30") == 0)
                currentProgramMode = ProgramMode::Rewind;

            // Spindle
            else if (strcmp(thisToken, "M3") == 0)
                currentSpindleState = SpindleState::CW;
            else if (strcmp(thisToken, "M4") == 0)
                currentSpindleState = SpindleState::CCW;
            else if (strcmp(thisToken, "M5") == 0)
                currentSpindleState = SpindleState::Off;

            // Coolant
            else if (strcmp(thisToken, "M7") == 0)
                currentCoolantState = CoolantState::Mist;
            else if (strcmp(thisToken, "M8") == 0)
                currentCoolantState = CoolantState::Flood;
            else if (strcmp(thisToken, "M9") == 0)
                currentCoolantState = CoolantState::Off;
        }
        else if (thisToken[0] == 'T')
        {
            strcpy(currentTool, thisToken);
        }
        else if (thisToken[0] == 'F')
        {
            float tmpFloat;

            if (sscanf(thisToken, "%f", &tmpFloat) == 1)
            {
                currentFeedRate = tmpFloat;
            }
        }

        // next one
        thisToken = strtok(NULL, delim);
    }
}

void SendGRBLCommand_NoCount(char* command)
{
    GrblCommPrintCharArray(command);
}
void SendGRBLCommand(char* command)
{
    SendGRBLCommand_NoCount(command);
    grblCommandCount++;
}
void SendGRBLCommand_NoCount(const char command[])
{
    GrblCommPrintCharArray(command);
}

void SendGRBLCommand(const char command[])
{
    SendGRBLCommand_NoCount(command);
    grblCommandCount++;
}

void ResetGRBLCommandCount()
{
    grblCommandCount = 0;
}

void SendGRBLCommandSoftReset()
{
    SendGRBLCommand("\x18");	// GRBL Soft reset (ctrl-x) - note no CRLF
}

void SendGRBLCommandUnlock()
{
    SendGRBLCommand("$X\n");
}

void SendGRBLCommandFeedHold()
{
    SendGRBLCommand("\x21");     // GRBL Feed hold - note no CRLF
    stopJogCommand = true;		// flag to the jog controller to stop sending
    ResetGRBLCommandCount();	// GRBL flushes the buffer when it receives a feed hold
}

void SendGRBLCommandCancelJog()
{
    SendGRBLCommand("\x85");  // GRBL cancel jog - note no CRLF
    stopJogCommand = true;    // flag to the jog controller to stop sending
    ResetGRBLCommandCount();  // GRBL flushes the buffer when it receives a feed hold
}

void SendGRBLCommandHome()
{
    SendGRBLCommand("$H\n");
}

void SendGRBLCommandWPos()
{
    SendGRBLCommand("$10=0\n");     // Set to WPos
}

void SendGRBLCommandMPos()
{
    SendGRBLCommand("$10=1\n");     // Set to MPos
}




void SendJogCommand(float displacement)
{
    char commandStr[30];
    char tmpStr[30];

    strcpy(commandStr, "$J=G21G91");

    switch (currentJogAxis)
    {
    case CNCAxis::X:
        strcat(commandStr, "X");
        break;
    case CNCAxis::Y:
        strcat(commandStr, "Y");
        break;
    case CNCAxis::Z:
        strcat(commandStr, "Z");
        break;
    }

    sprintf(tmpStr, "%f", displacement);
    strcat(commandStr, tmpStr);

    sprintf(tmpStr, "F%d\n", int(getJogRate()));
    strcat(commandStr, tmpStr);

    SendGRBLCommand(commandStr);

}

void spindle_on_speed(int speed, SpindleState state)
{
    char commandStr[30];
    char tmpStr[30];

    if (state == SpindleState::CW) {
        strcpy(commandStr, "M3S");
        sprintf(tmpStr, "%d\n", speed);
        strcat(commandStr, tmpStr);
        SendGRBLCommand(commandStr);
    }
    else if (state == SpindleState::CCW) {
        strcpy(commandStr, "M4S");
        sprintf(tmpStr, "%d\n", speed);
        strcat(commandStr, tmpStr);
        SendGRBLCommand(commandStr);
    }
}


void spindle_on(SpindleState state)
{
    char commandStr[30];
    char tmpStr[30];
    if (state == SpindleState::CW) {
        strcpy(commandStr, "M3S");
        sprintf(tmpStr, "%d\n", int(getSpindleRPM()));
        strcat(commandStr, tmpStr);
        SendGRBLCommand(commandStr);
    }
    else if (state == SpindleState::CCW) {
        strcpy(commandStr, "M4S");
        sprintf(tmpStr, "%d\n", int(getSpindleRPM()));
        strcat(commandStr, tmpStr);
        SendGRBLCommand(commandStr);
    }
}

void spindle_off()
{
    SendGRBLCommand("M5\n");
}

void zero_axis(CNCAxis axis)
{
    switch (axis)
    {
    case CNCAxis::X:
        SendGRBLCommand("G10 L20 P1 X0.0\n");
        break;

    case CNCAxis::Y:
        SendGRBLCommand("G10 L20 P2 X0.0\n");
        break;

    case CNCAxis::Z:
        SendGRBLCommand("G10 L20 P3 X0.0\n");
        break;
    }
}

void goto_axis(CNCAxis axis, float position, float feed)
{
    char cmdString[30];
    switch (axis)
    {
    case CNCAxis::X:
        snprintf(cmdString, sizeof(cmdString), "G90 X%9.4f F%d\n", position, int(feed));
        break;

    case CNCAxis::Y:
        snprintf(cmdString, sizeof(cmdString), "G90 Y%9.4f F%d\n", position, int(feed));
        break;

    case CNCAxis::Z:
        snprintf(cmdString, sizeof(cmdString), "G90 Z%9.4f F%d\n", position, int(feed));
        break;
    }
    SendGRBLCommand(cmdString);
}

void goto_zero(float feed)
{
    char cmdString[30];
    snprintf(cmdString, sizeof(cmdString), "G90 X0Y0Z0 F%d\n", int(feed));
    SendGRBLCommand(cmdString);
}

size_t GrblCommPrintCharArray(const char* charArrayIn)
{
    size_t returnValue;

#ifdef GRBL_COMM_UART
    returnValue = grblSerial.print(charArrayIn);
#else
    returnValue = grblUSBSerial.print(charArrayIn);
    DEBUG_PRINT(charArrayIn);
#endif
    return returnValue;
}

size_t GrblCommWriteChar(const char charIn)
{
    size_t returnValue;

#ifdef GRBL_COMM_UART
    returnValue = grblSerial.write(charIn);
#else
    returnValue = grblUSBSerial.write(charIn);
#endif
    return returnValue;
}

int GrblCommRead()
{
    int returnValue;

#ifdef GRBL_COMM_UART
    returnValue = grblSerial.read();
#else
    returnValue = grblUSBSerial.read();
#endif

    return returnValue;
}

int GrblCommAvailable()
{
    int returnValue;

#ifdef GRBL_COMM_UART
    returnValue = grblSerial.available();
#else
    returnValue = grblUSBSerial.available();
#endif

    return returnValue;
}

void USBDeviceCheck(uint32_t usbbaud, uint32_t usbformat)
{
#ifdef GRBL_COMM_USB

#ifdef USB_COMM_DEBUG
    //	Serial.println("Enter USBDeviceCheck");
#endif

    grblUSB.Task();

    // Print out information about different devices.
    for (uint8_t i = 0; i < CNT_DEVICES; i++)
    {
        if (*drivers[i] != driver_active[i])
        {
            if (driver_active[i])
            {
#ifdef USB_COMM_DEBUG
                Serial.printf("*** Device %s - disconnected ***\n", driver_names[i]);
#endif
                driver_active[i] = false;
            }
            else
            {
#ifdef USB_COMM_DEBUG
                Serial.printf("*** Device %s %x:%x - connected ***\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
#endif
                driver_active[i] = true;

#ifdef USB_COMM_DEBUG
                const uint8_t* psz = drivers[i]->manufacturer();
                if (psz && *psz)
                {
                    Serial.printf("  manufacturer: %s\n", psz);
                }
#endif

#ifdef USB_COMM_DEBUG
                psz = drivers[i]->product();
                if (psz && *psz)
                {
                    Serial.printf("  product: %s\n", psz);
                }
#endif

#ifdef USB_COMM_DEBUG
                psz = drivers[i]->serialNumber();
                if (psz && *psz)
                {
                    Serial.printf("  Serial: %s\n", psz);
                }
#endif

                // If this is a new Serial device.
                if (drivers[i] == &grblUSBSerial)
                {
                    // Lets try first outputting something to our USerial to see if it will go out...
                    grblUSBSerial.begin(usbbaud, usbformat);
                }
            }
        }
#ifdef USB_COMM_DEBUG
        else
        {
            Serial.printf("  Driver %d, %d\n", i, driver_active[i]);
        }
#endif
    }
#endif
}