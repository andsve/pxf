#ifndef _PXF_GRAPHICS_TEXTURE_H_
#define _PXF_GRAPHICS_TEXTURE_H_

#include <Pxf/Math/Vector.h>

namespace Pxf
{
	namespace Graphics
	{
		
		//! Abstract texture class
		class Texture
		{
		public:
			virtual void Load(const char* _filepath) = 0;
			virtual void Unload() = 0;
			virtual void Reload() = 0;

			virtual int GetWidth() = 0;
			virtual int GetHeight() = 0;
			virtual int GetChannels() = 0;

			virtual Math::Vec4i CreateTextureSubset(float _x1, float _y1, float _x2, float _y2) = 0;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_TEXTURE_H_