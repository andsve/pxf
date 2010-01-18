#include <Pxf/Pxf.h>
#include <Pxf/Base/Clock.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/WindowGL.h>

#ifdef CONF_PLATFORM_MACOSX
#include <Carbon/Carbon.h>
#endif
#include <GL/glfw.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

int WindowGL::GetWidth() { return m_width; }
int WindowGL::GetHeight() {return m_height; }
float WindowGL::GetAspectRatio() { return ((float)m_width / (float)m_height); }

WindowGL::WindowGL(int _width, int _height, int _color_bits, int _alpha_bits, int _depth_bits, int _stencil_bits, bool _fullscreen /* = false */, bool _resizeable /* = false */, bool _vsync /* = false */, int _fsaasamples /* = 0 */)
{
	// Window settings
	m_width = _width;
	m_height = _height;
	m_fullscreen = _fullscreen;
	m_resizeable = _resizeable;
	m_vsync = _vsync;
	m_fsaa_samples = _fsaasamples;

	// Buffer bits settings
	m_bits_color = _color_bits;
	m_bits_alpha = _alpha_bits;
	m_bits_depth = _depth_bits;
	m_bits_stencil = _stencil_bits;

	// FPS
	m_fps = 0;
	m_fps_count = 0;
	m_fps_laststamp = Clock::GetTime();
}

bool WindowGL::Open()
{
	int t_params = GLFW_WINDOW;

	if (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
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

		return true;
	}
	else
		return false;

	
}

bool WindowGL::Close()
{
	if (glfwGetWindowParam(GLFW_OPENED) == GL_FALSE)
		return false;

	glfwCloseWindow();
	return true;
}

void WindowGL::Swap()
{
	if (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
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

bool WindowGL::IsOpen()
{
	if (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
		return true;
	else
		return false;
}

bool WindowGL::IsActive()
{
	if (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
	{
		if (GL_TRUE == glfwGetWindowParam(GLFW_ACTIVE))
			return true;
	}
	return false;
}

bool WindowGL::IsMinimized()
{
	if (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
	{
		if (GL_TRUE == glfwGetWindowParam(GLFW_ICONIFIED))
			return true;
	}

	return false;
}

void WindowGL::SetTitle(const char *_title)
{
	if (glfwGetWindowParam(GLFW_OPENED) == GL_TRUE)
	{
		glfwSetWindowTitle(_title);
	}
}

int WindowGL::GetFPS()
{
	return m_fps;
}

char* WindowGL::GetContextTypeName()
{
	return "OpenGL";
}