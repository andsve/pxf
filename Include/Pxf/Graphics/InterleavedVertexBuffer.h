#ifndef _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFER_H_
#define _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFER_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Graphics/DeviceType.h> // enum DeviceType
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/DeviceResource.h>
#include <Pxf/Math/Vector.h>

// TODO: Support attribute data: http://www.opengl.org/wiki/Vertex_Array_Objects

namespace Pxf
{
	namespace Graphics
	{
		class Device;

		//! Abstract class for vertex buffer
		class InterleavedVertexBuffer : public DeviceResource
		{
		private:
			struct AttributeData
			{
				uint8 NumComponents;
				uint8 StrideOffset;
				AttributeData(uint8 _NumComponents, uint8 _StrideOffset)
				{
					NumComponents = _NumComponents;
					StrideOffset = _StrideOffset;
				}
			};
		protected:
			uint32 m_Attributes;
			VertexBufferPrimitiveType m_PrimitiveType;
			VertexBufferLocation m_VertexBufferLocation;
			VertexBufferUsageFlag m_VertexBufferUsageFlag;

			void* m_InterleavedData;
			uint32 m_VertexCount;
			uint32 m_VertexSize;
			uint32 m_ByteCount;
			bool m_IsMapped;

			AttributeData m_VertexAttributes;
			AttributeData m_NormalAttributes;
			AttributeData m_TexCoordAttributes;
			AttributeData m_ColorAttributes;
			AttributeData m_IndexAttributes;
			AttributeData m_EdgeFlagAttributes;

		public:
			InterleavedVertexBuffer(Device* _pDevice, VertexBufferLocation _VertexBufferLocation, VertexBufferUsageFlag _VertexBufferUsageFlag)
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
				, m_InterleavedData(0)
				, m_VertexSize(0)
				, m_VertexCount(0)
				, m_ByteCount(0)
				, m_IsMapped(false)
			{}

			// TODO: Combine into Draw() and move glDrawArrays() from Device to the implementation.
			virtual void _PreDraw() = 0;
			virtual void _PostDraw() = 0;


			virtual void CreateNewBuffer(uint32 _NumVertices, uint32 _VertexSize) = 0;
			virtual void CreateFromBuffer(void* _Buffer,uint32 _NumVertices, uint32 _VertexSize) = 0; 

			virtual void UpdateData(void* _Buffer, uint32 _Count, uint32 _Offset) = 0;

			virtual void* MapData(VertexBufferAccessFlag _AccessFlag) = 0;
			virtual void UnmapData() = 0;

			void SetData(VertexBufferAttribute _AttribType, uint8 _StrideOffset, uint8 _NumComponents)
			{
				m_Attributes |= _AttribType;
				switch(_AttribType)
				{
				case VB_VERTEX_DATA:	m_VertexAttributes.StrideOffset = _StrideOffset;
										m_VertexAttributes.NumComponents = _NumComponents;
										break;
				case VB_NORMAL_DATA:	m_NormalAttributes.StrideOffset = _StrideOffset;
										m_NormalAttributes.NumComponents = _NumComponents;
										break;
				case VB_TEXCOORD_DATA:	m_TexCoordAttributes.StrideOffset = _StrideOffset;
										m_TexCoordAttributes.NumComponents = _NumComponents;
										break;
				case VB_COLOR_DATA:		m_ColorAttributes.StrideOffset = _StrideOffset;
										m_ColorAttributes.NumComponents = _NumComponents;
										break;
				case VB_INDEX_DATA:		m_IndexAttributes.StrideOffset = _StrideOffset;
										m_IndexAttributes.NumComponents = _NumComponents;
										break;
				case VB_EDGEFLAG_DATA:	m_EdgeFlagAttributes.StrideOffset = _StrideOffset;
										m_EdgeFlagAttributes.NumComponents = _NumComponents;
										break;
				}
			}

			uint8 GetStrideOffset(VertexBufferAttribute _AttribType)
			{
				switch(_AttribType)
				{
				case VB_VERTEX_DATA:   return m_VertexAttributes.StrideOffset;
				case VB_NORMAL_DATA:   return m_NormalAttributes.StrideOffset;
				case VB_TEXCOORD_DATA: return m_TexCoordAttributes.StrideOffset;
				case VB_COLOR_DATA:    return m_ColorAttributes.StrideOffset;
				case VB_INDEX_DATA:    return m_IndexAttributes.StrideOffset;
				case VB_EDGEFLAG_DATA: return m_EdgeFlagAttributes.StrideOffset;
				}

				PXFASSERT(0, "Unknown attribute type specified");
				return 0;
			}

			uint8 GetTypeSize(VertexBufferAttribute _AttribType)
			{
				switch(_AttribType)
				{
				case VB_VERTEX_DATA:   return m_VertexAttributes.NumComponents;
				case VB_NORMAL_DATA:   return m_NormalAttributes.NumComponents;
				case VB_TEXCOORD_DATA: return m_TexCoordAttributes.NumComponents;
				case VB_COLOR_DATA:    return m_ColorAttributes.NumComponents;
				case VB_INDEX_DATA:    return m_IndexAttributes.NumComponents;
				case VB_EDGEFLAG_DATA: return m_EdgeFlagAttributes.NumComponents;
				}

				PXFASSERT(0, "Unknown attribute type specified");
				return 0;
			}

			uint32 GetVertexCount() const
			{
				return m_VertexCount;
			}

			uint32 GetByteCount() const
			{
				return m_ByteCount;
			}

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

#endif // _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFER_H_