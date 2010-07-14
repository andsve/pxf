#ifndef _PXF_GAME_BOX2D_PARAMETERDEFS_H_
#define _PXF_GAME_BOX2D_PARAMETERDEFS_H_

#include <Pxf/Game/PhysicsBodyDefs.h>
#include <Box2D/Collision/Shapes/b2Shape.h>

namespace Pxf {
	namespace Game {
		// these are just a few parameters to get started.. could scrap this as well, its just to create bodies rapidly..
		struct body_parameters 
		{
			Math::Vec2f		position;
			Math::Vec2f		half_size;
			float			friction;
			float			density;
			float			radius;
			b2Shape::Type	shape_type;
			PhysicsBodyType	po_type;
		};
	} // Game
} // Pxf

#endif