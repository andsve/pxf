#ifndef _PXF_GRAPHICS_VERTEXBUFFER_H_
#define _PXF_GRAPHICS_VERTEXBUFFER_H_

#include <Pxf/Graphics/DeviceType.h> // enum DeviceType
#include <Pxf/Graphics/PrimitiveType.h> // enum PrimitiveType
#include <Pxf/Math/Vector.h>

namespace Pxf
{
	namespace Graphics
	{
		class Device;

		//! Abstract class for vertex buffer
		class VertexBuffer
		{
		private:
			unsigned int m_Attributes;

			virtual void PreDraw() = 0;
			virtual void PostDraw() = 0;
			friend class Device;
		public:
			VertexBuffer()
				: m_Attributes(0)
			{}

			virtual void SetPrimitive(PrimitiveType _PrimitiveType) = 0;
			virtual PrimitiveType GetPrimitive() = 0;
			
			// Prepare with pointers to vertex data and attributes
			virtual void SetVertexData(Math::Vec2f _data, unsigned _count, unsigned _stride = 0) = 0;
			virtual void SetVertexData(Math::Vec3f _data, unsigned _count, unsigned _stride = 0) = 0;
			virtual void SetNormalData(Math::Vec3f _data, unsigned _count, unsigned _stride = 0) = 0;
			virtual void SetTexCoordData(Math::Vec2f _data, unsigned _count, unsigned _stride = 0) = 0;
			virtual void SetColorData(Math::Vec3f _data, unsigned _count, unsigned _stride = 0) = 0;
			virtual void SetColorData(Math::Vec4f _data, unsigned _count, unsigned _stride = 0) = 0;
			virtual void SetIndexData(unsigned int* _data, unsigned _count, unsigned _stride = 0) = 0;
			virtual void SetEdgeFlagData(bool* _data, unsigned _count, unsigned _stride = 0) = 0;

			// Interleaved
			virtual void SetData(VertexBufferAttribute _AttribType, const void* _Ptr, unsigned _Size, unsigned _Count, unsigned _Stride = 0);
			//virtual void UpdateData(AttribType _AttribType, size);
			//virtual void* MapData();
			//virtual void UnmapData();

			// Commit and create vertex object
			virtual bool Commit();
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_VERTEXBUFFER_H_