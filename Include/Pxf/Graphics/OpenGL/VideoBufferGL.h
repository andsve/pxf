#ifndef _PXF_GRAPHICS_RENDERBUFFER_H_
#define _PXF_GRAPHICS_RENDERBUFFER_H_

namespace Pxf
{
	namespace Graphics
	{	
		struct RenderBufferGL
		{
			GLuint 	m_Handle;
			int 	m_Width;
			int 	m_Height;
			int 	m_Format;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_RENDERBUFFER_H_