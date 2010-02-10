#include <Pxf/Base/Platform.h>
#ifdef CONF_FAMILY_WINDOWS

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h> // QueryPerformanceCounter et al

using namespace Pxf;

bool Platform::s_TimeInitialized = false;
int64 Platform::m_Timestamp = 0;
int64 Platform::m_Frequency = -1;

void Platform::Initialize()
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

int64 Platform::GetTime()
{
	int64 newTimeStamp;
	QueryPerformanceCounter((LARGE_INTEGER*) &newTimeStamp);
	
	return ((newTimeStamp - m_Timestamp) * 1000) / m_Frequency;	
}

#endif // CONF_FAMILY_WINDOWS