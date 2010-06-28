#ifndef _PXF_GAME_PHYSICSWORLD_H_
#define _PXF_GAME_PHYSICSWORLD_H_

#include <Pxf/Math/Vector.h>

namespace Pxf 
{	
	namespace Game
	{
		class PhysicsObject;
		class PhysicsWorld
		{
		public:
			// TODO: what to do
			PhysicsWorld(Math::Vec3f _Gravity)
				: m_Gravity(_Gravity.x,_Gravity.y,_Gravity.z) { }
			virtual void Simulate() = 0;
			virtual void ClearForces() = 0;
		protected:
			Math::Vec3f m_Gravity;
		};
	} // Game
} // Pxf
#endif