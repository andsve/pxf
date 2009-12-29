#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/WindowGL.h>

#include <GL/glfw.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

WindowGL::WindowGL(int _width, int _height, bool _fullscreen /* = false */, bool _vsync /* = false */)
{
	m_width = _width;
	m_height = _height;
	m_fullscreen = _fullscreen;
	m_vsync = _vsync;

	m_opened = false;

	m_fps = 0;
	m_fps_count = 0;
	m_fps_laststamp = 0.0;
}

bool WindowGL::Open()
{
	int t_params = GLFW_WINDOW;

	if (m_opened)
		return false; // can't open an already open window

	// fullscreen?
	if (m_fullscreen)
		t_params = GLFW_FULLSCREEN;

	// Enable vertical sync
	if (m_vsync)
		glfwSwapInterval(1);

	if (GL_TRUE == glfwOpenWindow(m_width, m_height, 8, 8, 8, 8, 24, 0, t_params))
	{
		m_opened = true;

		return true;
	}
	else
		return false;

	
}

bool WindowGL::Close()
{
	if (!m_opened)
		return false;

	glfwCloseWindow();
	return true;
}

void WindowGL::Swap()
{
	if (m_opened)
	{
		double t_time_current = glfwGetTime();
		if (t_time_current - m_fps_laststamp >= 1.0)
		{
			m_fps = m_fps_count;
			m_fps_count = 0;
			m_fps_laststamp = t_time_current;
		}

		glfwSwapBuffers();
		m_fps_count += 1;
	}
}

bool WindowGL::IsOpen()
{
	return m_opened;
}

bool WindowGL::IsActive()
{
	if (m_opened)
	{
		if (GL_TRUE == glfwGetWindowParam(GLFW_ACTIVE))
			return true;
	}
	return false;
}

void WindowGL::SetTitle(const char *_title)
{
	if (m_opened)
	{
		glfwSetWindowTitle(_title);
	}
}

int WindowGL::GetFPS()
{
	return m_fps;
}