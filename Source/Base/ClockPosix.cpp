#include <Pxf/Base/Clock.h>
#ifdef CONF_FAMILY_UNIX
#include <sys/time.h> 

using namespace Pxf;

bool Clock::s_Initialized = false;
int64 Clock::m_Frequency = -1;
int64 Clock::m_Timestamp = 0;

void Clock::Initialize()
{
	struct timeval timeVal;
	gettimeofday(&timeVal, NULL);
	m_Timestamp = (int64)timeVal.tv_sec * (int64)1000000 + (int64)timeVal.tv_usec;
	m_Frequency = 1e-6;
}

int64 Clock::GetTime()
{
	struct timeval timeVal;
	gettimeofday(&timeVal, NULL);
	int64 newTimeStamp = (int64)timeVal.tv_sec * (int64)1000000 + (int64)timeVal.tv_usec;
	return (newTimeStamp - m_Timestamp) / (int64)1000;	
}

#endif // CONF_FAMILY_UNIX