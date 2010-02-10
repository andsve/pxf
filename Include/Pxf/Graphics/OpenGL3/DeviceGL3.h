#ifndef _PXF_GRAPHICS_DEVICEGL3_H_
#define _PXF_GRAPHICS_DEVICEGL3_H_

#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/Window.h>
//#include <windows.h>

namespace Pxf{
	namespace Graphics {

		class QuadBatch;

		class DeviceGL3 : public Device
		{
		public:
			DeviceGL3();
			~DeviceGL3();

			Window* OpenWindow(WindowSpecifications* _pWindowSpecs);
			void CloseWindow();

			DeviceType GetDeviceType() { return EOpenGL3; }

			void SwapBuffers();

			//QuadBatch* CreateQuadBatch(int _maxSize);

			/*virtual VertexBuffer* CreateVertexBuffer() = 0;
			virtual void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;
			virtual void DrawVertexBuffer(VertexBuffer* _pVertexBuffer) = 0;*/
		private:
			Window* m_Window;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGL3_H_
