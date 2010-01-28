#ifndef _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFER_H_
#define _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFER_H_

#include <Pxf/Base/Types.h>
#include <Pxf/Graphics/DeviceType.h> // enum DeviceType
#include <Pxf/Graphics/PrimitiveType.h> // enum PrimitiveType
#include <Pxf/Graphics/Device.h> // enumerations for VertexBuffer
#include <Pxf/Math/Vector.h>

// TODO: Support attribute data: http://www.opengl.org/wiki/Vertex_Array_Objects

namespace Pxf
{
	namespace Graphics
	{
		class Device;

		//! Abstract class for vertex buffer
		class InterleavedVertexBuffer
		{
		private:
			struct AttributeData
			{
				/* uint8 should be large enough to handle stride offsets and type sizes */
				uint8 TypeSize;
				uint8 StrideOffset;
				AttributeData(uint8 _TypeSize, uint8 _StrideOffset)
				{
					TypeSize = _TypeSize;
					StrideOffset = _StrideOffset;
				}
			};
		protected:
			uint32 m_Attributes;
			PrimitiveType m_PrimitiveType;
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
			InterleavedVertexBuffer(VertexBufferLocation _VertexBufferLocation)
				: m_Attributes(0)
				, m_PrimitiveType(ETriangleList)
				, m_VertexBufferLocation(_VertexBufferLocation)
				, m_VertexBufferUsageFlag(VB_STATIC_DRAW)
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

			virtual void _PreDraw() = 0;
			virtual void _PostDraw() = 0;


			virtual void CreateNewBuffer(uint32 _NumVertices, uint32 _VertexSize, VertexBufferUsageFlag _UsageFlag) = 0;
			virtual void CreateFromBuffer(void* _Buffer,uint32 _NumVertices, uint32 _VertexSize, VertexBufferUsageFlag _UsageFlag) = 0; 

			virtual void UpdateData(void* _Buffer, uint32 _Count, uint32 _Offset) = 0;

			virtual void* MapData(VertexBufferAccessFlag _AccessFlag) = 0;
			virtual void UnmapData() = 0;

			void SetData(VertexBufferAttribute _AttribType, uint8 _StrideOffset, uint8 _TypeSize)
			{
				m_Attributes |= _AttribType;
				switch(_AttribType)
				{
				case VB_VERTEX_DATA:	m_VertexAttributes.StrideOffset = _StrideOffset;
										m_VertexAttributes.TypeSize = _TypeSize;
										break;
				case VB_NORMAL_DATA:	m_NormalAttributes.StrideOffset = _StrideOffset;
										m_NormalAttributes.TypeSize = _TypeSize;
										break;
				case VB_TEXCOORD_DATA:	m_TexCoordAttributes.StrideOffset = _StrideOffset;
										m_TexCoordAttributes.TypeSize = _TypeSize;
										break;
				case VB_COLOR_DATA:		m_ColorAttributes.StrideOffset = _StrideOffset;
										m_ColorAttributes.TypeSize = _TypeSize;
										break;
				case VB_INDEX_DATA:		m_IndexAttributes.StrideOffset = _StrideOffset;
										m_IndexAttributes.TypeSize = _TypeSize;
										break;
				case VB_EDGEFLAG_DATA:	m_EdgeFlagAttributes.StrideOffset = _StrideOffset;
										m_EdgeFlagAttributes.TypeSize = _TypeSize;
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
				case VB_VERTEX_DATA:   return m_VertexAttributes.TypeSize;
				case VB_NORMAL_DATA:   return m_NormalAttributes.TypeSize;
				case VB_TEXCOORD_DATA: return m_TexCoordAttributes.TypeSize;
				case VB_COLOR_DATA:    return m_ColorAttributes.TypeSize;
				case VB_INDEX_DATA:    return m_IndexAttributes.TypeSize;
				case VB_EDGEFLAG_DATA: return m_EdgeFlagAttributes.TypeSize;
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

#endif // _PXF_GRAPHICS_INTERLEAVEDVERTEXBUFFER_H_