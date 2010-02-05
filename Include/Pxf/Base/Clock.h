#ifndef __PXF_BASE_CLOCK_H__
#define __PXF_BASE_CLOCK_H__

#include <Pxf/Base/Types.h>
#include <Pxf/Base/Config.h>

namespace Pxf
{
	class Clock
	{
	private:
		static bool s_Initialized;
		static int64 m_Frequency;
		static int64 m_Timestamp;
		static int64 m_Starttime;
		static void Initialize();
	public:
		Clock()
		{
			if (!s_Initialized)
			{
				Initialize();
				s_Initialized = true;
			}
		}
		
		static int64 GetTime();
	};
}

#endif //__PXF_BASE_CLOCK_H__

