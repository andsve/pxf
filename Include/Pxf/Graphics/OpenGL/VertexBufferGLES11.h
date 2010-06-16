#ifndef _PXF_GRAPHICS_OPENGLES_VERTEXBUFFERGLES11_H_
#define _PXF_GRAPHICS_OPENGLES_VERTEXBUFFERGLES11_H_

#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Graphics/DeviceType.h>
#include <Pxf/Graphics/Device.h> // VertexBufferType

namespace Pxf
{
	namespace Graphics
	{
		//! Abstract class for vertex buffer
		class VertexBufferGLES11 : public VertexBuffer
		{
		private:
			unsigned m_BufferObjectId;
		public:
			VertexBufferGLES11(Device* _pDevice, VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag);
			virtual ~VertexBufferGLES11();

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

#endif // _PXF_GRAPHICS_OPENGL2_VERTEXBUFFERGL2_H_