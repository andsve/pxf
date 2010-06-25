
#include <Pxf/Extra/LuaGame/LuaGame.h>

#define LOCAL_MSG "LuaGame"

#define LUAGAME_TABLE "LuaGame"

using namespace Pxf;
using namespace Pxf::Extra;

LuaGame::LuaGame(Util::String _gameFilename, Graphics::Device* _device, bool _gracefulFail)
{
    m_Device = _device;
    
    m_GameFilename = _gameFilename;
    m_GameIdent = "Unknown Game";
    
    // Reset version
    m_GameVersion = "0.0.0";
    
    m_Running = false;
    m_GracefulFail = _gracefulFail;
}

LuaGame::~LuaGame()
{
    // Destructor of LuaGame
    // Do nothing?
}

bool LuaGame::Load()
{   
    // Init lua state
    L = lua_open();
    
    // Register lua libs
    _register_lua_libs_callbacks();
    
    ////////////////////////////////
    // Load game core/helper script
    int s = luaL_loadfile(L, "LuaGame.lua");
    if (!s)
	{
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
		if ( s ) {
			Message(LOCAL_MSG, " [Error while running LuaGame.lua] -- %s", lua_tostring(L, -1));
			lua_pop(L, 1); // remove error message
		} else {
		    
		    // Set LuaGame.Instance to this class instance!
		    // Instance will later be used to call correct LuaGame instance.
            lua_getglobal(L, LUAGAME_TABLE);
            lua_pushlightuserdata(L, this);
            lua_setfield(L, -2, "Instance");
            lua_pop(L, 1);
		    
		    // Call core init function (LuaGame:CoreInit())
		    if (!CallGameMethod("CoreInit"))
                return false;
		    
		    //////////////////////////
			// Load main game script   
            s = luaL_loadfile(L, m_GameFilename.c_str());
        	if (!s)
        	{
        		s = lua_pcall(L, 0, LUA_MULTRET, 0);

        		if ( s ) {
        			Message(LOCAL_MSG, "-- %s", lua_tostring(L, -1));
        			lua_pop(L, 1);
        		} else {
        			// Call LuaGame:Init()
                    m_Running = CallGameMethod("Init");
        		}

        	} else {
        		Message(LOCAL_MSG, "Error: %s",lua_tostring(L,-1));
        	}
		}

	} else {
		Message(LOCAL_MSG, "Error while loading LuaGame.lua: %s",lua_tostring(L,-1));
	}
    
    return true;
}

int LuaGame::PreLoad()
{
    // Preload game data
    if (!m_Running)
        return false;
    
    // Returns how many preload data pieces there are left
    return 0;
}

bool LuaGame::Update(float dt)
{
    // Update game
    if (!m_Running)
        return false;
        
    lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	lua_getglobal(L, LUAGAME_TABLE);
    lua_getfield(L, -1, "Update");
    lua_remove(L, -2);
    lua_getglobal(L, LUAGAME_TABLE);
    lua_pushnumber(L, dt);
	m_Running = HandleLuaErrors(lua_pcall(L, 2, 0, -4));
        
    return m_Running;
}

bool LuaGame::Render()
{
    // Render game
    if (!m_Running)
        return false;
    
    // Are we in need of preloading anything?
    if (PreLoad() > 0)
    {
        // TODO: Render loading bar
    } else {
        m_Running = CallGameMethod("Render");
    }
        
    return m_Running;
}


///////////////////////////////////////////////////////////////////
// Private methods
void LuaGame::_register_lua_libs_callbacks()
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
	
	// Register own callbacks
	lua_register(L, "print", Print);
}

bool LuaGame::HandleLuaErrors(int _error)
{
	if (_error != 0)
	{
		if (_error == LUA_ERRRUN)
			Message(LOCAL_MSG, "-- Script runtime error: --");
		else if (_error == LUA_ERRMEM)
			Message(LOCAL_MSG, "-- Script memory allocation error: --");
		else
			Message(LOCAL_MSG, "-- Fatal script error: --");
		Message(LOCAL_MSG, "%s", lua_tostring(L, -1));

		lua_pop(L, 1); // remove error message
		return false;
	}
	return true;
}

bool LuaGame::CallGameMethod(const char* _method)
{
    lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	lua_getglobal(L, LUAGAME_TABLE);
    lua_getfield(L, -1, _method);
    lua_remove(L, -2);
    lua_getglobal(L, LUAGAME_TABLE);
	return HandleLuaErrors(lua_pcall(L, 1, 0, -3));
}

void* LuaGame::GetInstance(lua_State *_L)
{
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_getfield(_L, -1, "Instance");
    static void* p = lua_touserdata(_L, -1);
    lua_pop(_L, 2);
    
    return p;
}

///////////////////////////////////////////////////////////////////
// Callback methods

int LuaGame::Print(lua_State *_L)
{
    LuaGame* instance = (LuaGame*)GetInstance(_L);
    //printf("DEBUGGG: %s\n", "LOL SUP");
    
    // More or less copy paste from lbaselib.c of Lua dist.
    // Modified so that prints can be pushed to the "game console"
    int n = lua_gettop(_L);  /* number of arguments */
    int i;
    lua_getglobal(_L, "tostring");
    for (i=1; i<=n; i++) {
        const char *s;
        lua_pushvalue(_L, -1);  /* function to be called */
        lua_pushvalue(_L, i);   /* value to print */
        lua_call(_L, 1, 1);
        s = lua_tostring(_L, -1);  /* get result */
        if (s == NULL)
            return luaL_error(_L, LUA_QL("tostring") " must return a string to "
                                  LUA_QL("print"));
        if (i>1)
            fputs("\t", stdout);
        fputs("> ", stdout);
        fputs(s, stdout);
        lua_pop(_L, 1);  /* pop result */
    }
    fputs("\n", stdout);
    return 0;
}

