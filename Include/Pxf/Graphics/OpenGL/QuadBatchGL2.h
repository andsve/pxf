#ifndef _PXF_GRAPHICS_QUADBATCHGL2_H_
#define _PXF_GRAPHICS_QUADBATCHGL2_H_

#include <Pxf/Graphics/Device.h> // We need Vertex-struct
#include <Pxf/Graphics/QuadBatch.h>

namespace Pxf
{
	namespace Graphics
	{
		enum DeviceType;
		enum PrimitiveType;
		
		class QuadBatchGL2 : public QuadBatch
		{
			public:
				QuadBatchGL2(int _maxSize);
				~QuadBatchGL2();

				void SetColor(float r, float g, float b, float a);
				void SetColor(Math::Vec4f* c);
				void SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v);
				void SetRotation(float angle);

				void Begin();
				void DrawFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
				void DrawTopLeft(float x, float y, float w, float h);
				void DrawCentered(float x, float y, float w, float h);
				void End();
			private:
				void Flush();
				void Rotate(const Math::Vec3f &center, Math::Vec3f &point);

				Vertex* m_Vertices;
				Math::Vec4f m_CurrentColors[4];
				Math::Vec2f m_CurrentTexCoords[4];
				float m_CurrentDepthLayer;

				float m_Rotation;

				int m_VertexBufferPos;
				int m_VertexBufferSize;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_QUADBATCHGL2_H_