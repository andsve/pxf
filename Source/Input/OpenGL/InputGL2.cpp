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

// -- GLFW callbacks
static void char_callback(int _Char, int _Action)
{
	if (_Action == GLFW_PRESS)
		((InputGL2*)Instance)->last_char = _Char;
}

static void key_callback(int _Key, int _Action)
{
	if (_Action == GLFW_PRESS)
		((InputGL2*)Instance)->last_key = _Key;
}
static void mouse_callback(int _Button, int _Action)
{
	if (_Action == GLFW_PRESS)
	{
		((InputGL2*)Instance)->last_button = _Button + MOUSE_1;

		// set bit to 1
		((InputGL2*)Instance)->mouse_buttons |= (1<<_Button);

	}
	else
	{
		// set bit to 0
		((InputGL2*)Instance)->mouse_buttons &= ~(1<<_Button);
	}
}
// END -- GLFW callbacks

InputGL2::InputGL2(Window* _windowptr) : Input(_windowptr)
{
	// Singleton hack for our GLFW callbacks to work
	Instance = this;

	// Reset state vars
	last_char = 0;
	last_key = 0;
	last_button = 0;
	mouse_x = 0;
	mouse_y = 0;
	mouse_scroll = 0;
	mouse_mode = MODE_ABSOLUTE;

	// Setup GLFW callbacks
	glfwSetCharCallback(char_callback);
	glfwSetKeyCallback(key_callback);
	glfwSetMouseButtonCallback(mouse_callback);

	Message(LOCAL_MSG, "Input initiated.");
}

InputGL2::~InputGL2()
{
	// Clean up
}


void InputGL2::Update()
{
	glfwPollEvents();
}

bool InputGL2::IsKeyDown(int _key)
{
	return glfwGetKey(_key) == GLFW_PRESS;
}

bool InputGL2::IsButtonDown(int _button)
{
	return glfwGetMouseButton(_button - MOUSE_1) == GLFW_PRESS;
}

void InputGL2::GetMousePos(int *x, int *y)
{
	if (mouse_mode == MODE_RELATIVE)
	{
		int new_x = 0, new_y = 0;
		glfwGetMousePos(&new_x, &new_y);
		*x = new_x - mouse_x;
		*y = new_y - mouse_y;
		mouse_x = new_x;
		mouse_y = new_y;
	}
	else
	{
		glfwGetMousePos(x, y);
	}
}

void InputGL2::SetMouseMode(MOUSE_MODE _Mode)
{
	if (_Mode == MODE_RELATIVE)
	{
		glfwDisable(GLFW_MOUSE_CURSOR);
		glfwGetMousePos(&mouse_x, &mouse_y);
	}
	else
	{
		glfwEnable(GLFW_MOUSE_CURSOR);
	}

	mouse_mode = _Mode;
}

MOUSE_MODE InputGL2::GetMouseMode()
{
	return mouse_mode;
}

void InputGL2::SetMousePos(int x, int y)
{
	glfwSetMousePos(x, y);
}

void InputGL2::ShowCursor(bool _show)
{
	if (_show)
		glfwEnable(GLFW_MOUSE_CURSOR);
	else
		glfwDisable(GLFW_MOUSE_CURSOR);
}

// -- Cached keys, chars and button presses
int InputGL2::GetLastKey()
{
	return last_key;
}

int InputGL2::GetLastChar()
{
	return last_char;
}

int InputGL2::GetLastButton()
{
	return last_button;
}

void InputGL2::ClearLastKey()
{
	last_key = 0;
}

void InputGL2::ClearLastChar()
{
	last_char = 0;
}

void InputGL2::ClearLastButton()
{
	last_button = 0;
}