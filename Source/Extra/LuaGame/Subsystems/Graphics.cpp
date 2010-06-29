
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
    
    // luagame.graphics.getscreensize
    lua_pushcfunction(_L, GetScreenSize);
    lua_setfield(_L, -2, "getscreensize");
    
    // luagame.graphics.drawquad
    lua_pushcfunction(_L, DrawQuad);
    lua_setfield(_L, -2, "drawquad");
    
    // luagame.graphics.translate
    lua_pushcfunction(_L, Translate);
    lua_setfield(_L, -2, "translate");
    
    // luagame.graphics.rotate
    lua_pushcfunction(_L, Rotate);
    lua_setfield(_L, -2, "rotate");
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
        
        Math::Vec4f coords = Math::Vec4f(0, 0, 1, 1);//m_Texture->CreateTextureSubset(_texpixels->x, _texpixels->y, _texpixels->z, _texpixels->w);

    	g->m_QuadBatch->SetTextureSubset(coords.x, coords.y, coords.z, coords.w);
    	g->m_QuadBatch->AddTopLeft(lua_tonumber(_L, 1), lua_tonumber(_L, 2), lua_tonumber(_L, 3), lua_tonumber(_L, 4));
        
        //g->AddQuad(lua_tonumber(_L, 1), lua_tonumber(_L, 2), lua_tonumber(_L, 3), lua_tonumber(_L, 4));
        
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to drawquad function!");
        lua_error(_L);
    }
    
    return 0;
}

int GraphicsSubsystem::Translate(lua_State* _L)
{
    // luagame.graphics.translate(dx,dy)
    int argc = lua_gettop(_L);
    if (argc == 2 && lua_isnumber(_L, 1) && lua_isnumber(_L, 2))
    {
        // Send data to Game instance VBO
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        
        g->m_QuadBatch->Translate(lua_tonumber(_L, 1), lua_tonumber(_L, 2));
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to translate function!");
        lua_error(_L);
    }
    
    return 0;
}

int GraphicsSubsystem::Rotate(lua_State* _L)
{
    // luagame.graphics.rotate(angle)
    int argc = lua_gettop(_L);
    if (argc == 1 && lua_isnumber(_L, 1))
    {
        // Send data to Game instance VBO
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        
        g->m_QuadBatch->SetRotation(lua_tonumber(_L, 1));
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to rotate function!");
        lua_error(_L);
    }
    
    return 0;
}

int GraphicsSubsystem::GetScreenSize(lua_State* _L)
{
    int w,h;
    
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_getfield(_L, -1, "Instance");
    Game* g = (Game*)lua_touserdata(_L, -1);
    
    g->m_Device->GetSize(&w, &h);
    lua_pushnumber(_L, w);
    lua_pushnumber(_L, h);
    
    return 2;
}

