#ifndef _PXF_GRAPHICS_DEVICED3D9_H_
#define _PXF_GRAPHICS_DEVICED3D9_H_

#ifdef CONF_FAMILY_WINDOWS

#include <Pxf/Graphics/Device.h>
#include <windows.h>
#include <d3dx9.h>

namespace Pxf{
	namespace Graphics {

		class DeviceD3D9 : public Device
		{
		public:
			DeviceD3D9();
			~DeviceD3D9();
		private:
			IDirect3D9* m_D3D9;
			IDirect3DDevice9 *m_D3D_device;
			D3DPRESENT_PARAMETERS m_pp;
		};

	} // Graphics
} // Pxf

#endif // CONF_FAMILY_WINDOWS

#endif //_PXF_GRAPHICS_DEVICED3D9_H_
