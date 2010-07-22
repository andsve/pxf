#ifndef _PXF_GRAPHICS_QUADBATCHGLES11_H_
#define _PXF_GRAPHICS_QUADBATCHGLES11_H_

#include <Pxf/Graphics/Device.h> // We need Vertex-struct
#include <Pxf/Graphics/QuadBatch.h>

namespace Pxf
{
	namespace Graphics
	{
		
		class QuadBatchGLES11 : public QuadBatch
		{
			public:

				struct Vertex
				{
					Math::Vec3f pos;
					Math::Vec2f tex;
					Math::Vec4f color;
				};

				QuadBatchGLES11(Device* _pDevice, int _maxSize);
				~QuadBatchGLES11();

				void SetColor(float r, float g, float b);
				void SetColor(Math::Vec3f* c);
                void SetAlpha(float a);
				void SetTextureSubset(float tl_u, float tl_v, float br_u, float br_v);
				void SetRotation(float angle);
				void Rotate(float angle);
                void LoadIdentity();
				void Translate(float x, float y);
				void SetDepth(float d);

				void Reset();
				void AddFreeform(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
				void AddTopLeft(float x, float y, float w, float h);
				void AddCentered(float x, float y, float w, float h);
				void AddCentered(float x, float y, float w, float h, float rotation);
				
				void Draw();
			private:
				void Flush();
				void RotatePoint(const Math::Vec3f &center, Math::Vec3f &point, float rotation);

				Vertex* m_Vertices;
				Math::Vec4f m_CurrentColors[4];
				Math::Vec2f m_CurrentTexCoords[4];
				float m_CurrentDepthLayer;
                float m_CurrentAlpha;

				//float m_Rotation;
				
                //Math::Vec2f m_Translation;
                Math::Mat4 m_TransformMatrix;

				int m_VertexBufferPos;
				int m_VertexBufferSize;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_QUADBATCHGLES11_H_