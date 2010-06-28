
#include <Pxf/Extra/LuaGame/Subsystems/Graphics.h>

using namespace Pxf;
using namespace Extra::LuaGame;

void GraphicsSubsystem::RegisterClass(lua_State* _L)
{
    // luagame.graphics
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_newtable(_L);
    lua_setfield(_L, -2, "graphics");
    
    lua_getfield(_L, -1, "graphics");
    
    // luagame.graphics.drawquad
    lua_pushcfunction(_L, DrawQuad);
    lua_setfield(_L, -2, "drawquad"); 
}

int GraphicsSubsystem::DrawQuad(lua_State* _L)
{
    // luagame.graphics.drawquad(x, y, w, w)
    int argc = lua_gettop(_L);
    if (argc == 4 &&
        lua_isnumber(_L, 1) && lua_isnumber(_L, 2) && lua_isnumber(_L, 3) && lua_isnumber(_L, 4))
    {
        // Send data to Game instance VBO
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        
        g->AddQuad(lua_tonumber(_L, 1), lua_tonumber(_L, 2), lua_tonumber(_L, 3), lua_tonumber(_L, 4));
        
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to drawquad method!");
        lua_error(_L);
    }
    
    return 0;
}
