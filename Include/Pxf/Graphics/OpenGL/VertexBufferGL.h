#ifndef _PXF_GRAPHICS_VERTEXBUFFERGL_H_
#define _PXF_GRAPHICS_VERTEXBUFFERGL_H_

#include <Pxf/Graphics/VertexBuffer.h>
#include <Pxf/Graphics/DeviceType.h>
#include <GL/glew.h>
#include <GL/glfw.h>

namespace Pxf
{
	namespace Graphics
	{
		//! Abstract class for vertex buffer
		class VertexBufferGL : VertexBuffer
		{
			private:
				bool				m_IsLocked;		// mutex lock indicator
				bool				m_Bound;		// is VBO already bound?
				unsigned int		m_Stride;		// vertex data offset
				unsigned int		m_VCount;		// vertex count?
				void*				m_VBuffer;		// VBO data

				GLuint	m_VBOID;		// VBO handle
				GLenum	m_UsageFlag;	// Usage flag indicates how the data will be written 
				GLint	m_PrimitiveMode;// Indicates how the VBO is drawn

				bool CreateVBO();
				bool DestroyVBO();

			public:
				VertexBufferGL(void* _Data,unsigned int _Offset,unsigned int _VerticeCount,GLenum _Usage);
				~VertexBufferGL();

				virtual VertexBuffer& Lock();
				virtual VertexBuffer& Unlock();
				//virtual bool Fill(float* _Data,int _Stride,int _Size); 
				virtual void SetPrimitive(PrimitiveType _PrimitiveType);
				virtual PrimitiveType GetPrimitive(); 
				virtual DeviceType GetDeviceType() { return EOpenGL2; }
				virtual bool IsLocked() {return m_IsLocked; } 

				// OpenGL-specific:
				void Bind();
				void Unbind();
				bool IsBound() { return m_Bound; }
				int GetStride() { return m_Stride; }
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_VERTEXBUFFERGL_H_