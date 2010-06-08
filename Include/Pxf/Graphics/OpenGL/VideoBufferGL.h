#ifndef _PXF_GRAPHICS_VIDEOBUFFERGL_H_
#define _PXF_GRAPHICS_VIDEOBUFFERGL_H_

#include <Pxf/Graphics/VideoBuffer.h>

namespace Pxf
{
	namespace Graphics
	{	
		struct VideoBufferGL : VideoBuffer
		{
			unsigned int 	m_Handle;
			int				m_Format;
			int				m_Target;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_RENDERBUFFER_H_