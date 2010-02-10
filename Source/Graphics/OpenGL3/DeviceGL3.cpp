#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL3/DeviceGL3.h>
#include <Pxf/Graphics/OpenGL3/WindowGL3.h>
#include <Pxf/Base/Debug.h>

#define LOCAL_MSG "DeviceGL3"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

DeviceGL3::DeviceGL3()
{
	Message(LOCAL_MSG, "SUCK ON IT!");
}

DeviceGL3::~DeviceGL3()
{
	CloseWindow();
}

Window* DeviceGL3::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	m_Window = new WindowGL3(_pWindowSpecs);
	m_Window->Open();
	return m_Window;
}

void DeviceGL3::CloseWindow()
{
	if (m_Window)
	{
		delete m_Window;
	}
}

void DeviceGL3::SwapBuffers()
{
	if (m_Window)
	{
		m_Window->Swap();
	}
}