#ifndef _PXF_GRAPHICS_WINDOW_H_
#define _PXF_GRAPHICS_WINDOW_H_

namespace Pxf{
	namespace Graphics {

		class Window
		{
		public:
			virtual bool Open() = 0;
			virtual bool Close() = 0;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_WINDOW_H_
