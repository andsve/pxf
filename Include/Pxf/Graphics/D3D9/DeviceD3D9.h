#ifndef _PXF_GRAPHICS_DEVICED3D9_H_
#define _PXF_GRAPHICS_DEVICED3D9_H_

#ifdef CONF_FAMILY_WINDOWS
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/D3D9/WindowD3D.h>
#include <windows.h>
#include <d3dx9.h>

namespace Pxf{
	namespace Graphics {
		class Window;
		class WindowSpecifications;

		class DeviceD3D9 : public Device
		{
		public:
			DeviceD3D9();
			~DeviceD3D9();
			
			Window* OpenWindow(WindowSpecifications* _pWindowSpecs);
			void CloseWindow();

			Graphics::DeviceType GetDeviceType() { return EDirect3D9; }
			void SwapBuffers();
			/*
			VertexBuffer* CreateVertexBuffer();
			void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer);
			void DrawVertexBuffer(VertexBuffer* _pVertexBuffer);
			*/

		private:
			WindowD3D*				m_Window;
			IDirect3D9*				m_D3D9;
			IDirect3DDevice9*		m_D3D9_device;
		};

	} // Graphics
} // Pxf

#endif // CONF_FAMILY_WINDOWS

#endif //_PXF_GRAPHICS_DEVICED3D9_H_
