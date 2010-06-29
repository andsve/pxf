#ifndef _PXF_GAME_PHYSICSOBJECT_H_
#define _PXF_GAME_PHYSICSOBJECT_H_

#include <Pxf/Math/Vector.h>

namespace Pxf 
{	
	namespace Game
	{
		class PhysicsObject 
			{
			public:				
				virtual Math::Vec3f GetPosition() = 0;
				virtual void SetPosition(Math::Vec3f) = 0;
				
			private:
	
			};
	} // Game
} // Pxf
#endif