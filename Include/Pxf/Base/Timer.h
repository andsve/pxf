#ifndef __PXF_BASE_TIMER_H__
#define __PXF_BASE_TIMER_H__

#include <Pxf/Base/Types.h>

namespace Pxf
{
	inline uint64 RDTSC(void)
	{
	  _asm  _emit 0x0F
	  _asm  _emit 0x31
	}

	//! Abstract class used for timers
	class Timer
	{
	public:
		virtual inline void Start() = pure;
		virtual inline void Stop()	= pure;
		virtual uint64 Interval() = pure;
	}; // Timer

	class PCTimer : Timer
	{
	private:
		uint64 m_StartTime;
		uint64 m_StopTime;
	public:
		inline void Start();
		inline void Stop();
		uint64 Interval();
	}; // PerformanceCounter

	class RDTSCTimer : Timer
	{
	private:
		uint64 m_StartCycle;
		uint64 m_EndCycle;
	public:
		inline void Start();
		inline void Stop();
		uint64 Interval();
	}; // RDTSCTimer
}

#endif //__PXF_BASE_TIMER_H__

