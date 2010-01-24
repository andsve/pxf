#ifndef _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFERGL2_H_
#define _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFERGL2_H_

#include <Pxf/Graphics/InterleavedVertexBuffer.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Device.h> // VertexBufferType

// http://www.songho.ca/opengl/gl_vertexarray.html
// http://www.songho.ca/opengl/gl_vbo.html

namespace Pxf
{
	namespace Graphics
	{
		//! Abstract class for vertex buffer
		class InterleavedVertexBufferGL2 : public InterleavedVertexBuffer
		{
		public:
			InterleavedVertexBufferGL2(VertexBufferType _VertexBufferType);
			~InterleavedVertexBufferGL2();

			void SetData(VertexBufferAttribute _AttribType, unsigned _TypeSize, const void* _Ptr, unsigned _Count, unsigned _Stride = 0);
			bool Commit();

			void _PreDraw();
			void _PostDraw();
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFERGL2_H_