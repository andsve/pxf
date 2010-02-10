#ifndef _PXF_GRAPHICS_RENDERTARGET_H_
#define _PXF_GRAPHICS_RENDERTARGET_H_

namespace Pxf
{
	namespace Graphics
	{		
		// Move the enums..
		enum RTFormat
		{
			RT_FORMAT_RGBA8 = 0,
			RT_FORMAT_DEPTH_COMPONENT
		};

		enum RTType
		{
			RT_TYPE_FBO = 0,
			RT_TYPE_PBO
		};

		//! Abstract render target 
		class RenderTarget
		{
			RTType m_RTType;
		public:
			// TODO: How does directx handle RTs? :(
			RenderTarget(RTType _Type) { m_RTType = _Type; }
			RTType GetType() { return m_RTType; }
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_RENDERTARGET_H_