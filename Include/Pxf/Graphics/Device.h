#ifndef _PXF_GRAPHICS_DEVICE_H_
#define _PXF_GRAPHICS_DEVICE_H_

#include <Pxf/Graphics/DeviceType.h>

namespace Pxf
{
	namespace Input { class Input; }

	namespace Graphics
	{
		class Window;
		class WindowSpecifications;
		
		class VertexBuffer;
		
		//! Abstract video device
		class Device
		{
		public:

			// Windowing
			virtual Window* OpenWindow(WindowSpecifications* _pWindowSpecs) = 0;
			virtual void CloseWindow() = 0;

			// Input
			virtual Input::Input* GetInput() = 0;

			// Device
			virtual DeviceType GetDeviceType() = 0;

			// Graphics
			virtual void SwapBuffers() = 0;
			
			/*virtual VertexBuffer* CreateVertexBuffer() = 0;
			virtual void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;
			virtual void DrawVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;*/
			
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_DEVICE_H_