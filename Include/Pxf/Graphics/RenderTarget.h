#ifndef _PXF_GRAPHICS_RENDERTARGET_H_
#define _PXF_GRAPHICS_RENDERTARGET_H_

#include <Pxf/Base/Debug.h>
#include <Pxf/Graphics/DeviceResource.h>

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

		class Device;
		class Texture;

		//! Abstract render target 
		class RenderTarget : public DeviceResource
		{
			RTType m_RTType;
		public:
			// TODO: How does directx handle RTs? :(
			RenderTarget(Device* _pDevice, RTType _Type)
				: DeviceResource(_pDevice)
			{ m_RTType = _Type; }

			RTType GetType() { return m_RTType; }

			virtual void AddColorAttachment(Texture* _Color) = 0;
			virtual void AddDepthAttachment(Texture* _Depth) = 0;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_RENDERTARGET_H_