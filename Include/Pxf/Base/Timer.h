#ifndef __PXF_BASE_TIMER_H__
#define __PXF_BASE_TIMER_H__

#include <Pxf/Base/Types.h>

namespace Pxf
{
	class Timer
	{
	private:
		uint64 m_Start;
		uint64 m_Stop;
	public:
		Timer();
		void Start();
		void Stop();
		uint64 Interval();
	};
}

#endif //__PXF_BASE_TIMER_H__

