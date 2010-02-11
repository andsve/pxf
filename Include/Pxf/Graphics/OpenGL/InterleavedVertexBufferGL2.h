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
		private:
			unsigned m_BufferObjectId;
		public:
			InterleavedVertexBufferGL2(Device* _pDevice, VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag);
			virtual ~InterleavedVertexBufferGL2();

			void CreateNewBuffer(uint32 _NumVertices, uint32 _VertexSize);
			void CreateFromBuffer(void* _Buffer,uint32 _NumVertices, uint32 _VertexSize); 

			void UpdateData(void* _Buffer, uint32 _Count, uint32 _Offset);

			void* MapData(VertexBufferAccessFlag _AccessFlag);
			void UnmapData();

			void _PreDraw();
			void _PostDraw();
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFERGL2_H_