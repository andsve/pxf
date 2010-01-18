#ifndef _PXF_GRAPHICS_DEVICETYPE_H_
#define _PXF_GRAPHICS_DEVICETYPE_H_

namespace Pxf
{
	namespace Graphics
	{
		enum DeviceType
		{
			EOpenGL2,
			EOpenGL3,
			EDirect3D9,
			EDirect3D10,
			EDirect3D11
		};
		
		const char* DeviceTypeName(DeviceType _deviceType);
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_DEVICETYPE_H_