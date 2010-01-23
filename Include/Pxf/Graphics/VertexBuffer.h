#ifndef _PXF_GRAPHICS_VERTEXBUFFER_H_
#define _PXF_GRAPHICS_VERTEXBUFFER_H_

#include <Pxf/Graphics/DeviceType.h> // enum DeviceType
#include <Pxf/Graphics/PrimitiveType.h> // enum PrimitiveType
#include <Pxf/Graphics/Device.h> // enumerations for VertexBuffer
#include <Pxf/Math/Vector.h>

namespace Pxf
{
	namespace Graphics
	{
		class Device;

		//! Abstract class for vertex buffer
		class VertexBuffer
		{
		protected:
			unsigned int m_Attributes;
			PrimitiveType m_PrimitiveType;
			VertexBufferType m_VertexBufferType;

		public:
			VertexBuffer(VertexBufferType _VertexBufferType)
				: m_Attributes(0)
				, m_PrimitiveType(ETriangleList)
				, m_VertexBufferType(_VertexBufferType)
			{}

			virtual void _PreDraw() = 0;
			virtual void _PostDraw() = 0;

			// Interleaved
			virtual void SetData(VertexBufferAttribute _AttribType, unsigned _TypeSize, const void* _Ptr, unsigned _Count, unsigned _Stride = 0) = 0;
			//virtual void UpdateData(AttribType _AttribType, size);
			//virtual void* MapData();
			//virtual void UnmapData();

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

#endif // _PXF_GRAPHICS_VERTEXBUFFER_H_