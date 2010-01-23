#ifndef _PXF_GRAPHICS_VERTEXBUFFER_H_
#define _PXF_GRAPHICS_VERTEXBUFFER_H_

#include <Pxf/Graphics/DeviceType.h> // enum DeviceType
#include <Pxf/Graphics/PrimitiveType.h> // enum PrimitiveType

namespace Pxf
{
	namespace Graphics
	{
		class Device;

		//! Abstract class for vertex buffer
		class VertexBuffer
		{
		private:
			virtual void PreDraw() = 0;
			virtual void PostDraw() = 0;
			friend class Device;
		public:
			virtual void SetPrimitive(PrimitiveType _PrimitiveType) = 0;
			virtual PrimitiveType GetPrimitive() = 0;
			//virtual bool Fill(float* _Data,int _Stride,int _Size) = 0;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_VERTEXBUFFER_H_