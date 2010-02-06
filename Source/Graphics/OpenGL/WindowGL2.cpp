#include <Pxf/Pxf.h>
#include <Pxf/Base/Clock.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/WindowGL2.h>
#include <Pxf/Base/Debug.h>

#ifdef CONF_PLATFORM_MACOSX
#include <Carbon/Carbon.h>
#endif
#include <Pxf/Graphics/OpenGL/OpenGL.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

int WindowGL2::GetWidth() { return m_width; }
int WindowGL2::GetHeight() {return m_height; }
float WindowGL2::GetAspectRatio() { return ((float)m_width / (float)m_height); }

WindowGL2::WindowGL2(WindowSpecifications *_window_spec)
{
	// Window settings
	m_width = _window_spec->Width;
	m_height = _window_spec->Height;
	m_fullscreen = _window_spec->Fullscreen;
	m_resizeable = _window_spec->Resizeable;
	m_vsync = _window_spec->VerticalSync;
	m_fsaa_samples = _window_spec->FSAASamples;

	// Buffer bits settings
	m_bits_color = _window_spec->ColorBits;
	m_bits_alpha = _window_spec->AlphaBits;
	m_bits_depth = _window_spec->DepthBits;
	m_bits_stencil = _window_spec->StencilBits;

	// FPS
	m_fps = 0;
	m_fps_count = 0;
	m_fps_laststamp = Clock::GetTime();
}

WindowGL2::~WindowGL2()
{
	Close();
}

bool WindowGL2::Open()
{
	int t_params = GLFW_WINDOW;

	if (IsOpen())
		return false; // can't open an already open window

	// Enable fullscreen
	if (m_fullscreen)
		t_params = GLFW_FULLSCREEN;

	// Enable vertical sync
	if (m_vsync)
		glfwSwapInterval(1);

	// Set number of FSAA samples
	if (m_fsaa_samples > 0)
		glfwOpenWindowHint(GLFW_FSAA_SAMPLES, m_fsaa_samples);

	if (GL_TRUE == glfwOpenWindow(m_width, m_height, m_bits_color, m_bits_color, m_bits_color, m_bits_alpha, m_bits_depth, m_bits_stencil, t_params))
	{

#ifdef CONF_PLATFORM_MACOSX
		/* HACK - Get events without bundle */
		ProcessSerialNumber psn;    
		GetCurrentProcess(&psn);
		TransformProcessType(&psn,kProcessTransformToForegroundApplication);
		SetFrontProcess(&psn);
#endif
		
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			Message("WindowGL2", "Could not initiate glew.");
		}

		Message("WindowGL2", "Opened window of %dx%d@%d (r: %d g: %d b: %d a: %d d: %d s: %d)", m_width, m_height, m_bits_color*3+m_bits_alpha, m_bits_color, m_bits_color, m_bits_color, m_bits_alpha, m_bits_depth, m_bits_stencil);

		return true;
	}
	else
		return false;

	
}

bool WindowGL2::Close()
{
	if(!IsOpen())
		return false;

	glfwCloseWindow();
	return true;
}

void WindowGL2::Swap()
{
	if (IsOpen())
	{
		int64 diff;
		int64 t_current_time = Clock::GetTime();
		diff = t_current_time - m_fps_laststamp;
		if (diff >= 1000)
		{
			m_fps = m_fps_count;
			m_fps_count = 0;
			m_fps_laststamp = t_current_time;
		}

		glfwSwapBuffers();
		m_fps_count += 1;
	}
}

bool WindowGL2::IsOpen()
{
	if (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
		return true;
	else
		return false;
}

bool WindowGL2::IsActive()
{
	if (IsOpen())
	{
		if (GL_TRUE == glfwGetWindowParam(GLFW_ACTIVE))
			return true;
	}
	return false;
}

bool WindowGL2::IsMinimized()
{
	if (IsOpen())
	{
		if (GL_TRUE == glfwGetWindowParam(GLFW_ICONIFIED))
			return true;
	}

	return false;
}

void WindowGL2::SetTitle(const char *_title)
{
	if (IsOpen())
	{
		glfwSetWindowTitle(_title);
	}
}

int WindowGL2::GetFPS()
{
	return m_fps;
}

char* WindowGL2::GetContextTypeName()
{
	return "OpenGL";
}
