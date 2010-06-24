
#include <Pxf/Extra/LuaGame/LuaGame.h>

#define LOCAL_MSG "LuaGame"

using namespace Pxf;
using namespace Pxf::Extra;

LuaGame::LuaGame(Util::String _gameFilename, Graphics::Device* _device)
{
    m_Device = _device;
    
    m_GameFilename = _gameFilename;
    m_GameIdent = "Unknown Game";
    
    // Reset version
    m_GameVersion[0] = 0;
    m_GameVersion[1] = 0;
    m_GameVersion[2] = 0;
    
    m_Running = false;
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
    
    // Load game script   
    int s = luaL_loadfile(L, m_GameFilename.c_str());
	if (!s)
	{
		s = lua_pcall(L, 0, LUA_MULTRET, 0);

		if ( s ) {
			Message(LOCAL_MSG, "-- %s", lua_tostring(L, -1));
			lua_pop(L, 1); // remove error message
		} else {
			m_Running = true;
						
			// Load font file
			/*lua_getglobal(L, "theme_font_file");
			lua_getglobal(L, "theme_font_size");
			m_Font->Load(lua_tostring(L, 1), lua_tonumber(L, 2));
			//m_Texture = m_Device->CreateTexture(lua_tostring(L, 1));
			lua_pop(L, 2);
			*/

			// Call init()
			//CallLuaFunc("init");
		}

	} else {
		Message(LOCAL_MSG, "Error: %s",lua_tostring(L,-1));
	}
    
    return true;
}

int LuaGame::PreLoad()
{
    // Preload game data
    
    // Returns how many preload data pieces there are left
    return 0;
}

bool LuaGame::Update(float dt)
{
    // Update game
    
    return true;
}

bool LuaGame::Render()
{
    // Render game
    
    return true;
}

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
}

