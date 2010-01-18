#ifndef _PXF_GRAPHICS_WINDOWSPECIFICATIONS_H_
#define _PXF_GRAPHICS_WINDOWSPECIFICATIONS_H_

namespace Pxf
{
	struct WindowSpecifications
	{
		int Width;
		int Height;
		int ColorBits;
		int AlphaBits;
		int DepthBits;
		int StencilBits;
		int FSAASamples;
		bool Fullscreen;
		bool Resizeable;
		bool VerticalSync;
	};
}

#endif // _PXF_GRAPHICS_WINDOWSPECIFICATIONS_H_