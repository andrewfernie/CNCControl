#pragma once
//
//     Error Code Descriptions From GRBL Documentation
//
// ID	Error Code Description
// 1	G - code words consist of a letter and a value.Letter was not found.
// 2	Numeric value format is not valid or missing an expected value.
// 3	Grbl '$' system command was not recognized or supported.
// 4	Negative value received for an expected positive value.
// 5	Homing cycle is not enabled via settings.
// 6	Minimum step pulse time must be greater than 3usec
// 7	EEPROM read failed.Reset and restored to default values.
// 8	Grbl '$' command cannot be used unless Grbl is IDLE.Ensures smooth operation during a job.
// 9	G - code locked out during alarm or jog state
// 10	Soft limits cannot be enabled without homing also enabled.
// 11	Max characters per line exceeded.Line was not processed and executed.
// 12	(Compile Option) Grbl '$' setting value exceeds the maximum step rate supported.
// 13	Safety door detected as opened and door state initiated.
// 14	(Grbl - Mega Only) Build info or startup line exceeded EEPROM line length limit.
// 15	Jog target exceeds machine travel.Command ignored.
// 16	Jog command with no '=' or contains prohibited g - code.
// 17	Laser mode requires PWM output.
// 20	Unsupported or invalid g - code command found in block.
// 21	More than one g - code command from same modal group found in block.
// 22	Feed rate has not yet been set or is undefined.
// 23	G - code command in block requires an integer value.
// 24	Two G - code commands that both require the use of the XYZ axis words were detected in the block.
// 25	A G - code word was repeated in the block.
// 26	A G - code command implicitly or explicitly requires XYZ axis words in the block, but none were detected.
// 27	N line number value is not within the valid range of 1 - 9, 999, 999.
// 28	A G - code command was sent, but is missing some required P or L value words in the line.
// 29	Grbl supports six work coordinate systems G54 - G59.G59.1, G59.2, and G59.3 are not supported.
// 30	The G53 G - code command requires either a G0 seek or G1 feed motion mode to be active.A different motion was active.
// 31	There are unused axis words in the block and G80 motion mode cancel is active.
// 32	A G2 or G3 arc was commanded but there are no XYZ axis words in the selected plane to trace the arc.
// 33	The motion command has an invalid target.G2, G3, and G38.2 generates this error, if the arc is impossible to generate or if the probe target is the current position.
// 34	A G2 or G3 arc, traced with the radius definition, had a mathematical error when computing the arc geometry.Try either breaking up the arc into semi - circles or quadrants, or redefine them with the arc offset definition.
// 35	A G2 or G3 arc, traced with the offset definition, is missing the IJK offset word in the selected plane to trace the arc.
// 36	There are unused, leftover G - code words that aren't used by any command in the block.
// 37	The G43.1 dynamic tool length offset command cannot apply an offset to an axis other than its configured axis.The Grbl default axis is the Z - axis.
// 38	Tool number greater than max supported value.

const int error_num_min = 1;
const int error_num_max = 38;
const char error_message[][17] =
{
	//0  
	"Not an error    ",
	//1	
	"GC Ltr not fnd  ",
	//2	
	"Num val invalid ",
	//3	
	"$ sys cmd invld ",
	//4	
	"Expctd +'ve val.",
	//5	
	"Hom cy not enbld",
	//6	
	"Min stp pls <3us",
	//7	
	"EEPROM rd failed",
	//8	
	"$ nds Grbl IDLE ",
	//9	
	"Alm or jog state",
	//10
	"Home not enbld. ",
	//11
	"Too many chars. ",
	//12
	"Stp rte too high",
	//13
	"Sfty door open  ",
	//14 (Grbl-Mega Only) 
	"EEPROM line lnth",
	//15
	"Jog exceed limit",
	//16
	"Invalid Jog cmd ",
	//17
	"Lsr need PWM o/p",
	//18
	"Not an error    ",
	//19
	"Not an error    ",
	//20
	"Invalid GC cmd  ",
	//21
	"Cmd modal group ",
	//22
	"Fd rate undef   ",
	//23
	"GC nds int val. ",
	//24
	"GC require XYZ 1",
	//25
	"GC word repeated",
	//26
	"GC require XYZ 2",
	//27
	"line num invld  ",
	//28
	"GC needs P or L ",
	//29
	"G54-G59,.1,.2,.3",
	//30
	"GC nds G0 or G1 ",
	//31
	"Unused axis wrds",
	//32
	"No XYZ axis wrds",
	//33
	"Arc invld target",
	//34
	"Arc math error  ",
	//35
	"Arc miss. IJK   ",
	//36
	"Unused GC words ",
	//37
	"The G43.1 error ",
	//38
	"Tool number>MAX "
};

//
//     Alarm Code Descriptions From GRBL Documentation
//
// ID	Alarm Code Description
// 1	Hard limit triggered. Machine position is likely lost due to sudden and immediate halt. Re-homing is highly recommended.
// 2	G-code motion target exceeds machine travel. Machine position safely retained. Alarm may be unlocked.
// 3	Reset while in motion. Grbl cannot guarantee position. Lost steps are likely. Re-homing is highly recommended.
// 4	Probe fail. The probe is not in the expected initial state before starting probe cycle, where G38.2 and G38.3 is not triggered and G38.4 and G38.5 is triggered.
// 5	Probe fail. Probe did not contact the workpiece within the programmed travel for G38.2 and G38.4.
// 6	Homing fail. Reset during active homing cycle.
// 7	Homing fail. Safety door was opened during active homing cycle.
// 8	Homing fail. Cycle failed to clear limit switch when pulling off. Try increasing pull-off setting or check wiring.
// 9	Homing fail. Could not find limit switch within search distance. Defined as 1.5 * max_travel on search and 5 * pulloff on locate phases.

const int alarm_num_min = 1;
const int alarm_num_max = 9;
const char alarm_message[][17] =
{
	//0	
	"Not an alarm    ",
	//1	
	"Hard limit trig ",
	//2	
	"GC > machn trvl ",
	//3	
	"Reset in motion ",
	//4	
	"Probe fail. 1   ",
	//5	
	"Probe fail. 2   ",
	//6	
	"Homing fail 1   ",
	//7	
	"Homing fail 2   ",
	//8	
	"Homing fail 3   ",
	//9	
	"Homing fail 4   "
};