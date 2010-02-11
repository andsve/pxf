#include <Pxf/Base/Timer.h>
#include <Pxf/Base/Platform.h>

using namespace Pxf;

Timer::Timer()
{
	// make sure it Platform is initialized
	Platform p;
	m_Start = 0;
	m_Stop = 0;
}

void Timer::Start()
{
	m_Start = Platform::GetTime();
}


void Timer::Stop()
{
	m_Stop = Platform::GetTime();
}

uint64 Timer::Interval()
{
	return m_Stop - m_Start;
}
