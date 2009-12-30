#include <Pxf/Base/Clock.h>
#ifdef CONF_FAMILY_WINDOWS

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h> // QueryPerformanceCounter et al

using namespace Pxf;

bool Clock::s_Initialized = false;
int64 Clock::m_Frequency = -1;
int64 Clock::m_Timestamp = 0;

void Clock::Initialize()
{
	int64 freq = 0;
	if (QueryPerformanceFrequency((LARGE_INTEGER*) &freq))
	{
		m_Frequency = freq;
		QueryPerformanceCounter((LARGE_INTEGER*) &m_Timestamp);
	}
	else
	{
		m_Frequency = -1;
		// TODO: Error or fallback on timeGetTime()
	}
}

int64 Clock::GetTime()
{
	int64 newTimeStamp;
	QueryPerformanceCounter((LARGE_INTEGER*) &newTimeStamp);
	
	return ((newTimeStamp - m_Timestamp) * 1000) / m_Frequency;	
}

#endif // CONF_FAMILY_WINDOWS