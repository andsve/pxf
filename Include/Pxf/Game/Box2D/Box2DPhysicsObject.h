#ifndef _PXF_GAME_BOX2D_BOX2DPHYSICSOBJECT_H_
#define _PXF_GAME_BOX2D_BOX2DPHYSICSOBJECT_H_

#include <Pxf/Game/PhysicsObject.h>

class b2Body;

namespace Pxf 
{
	namespace Game
	{
		class Box2DPhysicsObject : public PhysicsObject
		{
		public:
			Box2DPhysicsObject(b2Body* _Body = 0) 
				: m_Body(0) {
					m_Body = _Body;
				}
			~Box2DPhysicsObject();
				
			void SetPosition(Math::Vec3f _Position);
			Math::Vec3f GetPosition();
			
			b2Body* GetBody() { return m_Body; }
			void SetBody(b2Body* _Body) { m_Body = _Body; }
		private:
			b2Body* m_Body;
		};
	} // Game
} // Pxf
#endif