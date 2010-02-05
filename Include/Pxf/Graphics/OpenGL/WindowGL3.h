#ifndef _PXF_GRAPHICS_WINDOWGL3_H_
#define _PXF_GRAPHICS_WINDOWGL3_H_

#ifdef CONF_FAMILY_WINDOWS
// Header File For Windows
#include <windows.h>
#endif
#include <Pxf/Base/Config.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Graphics/WindowSpecifications.h>

namespace Pxf{
	namespace Graphics {

		class WindowGL3 : public Window
		{
		public:
			WindowGL3(WindowSpecifications *_window_spec);
			~WindowGL3();

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
			bool InitWindow();

			#ifdef CONF_FAMILY_WINDOWS

			HDC			m_HDC;		// Private GDI Device Context
			HGLRC		m_HRC;		// Permanent Rendering Context
			HWND		m_Window;	// Window Handle
			HINSTANCE	m_HInstance;// Instance Of The Application	

			#endif // CONF_FAMILY_WINDOWS
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

#endif //_PXF_GRAPHICS_WINDOWGL3_H_
