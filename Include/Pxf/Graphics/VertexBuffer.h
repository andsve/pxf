#ifndef _PXF_GRAPHICS_VERTEXBUFFER_H_
#define _PXF_GRAPHICS_VERTEXBUFFER_H_

namespace Pxf
{
	namespace Graphics
	{
		enum PrimitiveType
		{
			EPointList,
			ELineList,
			ELineStrip,
			ETriangleStrip,
			ETriangleList,
			ETriangleFan,
		};

		class VertexBuffer
		{
			private:
			public:
				virtual VertexBuffer& Lock() = 0;
				virtual VertexBuffer& Unlock() = 0;
				virtual bool IsLocked() = 0; 
				
				virtual void SetPrimitive(PrimitiveType _PrimitiveType) = 0;
				virtual PrimitiveType GetPrimitive() = 0;
				virtual 
		};
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_VERTEXBUFFER_H_