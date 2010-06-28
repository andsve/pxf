#ifndef _PXF_GAME_BOX2D_PARAMETERDEFS_H_
#define _PXF_GAME_BOX2D_PARAMETERDEFS_H_

#include <Pxf/Game/Box2D/Box2DParameterDefs.h>

namespace Pxf {
	namespace Game {
		// TODO: rethink this way of creating bodies...
		enum Box2DShapeType
		{
			BOX2D_SHAPE_POLYGON,
			BOX2D_SHAPE_CIRCLE
		};
		
		// these are just a few parameters to get started.. could scrap this as well, its just to create bodies rapidly..
		struct body_parameters 
		{
			Math::Vec2f		position;
			float			friction;
			float			density;
			Box2DShapeType	shape_type;
			PhysicsBodyType	po_type;
		};
	} // Game
} // Pxf

#endif