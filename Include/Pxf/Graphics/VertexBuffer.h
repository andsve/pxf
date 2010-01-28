#ifndef _PXF_GRAPHICS_VERTEXBUFFER_H_
#define _PXF_GRAPHICS_VERTEXBUFFER_H_

#include <Pxf/Graphics/Device.h>

namespace Pxf
{
	namespace Graphics
	{
		enum DeviceType;

		//! Abstract class for vertex buffer
		class VertexBuffer
		{
			private:
			public:
				//virtual VertexBuffer& Lock() = 0;
				//virtual VertexBuffer& Unlock() = 0;
				//virtual bool IsLocked() = 0; 
				
				virtual void SetPrimitive(VertexBufferPrimitiveType _PrimitiveType) = 0;
				virtual VertexBufferPrimitiveType GetPrimitive() = 0;
				//virtual bool Fill(float* _Data,int _Stride,int _Size) = 0;
				virtual DeviceType GetDeviceType() = 0;
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_VERTEXBUFFER_H_