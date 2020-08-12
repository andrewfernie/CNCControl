// Encoder2.h

#ifndef _ENCODER2_h
#define _ENCODER2_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <Encoder.h>
#include <Bounce2.h>

class CEncoder2 :Encoder
{
protected:
	long m_count;
	long m_lastCount;
	int8_t m_direction;
	float m_scaledPosition;
	float m_lastScaledPosition;
	uint8_t m_pinSwitch;
	uint8_t m_enableSwitch;
	float m_scale;
	float m_offset;

	const int8_t m_directionThreshold = 5;

public:
	CEncoder2(uint8_t pin1, uint8_t pin2);
	CEncoder2(uint8_t pin1, uint8_t pin2, uint8_t pin_sw);
	void Init();
	long GetPosition();
	long GetLastPosition();
	float GetScaledPosition();
	float GetLastScaledPosition();
	void SetScaledPosition(float pos);
	void SetLastScaledPosition(float pos);
	int8_t GetDirection();
	bool ReadSwitch();
	void Reset();
};

#endif
