#ifndef _PXF_GRAPHICS_DEVICE_H_
#define _PXF_GRAPHICS_DEVICE_H_

#include <Pxf/Graphics/DeviceType.h>

namespace Pxf
{

	namespace Graphics
	{
		class Window;
		class WindowSpecifications;
		
		class VertexBuffer;

		class PrimitiveBatch;
		
		//! Abstract video device
		class Device
		{
		public:

			// Windowing
			virtual Window* OpenWindow(WindowSpecifications* _pWindowSpecs) = 0;
			virtual void CloseWindow() = 0;

			// Device
			virtual DeviceType GetDeviceType() = 0;

			// Graphics
			virtual void SwapBuffers() = 0;

			// PrimitiveBatch
			void PrimitiveBatch* PrimitiveBatch() { return m_PrimitiveBatch; }
			
			/*virtual VertexBuffer* CreateVertexBuffer() = 0;
			virtual void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;
			virtual void DrawVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;*/
		protected:
			PrimitiveBatch* m_PrimitiveBatch;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_DEVICE_H_