#ifndef _PXF_GRAPHICS_DEVICEGL2_H_
#define _PXF_GRAPHICS_DEVICEGL2_H_

#include <Pxf/Graphics/OpenGL/OpenGL.h>
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

			void SetViewport(int _x, int _y, int _w, int _h);
			void SetProjection(Math::Mat4 *_matrix);
			void SwapBuffers();

			Texture* CreateTexture(const char* _filepath);
			void BindTexture(Texture* _texture);
			void BindTexture(Texture* _texture, unsigned int _texture_unit);

			QuadBatch* CreateQuadBatch(int _maxSize);

			NonInterleavedVertexBuffer* CreateNonInterleavedVertexBuffer(VertexBufferLocation _VertexBufferLocation);
			InterleavedVertexBuffer* CreateInterleavedVertexBuffer(VertexBufferLocation _VertexBufferLocation);
			void DestroyVertexBuffer(NonInterleavedVertexBuffer* _pVertexBuffer);
			void DestroyVertexBuffer(InterleavedVertexBuffer* _pVertexBuffer);
			void DrawBuffer(InterleavedVertexBuffer* _pVertexBuffer);
		private:
			Window* m_Window;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_DEVICEGL2_H_
