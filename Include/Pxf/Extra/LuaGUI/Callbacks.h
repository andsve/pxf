#ifndef _PXF_EXTRA_GUICALLBACKS_H_
#define _PXF_EXTRA_GUICALLBACKS_H_

#include <list>

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

	//////////////////////////////////////////////////////////////////////////
	// Static Lua callbacks
	//////////////////////////////////////////////////////////////////////////

	static int _guicb_print(lua_State *L)
	{
		int n=lua_gettop(L);
		int i;
		for (i=1; i<=n; i++)
		{
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
		lua_register(L,"print", _guicb_print);
	}

} // Pxf

#endif // _PXF_EXTRA_GUICALLBACKS_H_