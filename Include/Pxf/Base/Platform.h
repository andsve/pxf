#ifndef __PXF_BASE_PLATFORM_H__
#define __PXF_BASE_PLATFORM_H__

#include <Pxf/Base/Types.h>
#include <Pxf/Base/Config.h>

namespace Pxf
{
	class Platform
	{
	private:
		static bool s_TimeInitialized;
		static int64 m_Frequency;
		static int64 m_Timestamp;
		static void Initialize();
	public:
		Platform()
		{
			if (!s_TimeInitialized)
			{
				Initialize();
				s_TimeInitialized = true;
			}
		}
		
		static int64 GetTime();
	};
}

#endif //__PXF_BASE_PLATFORM_H__

