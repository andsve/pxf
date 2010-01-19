#ifndef _PXF_GRAPHICS_DEVICEGL2_H_
#define _PXF_GRAPHICS_DEVICEGL2_H_

#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/OpenGL/WindowGL.h>
#include <windows.h>
#include <d3dx9.h>

namespace Pxf{
	namespace Graphics {

		class DeviceGL2 : public Device
		{
		public:
			DeviceGL2();
			~DeviceGL2();

			Window* OpenWindow(WindowSpecifications* _pWindowSpecs);
			void CloseWindow();

			DeviceType GetDeviceType() { return DeviceType::EOpenGL2; }

			/*virtual VertexBuffer* CreateVertexBuffer() = 0;
			virtual void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;
			virtual void DrawVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;*/
		private:
			Window* m_Window;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGL2_H_
