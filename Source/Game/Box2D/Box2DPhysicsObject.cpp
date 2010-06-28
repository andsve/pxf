#include <Pxf/Game/Box2D/Box2DPhysicsObject.h>
#include <Box2D/Common/b2Math.h>
#include <Box2D/Dynamics/b2Body.h>

#define LOCAL_MSG "Box2DPhysicsObject"

using namespace Pxf;
using namespace Game;

Box2DPhysicsObject::Box2DPhysicsObject()
{

}

Box2DPhysicsObject::~Box2DPhysicsObject()
{
	/*
	if(m_Body)
	{
		delete m_Body;
	} */

	m_Body = 0;
}

void Box2DPhysicsObject::SetPosition(Math::Vec3f _Position)
{
	// lol dunno, remove this?
}

Math::Vec3f Box2DPhysicsObject::GetPosition()
{
	b2Vec2 _Vec = m_Body->GetPosition();
	return Math::Vec3f(_Vec.x,_Vec.y,0.0f);
}


