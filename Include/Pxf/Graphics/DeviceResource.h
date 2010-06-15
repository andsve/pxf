#ifndef _PXF_GRAPHICS_DEVICERESOURCE_
#define _PXF_GRAPHICS_DEVICERESOURCE_

#include <Pxf/Base/Types.h>
#include <Pxf/Resource/AbstractResource.h>

namespace Pxf {

namespace Graphics {
	class Device;
	
	class DeviceResource
	{
	protected:
		static unsigned GLOBAL_RESOURCE_COUNTER;
		uint m_ID;
		Pxf::Graphics::Device *m_pDevice;		        
		Resource::AbstractResource* m_Resource;

	public:
		DeviceResource(Device *_pDevice, Resource::AbstractResource* _Resource = 0);

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