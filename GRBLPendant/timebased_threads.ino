//=========================================================
//Project: GRBL Pendant
//Module:  timebased_threads.ino         
//=========================================================
//
// Author: Andrew Fernie
// Source code freely released - do with it what you like!
//
//----------------------------------------------------------
// This code started from the XLCD project by Frank Herrmann
//----------------------------------------------------------


//
//	 *************************************
//   Thread to get buttons
//	 *************************************
//
void simpleThread_setup(readButtons)
{
	// output
}

boolean simpleThread_loop(readButtons)
{

	return false;
}


//
//	*************************************
//	Thread to send a '?' to
//	get GRBL position informations
//	*************************************
//
void simpleThread_setup(getPositions)
{
	// output
}

boolean simpleThread_loop(getPositions)
{

	// Test when called last time and if this time less the interval time,
	if ((millis() - lastCall) < simpleThread_dynamic_getLoopTime(getPositions))
	{
		return false;
	}

	// output counter value
	grblSerial.print('?');

#if defined(DEBUG)
	Serial.print(F("<Free "));
	Serial.print(freeMem());
	Serial.println(F(">"));
#endif

	return false;
}

//
//	*************************************
//	Thread to send a '$G' to
//	get GRBL position informations
//	*************************************
//
void simpleThread_setup(getStates)
{
	// output
}

boolean simpleThread_loop(getStates)
{
	// output counter value
	grblSerial.print("$G\n");

	return false;
}

uint32_t freeMem()
{ 
	// for Teensy 3.2
	uint32_t stackTop;
	uint32_t heapTop;

	// current position of the stack.
	stackTop = (uint32_t)& stackTop;

	// current position of heap.
	void* hTop = malloc(1);
	heapTop = (uint32_t)hTop;
	free(hTop);

	// The difference is (approximately) the free, available ram.
	return stackTop - heapTop;
}