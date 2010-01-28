#include <Pxf/Graphics/Device.h>

#include <Pxf/Graphics/Texture.h>
#include <Pxf/Extra/LuaGUI/GUIScript.h>
#include <Pxf/Extra/LuaGUI/Callbacks.h> // lua callbacks

using namespace Pxf;
using namespace Pxf::Math;
using namespace Pxf::Graphics;
using namespace Pxf::Extra::LuaGUI;

GUIScript::GUIScript(const char* _filepath, Math::Vec4i* _viewarea, Graphics::Device* _device)
{
	m_Filepath = _filepath;
	m_Device = _device;
	m_Running = false;

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
			Message("LuaGUI", "-- %s", lua_tostring(L, -1));
			lua_pop(L, 1); // remove error message
		} else {
			m_Running = true;

			// Load theme file
			lua_getglobal(L, "theme_texture");
			m_Texture = m_Device->CreateTexture(lua_tostring(L, 1));
			lua_pop(L, 1);

			// Call init()
			CallLuaFunc("init");
		}

	} else {
		Message("LuaGUI", "%s",lua_tostring(L,-1));
	}
}

void GUIScript::AddQuad(GUIWidget* _widget, Math::Vec4i* _quad, Math::Vec4i* _texpixels)
{
	//printf("inside testit! got: %i\n", _i);
	_widget->AddQuad(_quad, &m_Texture->CreateTextureSubset(_texpixels->x, _texpixels->y, _texpixels->z, _texpixels->w));
}

void GUIScript::Update(Math::Vec2f* _mouse, bool _mouse_down, float _delta)
{
	if (m_Running)
	{

		// Update widgets
		for ( std::list<GUIWidget*>::iterator it = m_Widgets.begin() ; it != m_Widgets.end(); it++ )
		{
			((GUIWidget*)*it)->Update(_mouse, _mouse_down);
		}

		g_CurrentScript.push(this);

		// Call update(_delta)
		lua_getglobal(L, "debug");
		lua_getfield(L, -1, "traceback");
		lua_remove(L, -2);
		lua_getfield(L, LUA_GLOBALSINDEX, "update");
		lua_pushnumber(L, _delta);
		m_Running = HandleLuaErrors(lua_pcall(L, 1, 0, -3));

		g_CurrentScript.pop();
	}
}

void GUIScript::Draw()
{
	if (m_Running)
	{
		// Setup viewport
		m_Device->SetViewport(m_Viewarea[0], m_Viewarea[1], m_Viewarea[2], m_Viewarea[3]);
		Math::Mat4 t_ortho = Math::Mat4::Ortho(0, m_Viewarea[2], m_Viewarea[3], 0, 0, 1);
		m_Device->SetProjection(&t_ortho);
		m_Device->BindTexture(m_Texture);

		for ( std::list<GUIWidget*>::iterator it = m_Widgets.begin() ; it != m_Widgets.end(); it++ )
		{
			((GUIWidget*)*it)->Reset();
		}

		// Setup draw calls
		CallLuaFunc("DrawWidgets");

		// Loop through widgets
		for ( std::list<GUIWidget*>::iterator it = m_Widgets.begin() ; it != m_Widgets.end(); it++ )
		{
			((GUIWidget*)*it)->Draw();
		}
	}
}

GUIWidget* GUIScript::AddWidget(const char* _name, Math::Vec4i _hitbox)
{
	GUIWidget* widget = new GUIWidget(_name, &_hitbox, m_Device);
	m_Widgets.push_front(widget);
	return widget;
}

void GUIScript::CallLuaFunc(const char* _funcname)
{
	g_CurrentScript.push(this);

	// Call update(_delta)
	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	lua_getfield(L, LUA_GLOBALSINDEX, _funcname);
	m_Running = HandleLuaErrors(lua_pcall(L, 0, 0, -2));

	g_CurrentScript.pop();
}

bool GUIScript::HandleLuaErrors(int _error)
{
	if (_error != 0)
	{
		if (_error == LUA_ERRRUN)
			Message("LuaGUI", "-- Script runtime error: --");
		else if (_error == LUA_ERRMEM)
			Message("LuaGUI", "-- Script memory allocation error: --");
		else
			Message("LuaGUI", "-- Fatal script error: --");
		Message("LuaGUI", "%s", lua_tostring(L, -1));

		lua_pop(L, 1); // remove error message
		return false;
	}
	return true;
}