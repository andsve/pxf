
#include <Pxf/Extra/LuaGame/Subsystems/Physics.h>

using namespace Pxf;
using namespace Extra::LuaGame;
using namespace Extra::LuaGame::PhysicsSubsystem;

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
	lua_pushnumber(_L,WORLD_TYPE_BULLET3D);
	lua_setfield(_L,-2,"BULLET3D");
	
	// luagame.physics.BOX2D
	lua_pushnumber(_L,WORLD_TYPE_BOX2D);
	lua_setfield(_L,-2,"BOX2D");
}

int PhysicsSubsystem::NewWorld(lua_State* _L)
{
	int _RetVal = 0;

	int argc = lua_gettop(_L);	
	if(argc == 1 && lua_isnumber(_L,1))
	{
		int _Val = lua_tonumber(_L, 1);
		
		switch(_Val)
		{
			case 1: // Bullet3D
				_RetVal = _NewBullet3DWorld(_L);
				break;
			case 2: // Box2D
				_RetVal = _NewBox2DWorld(_L);
				break;
			default: 
				lua_pushstring(_L,"Unknown physics identifier passed to newworld function");
				lua_error(_L);
				break;
		}
	}
	else
	{
		lua_pushstring(_L,"Invalid argument passed to newworld function!");
		lua_error(_L);
	}
	
	return _RetVal;
}

int PhysicsSubsystem::_NewBullet3DWorld(lua_State* _L)
{
	return 0;
}

int PhysicsSubsystem::_NewBox2DWorld(lua_State* _L)
{
	LuaPhysicsWorld* _NewWorld = new LuaBox2DPhysicsWorld(Math::Vec3f(0.0f,0.0f,0.0f));

	lua_newtable(_L);
    
    lua_pushlightuserdata(_L, _NewWorld);
    lua_setfield(_L, -2, "instance");

	/*
	lua_pushcfunction(_L,_NewWorld->NewBody);
	lua_setfield(_L,-2, "newbody"); */

	/*
	lua_pushcfunction(_L,_NewWorld->Simulate);
	lua_setfield(_L,-2, "simulate"); */

	/*
	lua_pushcfunction(_L,_NewWorld->ClearForces);
	lua_setfield(_L,-2, "clearforces"); */

	return 1;
}

LuaBox2DPhysicsBody::LuaBox2DPhysicsBody()
	: m_Body(0)
{
	
}
LuaBox2DPhysicsBody::~LuaBox2DPhysicsBody() 
{
	if(m_Body)
		delete m_Body;
}

int LuaBox2DPhysicsBody::GetPosition(lua_State* _L) 
{

	return 0;
}

LuaBox2DPhysicsWorld::~LuaBox2DPhysicsWorld()
{
	if(m_World)
		delete m_World;
}

int LuaBox2DPhysicsWorld::Simulate(lua_State* _L)
{
	/*
	if(m_World)
		m_World->Step(m_TimeStep,m_VelocityIterations,m_PositionIterations); */
	return 0;
}

int LuaBox2DPhysicsWorld::ClearForces(lua_State* _L)
{
	return 0;
}

int LuaBox2DPhysicsWorld::NewBody(lua_State* _L) 
{
	LuaBox2DPhysicsBody* _NewBody = new LuaBox2DPhysicsBody();

	lua_newtable(_L);
    
    lua_pushlightuserdata(_L, _NewBody);
    lua_setfield(_L, -2, "instance");

	//lua_pushcfunction(_L, _NewBody->GetPosition);
	//lua_setfield(_L, -2, "position");

	return 1;
}

/*
void LuaPhysicsBody::RegisterClass(lua_State* _L) 
{
	// luagame.physics.body
	/*
	lua_getglobal(_L, LUAGAME_TABLE);
	lua_getfield(_L, -1, "physics");
	lua_newtable(_L);
	lua_setfield(_L, -2, "body");
	lua_getfield(_L, -1, "body");

	// luagame.physics.body.position 
	lua_pushcfunction(_L,GetPosition);
	//lua_setfield(_L, -2, "position"); 
}*/