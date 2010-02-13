#ifndef _PXF_GRAPHICS_DEVICERESOURCE_
#define _PXF_GRAPHICS_DEVICERESOURCE_

#include <Pxf/Base/Types.h>

namespace Pxf {
	namespace Resource {
		class AbstractResource;
	}

namespace Graphics {
	
	class Device;
	
	class DeviceResource
	{
	protected:
		static unsigned GLOBAL_RESOURCE_COUNTER;
		uint m_ID;
		Device *m_pDevice;		        
		Resource::AbstractResource* m_Resource;

	public:
		DeviceResource(Device *_pDevice, Resource::AbstractResource* _Resource = 0)
		{
	    	m_Resource = _Resource;
			m_pDevice = _pDevice;
			m_ID = GLOBAL_RESOURCE_COUNTER;
			GLOBAL_RESOURCE_COUNTER++;
		}

		inline Resource::AbstractResource* GetResource()
		{
			return m_Resource;
		}

		inline uint GetID()
		{
			return m_ID;
		}
	};
}}

#endif // _PXF_GRAPHICS_DEVICERESOURCE_H_