#ifndef _PXF_GRAPHICS_DEVICE_H_
#define _PXF_GRAPHICS_DEVICE_H_

namespace Pxf
{
	namespace Graphics
	{
		class Window;
		class WindowSpecifications;
		
		class VertexBuffer;
		
		//! Abstract video device
		class Device
		{
		public:
			virtual Window* OpenWindow(WindowSpecifications* _pWindowSpecs) = 0;
			virtual void CloseWindow() = 0;
			
			/*virtual VertexBuffer* CreateVertexBuffer() = 0;
			virtual void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;
			virtual void DrawVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;*/
			
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_DEVICE_H_