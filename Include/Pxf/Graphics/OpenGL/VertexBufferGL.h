#ifndef _PXF_GRAPHICS_VERTEXBUFFERGL_H_
#define _PXF_GRAPHICS_VERTEXBUFFERGL_H_

#include <Pxf/Graphics/VertexBuffer.h>
#include <GL/glfw.h>

namespace Pxf
{
	namespace Graphics
	{
		//! Abstract class for vertex buffer
		class VertexBufferGL : VertexBuffer
		{
			private:
				bool	m_IsLocked;		// mutex lock indicator
				int		m_Stride;		// vertex data offset
				void*	m_VBuffer;		// VBO data

				GLuint*	m_VBOID;		// VBO handle
				GLenum	m_UsageFlag;	// Usage flag indicates how the data will be written 
				

			public:
				VertexBufferGL(void* _Data,int _Offset,GLenum _Usage);
				~VertexBufferGL();

				virtual VertexBuffer& Lock();
				virtual VertexBuffer& Unlock();
				virtual bool IsLocked(); 
				
				virtual void SetPrimitive(PrimitiveType _PrimitiveType);
				virtual PrimitiveType GetPrimitive();
				virtual bool Fill(float* _Data,int _Stride,int _Size);

				// OpenGL-specific:
				void Bind();
				void Unbind();
				int GetStride();
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_VERTEXBUFFERGL_H_