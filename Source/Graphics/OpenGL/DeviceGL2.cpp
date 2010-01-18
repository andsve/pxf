#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/OpenGL/DeviceGL2.h>
#include <Pxf/Base/Debug.h>

#define LOCAL_MSG "DeviceGL2"

using namespace Pxf;
using namespace Pxf::Graphics;
using Util::String;

DeviceGL2::DeviceGL2()
{
	Message(LOCAL_MSG, "HEY NOW!");
}

DeviceGL2::~DeviceGL2()
{
	CloseWindow();
}

Window* DeviceGL2::OpenWindow(WindowSpecifications* _pWindowSpecs)
{
	m_Window = new WindowGL(_pWindowSpecs);
	m_Window->Open();
	return m_Window;
}

void DeviceGL2::CloseWindow()
{
	if (m_Window)
	{
		delete m_Window;
	}
}