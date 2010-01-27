#ifndef _PXF_EXTRA_GUICALLBACKS_H_
#define _PXF_EXTRA_GUICALLBACKS_H_

#include <stack>

#include <Pxf/Math/Vector.h>
#include <Pxf/Extra/LuaGUI/LuaGUI.h>

// Lua includes
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace Pxf
{
namespace Extra
{
namespace LuaGUI
{
	//////////////////////////////////////////////////////////////////////////
	// Current active GUIScript
	//////////////////////////////////////////////////////////////////////////
	static std::stack<GUIScript*> g_CurrentScript;

	//////////////////////////////////////////////////////////////////////////
	// Static Lua callbacks
	//////////////////////////////////////////////////////////////////////////

	static int _guicb_print(lua_State *L)
	{
		int n=lua_gettop(L);
		int i;
		for (i=1; i<=n; i++)
		{
			//(*g_CurrentScript.top()).testit(lua_tointeger(L, i));

			if (i>1) printf("\t");
			if (lua_isstring(L,i))
				printf("%s",lua_tostring(L,i));
			else if (lua_isnil(L,i))
				printf("%s","nil");
			else if (lua_isboolean(L,i))
				printf("%s",lua_toboolean(L,i) ? "true" : "false");
			else
				printf("%s:%p",luaL_typename(L,i),lua_topointer(L,i));
		}
		printf("\n");
		return 0;
	}

	static int _guicb_AddWidget(lua_State *L)
	{
		int n = lua_gettop(L); // arguments
		if (n == 5)
		{
			GUIWidget* widget = (*g_CurrentScript.top()).AddWidget(lua_tostring(L,1), Math::Vec4i(lua_tonumber(L,2), lua_tonumber(L,3), lua_tonumber(L,4), lua_tonumber(L,5)));
			lua_pushlightuserdata(L, (void*)widget);
			return 1;
		} else {
			Message(PXF_LUAGUI_MESSAGE_ID, "Wrong number (%i instead of 6) of parameters to AddWidget(...).", n);
		}
		return 0;
	}

	static int _guicb_AddState(lua_State *L)
	{
		int n = lua_gettop(L); // arguments
		if (n == 2)
		{
			GUIWidget* widget = (GUIWidget*)lua_touserdata(L, 1);
			widget->AddState(lua_tostring(L, 2));
			return 0;
		} else {
			Message(PXF_LUAGUI_MESSAGE_ID, "Wrong number (%i instead of 2) of parameters to AddState(...).", n);
		}
		return 0;
	}

	static int _guicb_SetState(lua_State *L)
	{
		int n = lua_gettop(L); // arguments
		if (n == 2)
		{
			GUIWidget* widget = (GUIWidget*)lua_touserdata(L, 1);
			widget->SetState(lua_tostring(L, 2));
			return 0;
		} else {
			Message(PXF_LUAGUI_MESSAGE_ID, "Wrong number (%i instead of 2) of parameters to SetState(...).", n);
		}
		return 0;
	}

	static int _guicb_GetState(lua_State *L)
	{
		int n = lua_gettop(L); // arguments
		if (n == 1)
		{
			GUIWidget* widget = (GUIWidget*)lua_touserdata(L, 1);
			lua_pushstring(L, widget->GetState());
			return 1;
		} else {
			Message(PXF_LUAGUI_MESSAGE_ID, "Wrong number (%i instead of 1) of parameters to GetState(...).", n);
		}
		return 0;
	}

	static int _guicb_SetPosition(lua_State *L)
	{
		int n = lua_gettop(L); // arguments
		if (n == 3)
		{
			GUIWidget* widget = (GUIWidget*)lua_touserdata(L, 1);
			widget->SetPosition(Math::Vec2f(lua_tonumber(L, 2), lua_tonumber(L, 3)));
			return 0;
		} else {
			Message(PXF_LUAGUI_MESSAGE_ID, "Wrong number (%i instead of 3) of parameters to SetPosition(...).", n);
		}
		return 0;
	}

	static int _guicb_AddQuad(lua_State *L)
	{
		int n = lua_gettop(L); // arguments
		if (n == 9) // AddQuad(instance, x1, y1, x2, y2, tex_x1, tex_y1, tex_x2, tex_y2)
		{
			Math::Vec4i quad(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4), lua_tonumber(L, 5));
			Math::Vec4i texcoord(lua_tonumber(L, 6), lua_tonumber(L, 7), lua_tonumber(L, 8), lua_tonumber(L, 9));
			GUIWidget* widget = (GUIWidget*)lua_touserdata(L, 1);

			(*g_CurrentScript.top()).AddQuad(widget, &quad, &texcoord);
			
			return 0;
		} else {
			Message(PXF_LUAGUI_MESSAGE_ID, "Wrong number (%i instead of 6) of parameters to AddWidget(...).", n);
		}
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// Register all the above callbacks
	//////////////////////////////////////////////////////////////////////////
	static void _register_lua_libs_callbacks(lua_State *L)
	{
		// Lua libs
		static const luaL_Reg lualibs[] = {
			{"", luaopen_base},
			{LUA_LOADLIBNAME, luaopen_package},
			{LUA_TABLIBNAME, luaopen_table},
			//{LUA_IOLIBNAME, luaopen_io},
			//{LUA_OSLIBNAME, luaopen_os},
			{LUA_STRLIBNAME, luaopen_string},
			{LUA_MATHLIBNAME, luaopen_math},
			{LUA_DBLIBNAME, luaopen_debug},
			{NULL, NULL}
		};

		for (const luaL_Reg *lib = lualibs; lib->func; lib++) {
			lua_pushcfunction(L, lib->func);
			lua_pushstring(L, lib->name);
			lua_call(L, 1, 0);
		}


		// GUI callbacks
		lua_register(L,"print",         _guicb_print);
		lua_register(L,"_AddWidget",    _guicb_AddWidget);
		lua_register(L,"_AddQuad",      _guicb_AddQuad);
		lua_register(L,"_AddState",     _guicb_AddState);
		lua_register(L,"_SetState",     _guicb_SetState);
		lua_register(L,"_GetState",     _guicb_GetState);
		lua_register(L,"_SetPosition",  _guicb_SetPosition);
	}
} // LuaGUI
} // Extra
} // Pxf

#endif // _PXF_EXTRA_GUICALLBACKS_H_