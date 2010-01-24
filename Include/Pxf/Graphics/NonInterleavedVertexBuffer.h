#ifndef _PXF_GRAPHICS_NONINTERLEAVEDVERTEXBUFFER_H_
#define _PXF_GRAPHICS_NONINTERLEAVEDVERTEXBUFFER_H_

#include <Pxf/Graphics/DeviceType.h> // enum DeviceType
#include <Pxf/Graphics/PrimitiveType.h> // enum PrimitiveType
#include <Pxf/Graphics/Device.h> // enumerations for VertexBuffer
#include <Pxf/Math/Vector.h>

/*
	TODO:

	Only relevant to VBO?
	//virtual void UpdateData(AttribType _AttribType, size);
	//virtual void* MapData();
	//virtual void UnmapData();
*/

namespace Pxf
{
	namespace Graphics
	{
		class Device;

		//! Abstract class for vertex buffer
		class NonInterleavedVertexBuffer
		{
		protected:
			unsigned int m_Attributes;
			PrimitiveType m_PrimitiveType;
			VertexBufferLocation m_VertexBufferLocation;

		public:
			NonInterleavedVertexBuffer(VertexBufferLocation _VertexBufferLocation)
				: m_Attributes(0)
				, m_PrimitiveType(ETriangleList)
				, m_VertexBufferLocation(_VertexBufferLocation)
			{}

			virtual void _PreDraw() = 0;
			virtual void _PostDraw() = 0;


			// void* CreateNew();

			// Interleaved
			virtual void SetData(VertexBufferAttribute _AttribType, unsigned _TypeSize, const void* _Ptr, unsigned _Count, unsigned _Stride = 0) = 0;

			// Commit and create vertex object
			virtual bool Commit() = 0;

			void SetPrimitive(PrimitiveType _PrimitiveType)
			{
				m_PrimitiveType = _PrimitiveType;
			}

			PrimitiveType GetPrimitive()
			{
				return m_PrimitiveType;
			}
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_NONINTERLEAVEDVERTEXBUFFER_H_