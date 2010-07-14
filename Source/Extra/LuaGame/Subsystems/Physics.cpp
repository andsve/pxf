
#include <Pxf/Extra/LuaGame/Subsystems/Physics.h>

using namespace Pxf;
using namespace Extra::LuaGame;

void PhysicsSubsystem::RegisterClass(lua_State* _L)
{
    // luagame.physics
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_newtable(_L);
    lua_setfield(_L, -2, "physics");
    lua_getfield(_L, -1, "physics");
    
	// luagame.physics.newworld
	lua_pushcfunction(_L,NewWorld);
	lua_setfield(_L,-2,"newworld");
	
	// push physics engine identifiers 
	// luagame.physics.BULLET3D
	lua_pushnumber(_L,1);
	lua_setfield(_L,-2,"BULLET3D");
	
	// luagame.physics.BOX2D
	lua_pushnumber(_L,2);
	lua_setfield(_L,-2,"BOX2D");
	
	/*
    // luagame.graphics.getscreensize
    lua_pushcfunction(_L, GetScreenSize);
    lua_setfield(_L, -2, "getscreensize");
    
    // luagame.graphics.unbindtexture
    lua_pushcfunction(_L, UnbindTexture);
    lua_setfield(_L, -2, "unbindtexture");
    
    // luagame.graphics.drawquad
    lua_pushcfunction(_L, DrawQuad);
    lua_setfield(_L, -2, "drawquad");
    
    // luagame.graphics.setcolor
    lua_pushcfunction(_L, SetColor);
    lua_setfield(_L, -2, "setcolor");
    
    // luagame.graphics.loadidentity
    lua_pushcfunction(_L, LoadIdentity);
    lua_setfield(_L, -2, "loadidentity");
    
    // luagame.graphics.translate
    lua_pushcfunction(_L, Translate);
    lua_setfield(_L, -2, "translate");
    
    // luagame.graphics.rotate
    lua_pushcfunction(_L, Rotate);
    lua_setfield(_L, -2, "rotate");
	 */
}

int PhysicsSubsystem::NewWorld(lua_State* _L)
{
	int argc = lua_gettop(_L);	
	if(argc == 1 && lua_isnumber(_L,1))
	{
		int _Val = lua_tonumber(_L, 1);
		
		switch(_Val)
		{
			case 1: 
				printf("luls\n"); 
				break;
			case 2:
				printf("bruls\n");
				break;
			default: break;
		}
	}
	else
	{
		lua_pushstring(_L,"Invalid argument passed to newworld function!");
		lua_error(_L);
	}
	
	return 0;
}