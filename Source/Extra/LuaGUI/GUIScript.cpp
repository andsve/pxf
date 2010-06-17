#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/QuadBatch.h>

#include <Pxf/Graphics/Texture.h>
#include <Pxf/Extra/LuaGUI/GUIScript.h>
#include <Pxf/Extra/LuaGUI/Callbacks.h> // lua callbacks

using namespace Pxf;
using namespace Pxf::Math;
using namespace Pxf::Graphics;
using namespace Pxf::Extra;
using namespace Pxf::Extra::LuaGUI;

GUIScript::GUIScript(const char* _filepath, Math::Vec4i* _viewarea, Graphics::Device* _device)
{
	m_Filepath = _filepath;
	m_Device = _device;
	m_Running = false;
	m_ShouldReload = false;

	m_Viewarea[0] = _viewarea->x;
	m_Viewarea[1] = _viewarea->y;
	m_Viewarea[2] = _viewarea->z;
	m_Viewarea[3] = _viewarea->w;
	
	m_QuadBatch = m_Device->CreateQuadBatch(PXF_EXTRA_LUAGUI_MAXQUAD_PER_WIDGET);
	m_Font = new SimpleFont(m_Device);

	// All saved, lets load the script
	Load();
}

GUIScript::~GUIScript()
{
	if (m_QuadBatch)
		delete m_QuadBatch;
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
			
			// Load font file
			lua_getglobal(L, "theme_font_file");
			lua_getglobal(L, "theme_font_size");
			m_Font->Load(lua_tostring(L, 1), lua_tonumber(L, 2));
			//m_Texture = m_Device->CreateTexture(lua_tostring(L, 1));
			lua_pop(L, 2);

			// Call init()
			CallLuaFunc("init");
		}

	} else {
		Message("LuaGUI", "%s",lua_tostring(L,-1));
	}
}

void GUIScript::Unload()
{
	m_Running = false;

	delete m_Texture;

	for ( std::list<GUIWidget*>::iterator it = m_Widgets.begin() ; it != m_Widgets.end(); it++ )
	{
		GUIWidget* widget = ((GUIWidget*)*it);
		delete widget;
	}

	m_Widgets.clear();

	lua_close(L);
}

void GUIScript::Reload()
{
	m_ShouldReload = true;
}

void GUIScript::AddQuad(GUIWidget* _widget, Math::Vec4i* _quad, Math::Vec4i* _texpixels)
{
	Math::Vec4f coords = m_Texture->CreateTextureSubset(_texpixels->x, _texpixels->y, _texpixels->z, _texpixels->w);
	
	m_QuadBatch->SetTextureSubset(coords.x, coords.y, coords.z, coords.w);
	m_QuadBatch->AddTopLeft(_widget->GetPosition()->x + _quad->x, _widget->GetPosition()->y + _quad->y, _quad->z, _quad->w);
}

void GUIScript::AddText(GUIWidget* _widget, Util::String _text, Math::Vec3f _pos)
{
	_pos.x += _widget->GetPosition()->x;
	_pos.y += _widget->GetPosition()->y;
	m_Font->AddText(_text, _pos);
}

void GUIScript::AddTextCentered(GUIWidget* _widget, Util::String _text, Math::Vec3f _pos)
{
	_pos.x += _widget->GetPosition()->x;
	_pos.y += _widget->GetPosition()->y;
	m_Font->AddTextCentered(_text, _pos);
}

void GUIScript::Update(Math::Vec2f* _mouse, bool _mouse_down, float _delta)
{
	// Reload script if needed
	if (m_ShouldReload)
	{
		Unload();
		Load();
		m_ShouldReload = false;
	}

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
		lua_getfield(L, LUA_GLOBALSINDEX, "UpdateWidgets");
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

		// Reset quad batch an make ready for script to send draw-data
		m_QuadBatch->Reset();
		m_Font->ResetText();

		// Setup draw calls
		CallLuaFunc("DrawWidgets");

		// Draw our quad batch!
		m_QuadBatch->Draw();
		m_Font->Draw();
	}
}

bool GUIScript::MessagePump(ScriptMessage* _pmessage)
{
  if (!m_Messages.empty())
  {
    ScriptMessage* tmpmessage = m_Messages.back();
    _pmessage->id = tmpmessage->id;
    _pmessage->data = tmpmessage->data;
		_pmessage->script = this;
    m_Messages.pop_back();
    return true;
  }
  
  return false;
}

GUIWidget* GUIScript::AddWidget(const char* _name, Math::Vec4i _hitbox)
{
	GUIWidget* widget = new GUIWidget(_name, &_hitbox, m_Device);
	m_Widgets.push_front(widget);
	return widget;
}

void GUIScript::SendMessageInternal(GUIWidget* _widget, int _messageid, void* _data)
{
  ScriptMessage* _message = new ScriptMessage();
  _message->id = _messageid;
  _message->data = _data;
	_message->script = this;
  
  m_Messages.push_front(_message);
}

void GUIScript::SendMessage(int _messageid, char* _data)
{
  g_CurrentScript.push(this);

	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	lua_getfield(L, LUA_GLOBALSINDEX, "_RecieveMessage");
	lua_pushnumber(L, _messageid);
	lua_pushstring(L, _data);
	m_Running = HandleLuaErrors(lua_pcall(L, 2, 0, -4));

	g_CurrentScript.pop();
}

void GUIScript::CallLuaFunc(const char* _funcname)
{
	g_CurrentScript.push(this);

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