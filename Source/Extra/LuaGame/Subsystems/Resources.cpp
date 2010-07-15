
#include <Pxf/Extra/LuaGame/Subsystems/Resources.h>

#include <Pxf/Graphics/Texture.h>

using namespace Pxf;
using namespace Extra::LuaGame;

void ResourcesSubsystem::RegisterClass(lua_State* _L)
{
    // luagame.graphics
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_newtable(_L);
    lua_setfield(_L, -2, "resources");
    
    lua_getfield(_L, -1, "resources");
    
    // luagame.graphics.loadtexture
    lua_pushcfunction(_L, LoadTexture);
    lua_setfield(_L, -2, "loadtexture");
}

int ResourcesSubsystem::LoadTexture(lua_State* _L)
{
    // luagame.resources.loadtexture(filepath)
    int argc = lua_gettop(_L);
    if (argc == 1 && lua_isstring(_L, 1))
    {
        const char* t_filepath = lua_tostring(_L, 1);
        
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        lua_pop(_L, 2);
        
    
        // Push a table with the following content:
        /**
         * luagame.resources.loadtexture = [instance,    -- Texture class instance
         *                                  filepath     -- returns path to the file
         *                                  bind()       -- binds the texture (ex glBindTexture(...))
         *                                  getsize()    -- returns size of texture in pixels
         *                                 ]
         *
         */
    
        lua_newtable(_L);
    
        lua_pushlightuserdata(_L, g->AddPreload(t_filepath));
        lua_setfield(_L, -2, "instance");
    
        lua_pushstring(_L, t_filepath);
        lua_setfield(_L, -2, "filename");
        
        lua_pushcfunction(_L, BindTexture);
        lua_setfield(_L, -2, "bind");
        
        // texture:getsize()
        lua_pushcfunction(_L, GetTextureSize);
        lua_setfield(_L, -2, "getsize");
    
        return 1;
    
    } else {
        
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to loadtexture function!");
        lua_error(_L);
        
    }
    
    return 0;
}

int ResourcesSubsystem::BindTexture(lua_State* _L)
{
    // texture:bind()
    int argc = lua_gettop(_L);
    if (argc == 1)
    {
        if (!lua_istable(_L, 1))
        {
            lua_pushstring(_L, "Nil value sent to texture.bind()!");
            lua_error(_L);
            return 0;
        }

        lua_getfield(_L, 1, "instance");

        if (!lua_isuserdata(_L, -1))
        {
            lua_pushstring(_L, "'instance' field is not a valid userdata!");
            lua_error(_L);
            return 0;
        }
        
        Graphics::Texture* tex = 0;
        tex = (Graphics::Texture*)lua_touserdata(_L, -1);
                
        // Send data to Game instance VBO
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        
        g->BindTexture(tex);
        
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to texture.bind() function!");
        lua_error(_L);
    }
    
    return 0;
}

int ResourcesSubsystem::GetTextureSize(lua_State* _L)
{
    // w,h = texture:getsize()
    
    int argc = lua_gettop(_L);
    if (argc == 1)
    {
        if (!lua_istable(_L, 1))
        {
            lua_pushstring(_L, "Nil value sent to texture.getsize()!");
            lua_error(_L);
            return 0;
        }

        lua_getfield(_L, 1, "instance");

        if (!lua_isuserdata(_L, -1))
        {
            lua_pushstring(_L, "'instance' field is not a valid userdata!");
            lua_error(_L);
            return 0;
        }
        
        Graphics::Texture* tex = 0;
        tex = (Graphics::Texture*)lua_touserdata(_L, -1);
        
        // Push width and height
        lua_pushnumber(_L, tex->GetWidth());
        lua_pushnumber(_L, tex->GetHeight());
        
        return 2;
        
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to texture.bind() function!");
        lua_error(_L);
    }
    
    return 0;
}

