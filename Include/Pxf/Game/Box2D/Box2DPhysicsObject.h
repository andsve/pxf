#ifndef _PXF_GAME_BOX2DPHYSICSOBJECT_H_
#define _PXF_GAME_BOX2DPHYSICSOBJECT_H_

#include <Pxf/Game/PhysicsObject.h>

class b2Body;

namespace Pxf 
{
	namespace Game
	{
		class Box2DPhysicsObject : public PhysicsObject
		{
		public:
			Box2DPhysicsObject(b2World* _World);
			~Box2DPhysicsObject();
				
			void SetPosition(Math::Vec3f _Position);
			Math::Vec3f GetPosition();
			
			b2Body* GetBody() { return m_Body; }
		private:
			b2Body* m_Body;
		};
	} // Game
} // Pxf
#endif