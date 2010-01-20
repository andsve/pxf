#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Input/OpenGL/InputGL2.h>
#include <Pxf/Base/Debug.h>

#include <GL/glfw.h>

#define LOCAL_MSG "DeviceGL2"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

DeviceGL2::DeviceGL2()
{
	// Initialize GLFW
	if (glfwInit() != GL_TRUE)
	{
		Message(LOCAL_MSG, "Could not initialize GLFW!");
		return;
	}

	Message(LOCAL_MSG, "Device initiated.");
	
}

DeviceGL2::~DeviceGL2()
{
	// Close any open window.
	CloseWindow();
	

	// Terminate GLFW
	glfwTerminate();
}

Window* DeviceGL2::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	m_Window = new WindowGL2(_pWindowSpecs);
	m_Window->Open();
	return m_Window;
}

void DeviceGL2::CloseWindow()
{
	if (m_Window && m_Window->IsOpen())
	{
		m_Window->Close();
		delete m_Window;
	}
}

void DeviceGL2::SwapBuffers()
{
	if (m_Window)
	{
		m_Window->Swap();
	}
}

