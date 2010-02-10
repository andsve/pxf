#ifndef _PXF_GRAPHICS_TEXTUREDEFS_H
#define _PXF_GRAPHICS_TEXTUREDEFS_H

namespace Pxf
{
	namespace Graphics
	{	
		enum TextureFilter
		{
			// valid for min & mag
			TEX_FILTER_NEAREST = 0,
			TEX_FILTER_LINEAR,

			// valid only for min, obviously.
			TEX_FILTER_LINEAR_MIPMAP_LINEAR,
			TEX_FILTER_LINEAR_MIPMAP_NEAREST,
			TEX_FILTER_NEAREST_MIPMAP_LINEAR,
			TEX_FILTER_NEAREST_MIPMAP_NEAREST
		};

		enum TextureClampMethod
		{
			TEX_CLAMP,
			TEX_CLAMP_TO_EDGE,
			TEX_REPEAT
		};
		
		enum TextureFormatStorage
		{
			TEX_FORMAT_RGB,
			TEX_FORMAT_RGBA,
			TEX_FORMAT_R,
			TEX_FORMAT_G,
			TEX_FORMAT_B,
			TEX_FORMAT_A
		};
	}
}

#endif