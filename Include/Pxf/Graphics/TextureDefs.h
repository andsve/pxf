#ifndef _PXF_GRAPHICS_TEXTUREDEFS_H
#define _PXF_GRAPHICS_TEXTUREDEFS_H

namespace Pxf
{
	namespace Graphics
	{	
		enum TextureFilter
		{
			// valid for min & mag
			FILTER_NEAREST = 0,
			FILTER_LINEAR,

			// valid only for min, obviously.
			FILTER_LINEAR_MIPMAP_LINEAR,
			FILTER_LINEAR_MIPMAP_NEAREST,
			FILTER_NEAREST_MIPMAP_LINEAR,
			FILTER_NEAREST_MIPMAP_NEAREST
		};

		enum TextureClampMethod
		{
			CLAMP,
			CLAMP_TO_EDGE,
			REPEAT
		};
		
		enum TextureFormatStorage
		{
			FORMAT_RGB,
			FORMAT_RGBA,
			FORMAT_R,
			FORMAT_G,
			FORMAT_B,
			FORMAT_A
		};
	}
}

#endif