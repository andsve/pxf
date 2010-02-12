#ifndef _PXF_GRAPHICS_DEVICERESOURCE_
#define _PXF_GRAPHICS_DEVICERESOURCE_

namespace Pxf {
namespace Graphics {
	
	class Device;
	
	class DeviceResource
	{
	protected:
		Device *m_pDevice;
	public:
		DeviceResource(Device *_pDevice)
		{
			m_pDevice = _pDevice;
		}
	};

}}

#endif // _PXF_GRAPHICS_DEVICERESOURCE_H_