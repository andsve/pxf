#include <Pxf/Game/Box2D/Box2DPhysicsWorld.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Common/b2Math.h>

using namespace Pxf;
using namespace Game;


Box2DPhysicsWorld::Box2DPhysicsWorld(Math::Vec2f _Gravity, bool _SleepObjects, float32 _TimeStep, int32 _VelocityIterations, int32 _PositionIterations)
	: PhysicsWorld(Math::Vec3f(_Gravity.x,_Gravity.y,0.0f))
	, m_TimeStep(_TimeStep)
	, m_SleepObjects(_SleepObjects)
	, m_VelocityIterations(_VelocityIterations)
	, m_PositionIterations(_PositionIterations)
{
	b2Vec2 _Grav(m_Gravity.x,m_Gravity.y);
	m_World = new b2World(_Grav,m_SleepObjects);
}

Box2DPhysicsWorld::~Box2DPhysicsWorld()
{
	if(m_World)
	{
		delete m_World;
		m_World = 0;
	}
}

void Box2DPhysicsWorld::Simulate()
{
	if(m_World)
		m_World->Step(m_TimeStep,m_VelocityIterations,m_PositionIterations);
}

void Box2DPhysicsWorld::ClearForces()
{
	if(m_World)
		m_World->ClearForces();
}

b2Body* Box2DPhysicsWorld::CreateBodyFromParams(body_parameters _Params)
{
	b2Body* _NewBody;
	
	switch(_Params.po_type)
	{
		case PO_BODY_STATIC: 
			break;
		case PO_BODY_DYNAMIC:
			break;		
	}

	return _NewBody;
}