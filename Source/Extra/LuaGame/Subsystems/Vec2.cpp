#include <Pxf/Extra/LuaGame/Subsystems/Vec2.h>

using namespace Pxf;
using namespace Pxf::Extra::LuaGame;

Vec2::Vec2()
{
    x = 13.0f;
    y = 37.0f;
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
    
    lua_getfield(_L, -1, "vec2");
    
    // luagame.vec2.new
    lua_pushcfunction(_L, New);
    lua_setfield(_L, -2, "new");
    
    // luagame.vec2.tostring
    //lua_pushcfunction(_L, ToString);
    //lua_setfield(_L, -2, "tostring");
}

int Vec2::New(lua_State* _L)
{
    Vec2* obj = new Vec2();
    
    // Push a table with the following content:
    /**
     * luagame.vec2.new = [instance,    -- Vec2 class instance
     *                     tostring()   -- returns the vector as a string
     *                    ]
     *
     */
    
    lua_newtable(_L);
    
    lua_pushlightuserdata(_L, obj);
    lua_setfield(_L, -2, "instance");
    
    lua_pushcfunction(_L, ToString);
    lua_setfield(_L, -2, "tostring");
    
    return 1;
}

int Vec2::ToString(lua_State* _L)
{
    if (!lua_istable(_L, -1))
    {
        lua_pushstring(_L, "Nil value sent to vec2.tostring!");
        lua_error(_L);
        return 0;
    }
    
    lua_getfield(_L, -1, "instance");
    
    if (!lua_isuserdata(_L, -1))
    {
        lua_pushstring(_L, "'instance' field is not a valid userdata!");
        lua_error(_L);
        return 0;
    }
    
    Vec2* obj = (Vec2*)lua_touserdata(_L, -1);
    char tbuff[1024];
    sprintf(tbuff, "x: %f, y: %f", obj->x, obj->y);
    lua_pushstring(_L, tbuff);
    
    return 1;
}

