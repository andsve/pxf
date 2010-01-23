#ifndef _PXF_GRAPHICS_PRIMITIVEBATCHGL2_H_
#define _PXF_GRAPHICS_PRIMITIVEBATCHGL2_H_

#include <Pxf/Graphics/PrimitiveBatch.h>

namespace Pxf
{
	namespace Graphics
	{
		enum DeviceType;
		enum PrimitiveType;

		//! Abstract class for primitive batch (2D rendering tool)
		class PrimitiveBatchGL2 : public PrimitiveBatch
		{
			public:
				PrimitiveBatchGL2();
				~PrimitiveBatchGL2();

				// Quads
				void QuadsBegin();
				void QuadsDrawFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
				void QuadsEnd();
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_PRIMITIVEBATCHGL2_H_