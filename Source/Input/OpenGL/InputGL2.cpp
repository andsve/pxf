#include <Pxf/Pxf.h>
#include <Pxf/Util/String.h>
#include <Pxf/Graphics/Window.h>
#include <Pxf/Input/OpenGL/InputGL2.h>
#include <Pxf/Base/Debug.h>

#include <GL/glfw.h>

#define LOCAL_MSG "InputGL2"

using namespace Pxf;
using namespace Pxf::Input;
using namespace Pxf::Graphics;
using Util::String;

InputGL2::InputGL2(Window* _windowptr) : Input(_windowptr)
{
	Message(LOCAL_MSG, "Input initiated.");

}

InputGL2::~InputGL2()
{
	// Clean up
}


void InputGL2::Update()
{
	// TODO: Update input device...
}