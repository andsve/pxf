#ifndef _PXF_GRAPHICS_WINDOWGL_H_
#define _PXF_GRAPHICS_WINDOWGL_H_

#include <Pxf/Graphics/Window.h>

namespace Pxf{
	namespace Graphics {

		class WindowGL : public Window
		{
		public:
			WindowGL(int _width, int _height, bool _fullscreen = false);
			bool Open();
			bool Close();
		private:
			int m_width, m_height;
			bool m_fullscreen, m_opened;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_WINDOWGL_H_
