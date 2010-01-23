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
			
			// Interleaved
			virtual void SetData(VertexBufferAttribute _AttribType, unsigned _TypeSize, const void* _Ptr, unsigned _Count, unsigned _Stride = 0) = 0;
			//virtual void UpdateData(AttribType _AttribType, size);
			//virtual void* MapData();
			//virtual void UnmapData();

			// Commit and create vertex object
			virtual bool Commit() = 0;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_VERTEXBUFFER_H_