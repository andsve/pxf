#ifndef _PXF_GRAPHICS_NONINTERLEAVEDVERTEXBUFFER_H_
#define _PXF_GRAPHICS_NONINTERLEAVEDVERTEXBUFFER_H_

#include <Pxf/Graphics/DeviceType.h> // enum DeviceType
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/DeviceResource.h>
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
		class NonInterleavedVertexBuffer : public DeviceResource
		{
		private:
			struct AttributeData
			{
				uint8 NumComponents;
				void* Data;
				AttributeData(uint8 _NumComponents, void* _Data)
				{
					NumComponents = _NumComponents;
					Data = _Data;
				}
			};

		protected:
			unsigned int m_Attributes;
			VertexBufferPrimitiveType m_PrimitiveType;
			VertexBufferLocation m_VertexBufferLocation;
			VertexBufferUsageFlag m_VertexBufferUsageFlag;

			uint32 m_VertexCount;
			bool m_IsMapped;

			AttributeData m_VertexAttributes;
			AttributeData m_NormalAttributes;
			AttributeData m_TexCoordAttributes;
			AttributeData m_ColorAttributes;
			AttributeData m_IndexAttributes;
			AttributeData m_EdgeFlagAttributes;

		public:
			NonInterleavedVertexBuffer(Device* _pDevice, VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag)
				: DeviceResource(_pDevice)
				, m_Attributes(0)
				, m_PrimitiveType(VB_PRIMITIVE_NONE)
				, m_VertexBufferLocation(_VertexBufferLocation)
				, m_VertexBufferUsageFlag(_VertexBufferUsageFlag)
				, m_VertexAttributes(0, 0)
				, m_NormalAttributes(0, 0)
				, m_TexCoordAttributes(0, 0)
				, m_ColorAttributes(0, 0)
				, m_IndexAttributes(0, 0)
				, m_EdgeFlagAttributes(0, 0)
				, m_VertexCount(0)
				, m_IsMapped(false)
			{}

			virtual void _PreDraw() = 0;
			virtual void _PostDraw() = 0;


			//void SetData(VertexBufferAttribute _AttribType, void* _DataPtr, uint8 _NumComponents);

			void SetPrimitive(VertexBufferPrimitiveType _PrimitiveType)
			{
				m_PrimitiveType = _PrimitiveType;
			}

			VertexBufferPrimitiveType GetPrimitive()
			{
				return m_PrimitiveType;
			}
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_NONINTERLEAVEDVERTEXBUFFER_H_