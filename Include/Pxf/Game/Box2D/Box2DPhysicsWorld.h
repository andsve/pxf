#ifndef _PXF_GAME_BOX2D_PHYSICSWORLD_H_
#define _PXF_GAME_BOX2D_PHYSICSWORLD_H_

#include <Pxf/Game/PhysicsWorld.h>
#include <Pxf/Game/Box2D/Box2DParameterDefs.h>
#include <Box2D/Common/b2Settings.h>
#include <Pxf/Math/Vector.h>

// box2d is in a global namespace
class b2World;
class b2Body;

namespace Pxf 
{	
	/* This attempt at forward declaration yields specialization of 'Pxf::Math::Vector3D<float>' after instantiation error..
	   any ideas?
	namespace Math
	{
		class Vector3D<float>;
		typedef Vector3D<float> Vec3f;
	} */
	
	namespace Game
	{	
		struct body_parameters;
		class PhysicsObject;
		class Box2DPhysicsWorld : public PhysicsWorld
			{
			public:
				Box2DPhysicsWorld(Math::Vec2f _Gravity, bool _SleepObjects, float32 _TimeStep, int32 _VelocityIterations, int32 _PositionIterations);
				~Box2DPhysicsWorld();
				
				// TODO: what to do
				void Simulate();
				void ClearForces();
				
				PhysicsObject* CreateBodyFromParams(body_parameters _Params);
				
				b2World* GetWorld() { return m_World; }
				
			private:
				b2World*	m_World;
				bool		m_SleepObjects;
				float32		m_TimeStep;				// = 1.0f / 60.0f;
				int32		m_VelocityIterations;	// = 6;
				int32		m_PositionIterations ;	//= 2;
			};
	} // Game
} // Pxf
#endif