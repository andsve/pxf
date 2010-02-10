#ifndef _PXF_GRAPHICS_TEXTURE_H_
#define _PXF_GRAPHICS_TEXTURE_H_

#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Graphics/TextureDefs.h>
#include <Pxf/Math/Vector.h>

namespace Pxf
{
	namespace Graphics
	{
		
		//! Abstract texture class
		class Texture : public DeviceResource
		{
		public:
			Texture(Device* _pDevice)
				: DeviceResource(_pDevice)
			{}

			virtual void Load(const char* _filepath) = 0;
			virtual void LoadData(const unsigned char* _datachunk, int _width, int _height, int _channels) = 0;
			virtual void Unload() = 0;
			virtual void Reload() = 0;

			virtual int GetWidth() = 0;
			virtual int GetHeight() = 0;
			virtual int GetChannels() = 0;

			virtual void SetMagFilter(TextureFilter _Filter) = 0;
			virtual void SetMinFilter(TextureFilter _Filter) = 0;

			virtual void SetClampMethod(TextureClampMethod _Method) = 0;

			virtual Math::Vec4f CreateTextureSubset(float _x1, float _y1, float _x2, float _y2) = 0;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTURE_H_