#ifndef _PXF_GRAPHICS_PRIMITIVEBATCH_H_
#define _PXF_GRAPHICS_PRIMITIVEBATCH_H_

namespace Pxf
{
	namespace Graphics
	{
		enum DeviceType;
		enum PrimitiveType;

		//! Abstract class for primitive batch (2D rendering tool)
		class PrimitiveBatch
		{
			public:
				// Quads
				virtual void QuadsBegin() = 0;
				virtual void QuadsDrawFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3) = 0;
				virtual void QuadsEnd() = 0;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_PRIMITIVEBATCH_H_