#ifndef _PXF_GRAPHICS_VERTEXBUFFERGL2_VA_H_
#define _PXF_GRAPHICS_VERTEXBUFFERGL2_VA_H_

#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Device.h> // VertexBufferType

// http://www.songho.ca/opengl/gl_vertexarray.html
// http://www.songho.ca/opengl/gl_vbo.html

namespace Pxf
{
	namespace Graphics
	{
		//! Abstract class for vertex buffer
		class VertexBufferGL2_VA : public VertexBuffer
		{
		private:
			void PreDraw();
			void PostDraw();

		public:
			VertexBufferGL2_VA(VertexBufferType _VertexBufferType);
			~VertexBufferGL2_VA();

			void SetData(VertexBufferAttribute _AttribType, unsigned _TypeSize, const void* _Ptr, unsigned _Count, unsigned _Stride = 0);
			bool Commit();
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_VERTEXBUFFERGL2_VA_H_