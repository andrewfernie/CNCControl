// 
// 
// 
#include "Encoder2.h"

CEncoder2::CEncoder2(uint8_t pin1, uint8_t pin2, int8_t divider) :Encoder{ pin1, pin2 }
{
	m_pinSwitch = 0;
	m_enableSwitch = false;
	m_divider = divider;
	Init();
}

CEncoder2::CEncoder2(uint8_t pin1, uint8_t pin2, int8_t divider, uint8_t pin_sw) : Encoder{ pin1, pin2 }
{
	m_pinSwitch = pin_sw;
	m_enableSwitch = true;
	m_divider = divider;
	pinMode(pin_sw, INPUT);      // sets the encoder select digital pin
	Init();
}

void CEncoder2::Init()
{
	SetScaleOffset(1.0, 0.0);
	SetMinMaxPosition(-999999, 999999);
	Reset();
}

//--------------------------------------------------------------------------------------
// GetPosition()
//--------------------------------------------------------------------------------------
long CEncoder2::GetPosition()
{
	m_lastCount = m_count;

	m_count = read() / m_divider;

	if (m_count > m_maximumPosition)
	{
		m_count = m_maximumPosition;
		write(m_count * m_divider);
	}
	else if (m_count < m_minimumPosition)
	{
		m_count = m_minimumPosition;
		write(m_count * m_divider);
	}

	if ((m_count - m_lastCount) > m_directionThreshold)
		m_direction = 1;
	else if ((m_count - m_lastCount) < -m_directionThreshold)
		m_direction = -1;
	else
		m_direction = 0;

	return m_count;
}

//--------------------------------------------------------------------------------------
// GetLastPosition()
//--------------------------------------------------------------------------------------
long CEncoder2::GetLastPosition()
{
	return m_lastCount;
}

//--------------------------------------------------------------------------------------
// GetLastScaledPosition()
//--------------------------------------------------------------------------------------
float CEncoder2::GetLastScaledPosition()
{
	return ((float)m_lastCount * m_scale) + m_offset;
}

//--------------------------------------------------------------------------------------
// GetScaledPosition()
//--------------------------------------------------------------------------------------
float CEncoder2::GetScaledPosition()
{
	return ((float)m_count * m_scale) + m_offset;
}

//--------------------------------------------------------------------------------------
// ReadJogEncoderSwitch(()
//--------------------------------------------------------------------------------------
bool CEncoder2::ReadSwitch()
{
	bool sw = false;

	if (m_enableSwitch)
	{
		sw = !digitalRead(m_pinSwitch);
	}
	return sw;
}

void CEncoder2::Reset()
{
	write(0);
	m_count = 0;
	m_lastCount = 0;
	m_direction = 0;
	m_scaledPosition = m_offset;
	m_lastScaledPosition = m_offset;
}

int8_t CEncoder2::GetDirection()
{
	return m_direction;
}

void CEncoder2::SetMinMaxPosition(long minPos, long maxPos)
{
	m_minimumPosition = minPos;
	m_maximumPosition = maxPos;
}

void CEncoder2::SetScaleOffset(float scale, float offset)
{
	m_scale = scale;
	m_offset = offset;
}