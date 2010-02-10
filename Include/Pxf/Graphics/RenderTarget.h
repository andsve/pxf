#ifndef _PXF_GRAPHICS_RENDERTARGET_H_
#define _PXF_GRAPHICS_RENDERTARGET_H_

namespace Pxf
{
	namespace Graphics
	{		
		enum RTFormat
		{
			RT_FORMAT_RGBA8 = 0,
			RT_FORMAT_DEPTH_COMPONENT
		};

		//! Abstract render target 
		class RenderTarget
		{
		public:
			// TODO: How does directx handle RTs? :(
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_RENDERTARGET_H_