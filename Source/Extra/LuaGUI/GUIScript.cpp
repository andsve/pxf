#include <Pxf/Graphics/Device.h>

#include <Pxf/Extra/LuaGUI/GUIScript.h>
#include <Pxf/Extra/LuaGUI/Callbacks.h> // lua callbacks

using namespace Pxf;
using namespace Pxf::Math;
using namespace Pxf::Graphics;
using namespace Pxf::Extra::LuaGUI;

GUIScript::GUIScript(const char* _filepath, Vec4i* _viewarea, Device* _device)
{
	m_Filepath = _filepath;
	m_Device = _device;

	m_Viewarea[0] = _viewarea->x;
	m_Viewarea[1] = _viewarea->y;
	m_Viewarea[2] = _viewarea->z;
	m_Viewarea[3] = _viewarea->w;

	// All saved, lets load the script
	Load();
}

GUIScript::~GUIScript()
{
	// clean up
}

void GUIScript::Load()
{
	L = lua_open();
	_register_lua_libs_callbacks(L);

	int s = luaL_loadfile(L, m_Filepath.c_str());
	if (!s)
	{
		s = lua_pcall(L, 0, LUA_MULTRET, 0);

		if ( s ) {
			printf("-- %s \n", lua_tostring(L, -1));
			lua_pop(L, 1); // remove error message
		}

	} else {
		printf("%s\n",lua_tostring(L,-1));
	}
}

void GUIScript::Update(float _delta)
{
	
}

void GUIScript::Draw()
{
	// Setup viewport
	m_Device->SetViewport(m_Viewarea[0], m_Viewarea[1], m_Viewarea[2], m_Viewarea[3]);

	// Loop through widgets
}