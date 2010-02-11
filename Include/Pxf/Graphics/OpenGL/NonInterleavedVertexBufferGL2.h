#ifndef _PXF_GRAPHICS_NONINTERLEAVEDVERTEXBUFFERGL2_VA_H_
#define _PXF_GRAPHICS_NONINTERLEAVEDVERTEXBUFFERGL2_VA_H_

#include <Pxf/Graphics/NonInterleavedVertexBuffer.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Device.h> // VertexBufferType

// http://www.songho.ca/opengl/gl_vertexarray.html
// http://www.songho.ca/opengl/gl_vbo.html

namespace Pxf
{
	namespace Graphics
	{
		//! Abstract class for vertex buffer
		class NonInterleavedVertexBufferGL2 : public NonInterleavedVertexBuffer
		{
		public:
			NonInterleavedVertexBufferGL2(Device* _pDevice, VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag);
			virtual ~NonInterleavedVertexBufferGL2();

			void SetData(VertexBufferAttribute _AttribType, unsigned _TypeSize, const void* _Ptr, unsigned _Count, unsigned _Stride = 0);
			bool Commit();

			void _PreDraw();
			void _PostDraw();
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_NONINTERLEAVEDVERTEXBUFFERGL2_VA_H_