#ifndef _PXF_GRAPHICS_PRIMITIVETYPE_H_
#define _PXF_GRAPHICS_PRIMITIVETYPE_H_

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
		
		const char* PrimitiveTypeName(PrimitiveType _PrimitiveType);
	} // Graphics
} // Pxf

#endif // _PXF_GRAPHICS_PRIMITIVETYPE_H_