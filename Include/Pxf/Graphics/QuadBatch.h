#ifndef _PXF_GRAPHICS_QUADBATCH_H_
#define _PXF_GRAPHICS_QUADBATCH_H_

#include <Pxf/Math/Vector.h>

namespace Pxf
{
	namespace Graphics
	{
		enum DeviceType;
		enum PrimitiveType;

		//! Abstract class for quad primitive batches (2D rendering tool)
		class QuadBatch
		{
			public:
				virtual void SetColor(float r, float g, float b, float a) = 0;
				virtual void SetColor(Math::Vec4f* c) = 0;
				virtual void SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v) = 0;
				virtual void SetRotation(float angle) = 0;

				virtual void Begin() = 0;
				virtual void DrawFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3) = 0;
				virtual void DrawTopLeft(float x, float y, float w, float h) = 0;
				virtual void DrawCentered(float x, float y, float w, float h) = 0;
				virtual void End() = 0;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_QUADBATCH_H_