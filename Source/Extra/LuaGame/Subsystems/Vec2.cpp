#include <Pxf/Extra/LuaGame/Subsystems/Vec2.h>

using namespace Pxf;
using namespace Pxf::Extra::LuaGame;

Vec2::Vec2()
{
    // Do nothing.
}

Vec2::~Vec2()
{
    // Nothing..
}

void Vec2::RegisterClass(lua_State* _L)
{
    // luagame.vec2
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_newtable(_L);
    lua_setfield(_L, -2, "vec2");
    
    // luagame.vec2.new
    
    // luagame.vec2.ToString
    lua_getfield(_L, -1, "vec2");
    lua_pushcfunction(_L, ToString);
    lua_setfield(_L, -2, "ToString");
}

int Vec2::ToString(lua_State* _L)
{
    lua_pushstring(_L, "2D Vector");
    
    return 1;
}

