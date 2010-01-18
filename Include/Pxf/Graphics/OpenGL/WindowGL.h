#ifndef _PXF_GRAPHICS_WINDOWGL_H_
#define _PXF_GRAPHICS_WINDOWGL_H_

#include <Pxf/Graphics/Window.h>

namespace Pxf{
	namespace Graphics {

		class WindowGL : public Window
		{
		public:
			WindowGL(int _width, int _height, int _color_bits, int _alpha_bits, int _depth_bits, int _stencil_bits, bool _fullscreen = false, bool _resizeable = false, bool _vsync = false, int _fsaasamples = 0);
			bool Open();
			bool Close();
			void Swap();

			void SetTitle(const char *_title);

			int GetFPS();
			int GetWidth();
			int GetHeight();
			float GetAspectRatio();
			char* GetContextTypeName();

			bool IsOpen();
			bool IsActive();
			bool IsMinimized();
		private:
			int m_width, m_height;
			bool m_fullscreen, m_resizeable, m_vsync;

			// Bit settings
			int m_bits_color, m_bits_alpha, m_bits_depth, m_bits_stencil;

			// FSAA
			int m_fsaa_samples;

			// FPS
			int64 m_fps_laststamp;
			int m_fps, m_fps_count;
		};

	} // Graphics
} // Pxf

#endif //_PXF_GRAPHICS_WINDOWGL_H_
