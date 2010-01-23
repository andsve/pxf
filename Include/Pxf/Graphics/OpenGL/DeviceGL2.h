#ifndef _PXF_GRAPHICS_DEVICEGL2_H_
#define _PXF_GRAPHICS_DEVICEGL2_H_

#if defined(CONF_FAMILY_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef CreateFont // ...
#endif

#if defined(CONF_PLATFORM_MACOSX)
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/OpenGL/WindowGL2.h>

namespace Pxf{
	namespace Graphics {

		class QuadBatch;
		class VertexBuffer;

		class DeviceGL2 : public Device
		{
		public:
			DeviceGL2();
			virtual ~DeviceGL2();

			Window* OpenWindow(WindowSpecifications* _pWindowSpecs);
			void CloseWindow();

			DeviceType GetDeviceType() { return EOpenGL2; }

			void SwapBuffers();

			QuadBatch* CreateQuadBatch(int _maxSize);

			VertexBuffer* CreateVertexBuffer(VertexBufferLocation _VertexBufferLocation, VertexBufferType _VertexBufferType);
			void DestroyVertexBuffer(VertexBuffer* _pVertexBuffer);
			void DrawVertexBuffer(VertexBuffer* _pVertexBuffer);
		private:
			Window* m_Window;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGL2_H_
