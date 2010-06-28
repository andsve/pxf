#include <Pxf/Game/Box2D/Box2DPhysicsWorld.h>
#include <Box2D/Dynamics/b2World.h>
#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Dynamics/b2Fixture.h>

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
	b2Body*		_NewBody;
	b2BodyDef	_NewBodyDef;
	b2FixtureDef _NewBodyFixture;
	
	
	// TODO: redo this.. fugly code
	switch(_Params.po_type)
	{
		_NewBodyDef.position.Set(_Params.position.x,_Params.position.y);
			
		case PO_BODY_STATIC: 
			_NewBody = m_World->CreateBody(&_NewBodyDef);
			if(_Params.shape_type == b2Shape::e_polygon)
			{
				printf("static\n");
				b2PolygonShape _NewShape;
				_NewShape.SetAsBox(_Params.half_size.x,_Params.half_size.x);
				_NewBody->CreateFixture(&_NewShape,_Params.density);
			}
			else
			{
				// circle parameters
			}
			
			break;
		case PO_BODY_DYNAMIC:
			_NewBodyDef.type = b2_dynamicBody;
			_NewBody = m_World->CreateBody(&_NewBodyDef);
			
			if(_Params.shape_type == b2Shape::e_polygon)
			{
				printf("dynamic.pos: %f,%f\n", _Params.position.x,_Params.position.y);
				b2PolygonShape _NewShape;
				_NewShape.SetAsBox(_Params.half_size.x,_Params.half_size.x);
				_NewBodyFixture.shape = &_NewShape;
				_NewBodyFixture.density = _Params.density;
				_NewBodyFixture.friction = _Params.friction;
				_NewBody->CreateFixture(&_NewBodyFixture);
			}
			else
			{
				// circle parameters
			}
		
			break;		
	}

	return _NewBody;
}
/*
b2BodyDef		bodyDef1;
b2FixtureDef	fixtureDef1;
b2PolygonShape	dynamicBox1;

bodyDef1.type = b2_dynamicBody;
bodyDef1.position.Set(5.0f, 4.0f);
body1 = world.CreateBody(&bodyDef1);

dynamicBox1.SetAsBox(0.5f, 0.5f);

fixtureDef1.shape = &dynamicBox1;
fixtureDef1.density = 1.0f;
fixtureDef1.friction = 0.3f;

body1->CreateFixture(&fixtureDef1); */