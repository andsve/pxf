#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/WindowGL.h>

#include <GL/glfw.h>

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

WindowGL::WindowGL(int _width, int _height, bool _fullscreen /* = false */)
{
	m_width = _width;
	m_height = _height;
	m_fullscreen = _fullscreen;

	m_opened = false;
}

bool WindowGL::Open()
{
	int t_params = GLFW_WINDOW;

	if (m_opened)
		return false; // can't open an already open window

	// fullscreen?
	if (m_fullscreen)
		t_params = GLFW_FULLSCREEN;

	if (GL_TRUE == glfwOpenWindow(m_width, m_height, 8, 8, 8, 8, 24, 0, t_params))
		return true;
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