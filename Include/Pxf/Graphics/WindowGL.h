#ifndef _PXF_GRAPHICS_WINDOWGL_H_
#define _PXF_GRAPHICS_WINDOWGL_H_

#include <Pxf/Graphics/Window.h>

namespace Pxf{
	namespace Graphics {

		class WindowGL : public Window
		{
		public:
			WindowGL(int _width, int _height, bool _fullscreen = false, bool _vsync = false);
			bool Open();
			bool Close();
			void Swap();

			void SetTitle(const char *_title);

			int GetFPS();
			bool IsOpen();
			bool IsActive();
		private:
			int m_width, m_height;
			bool m_fullscreen, m_opened, m_vsync;

			// FPS
			double m_fps_laststamp;
			int m_fps, m_fps_count;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_WINDOWGL_H_
