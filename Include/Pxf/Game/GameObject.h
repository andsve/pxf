#ifndef _PXF_GAME_GAMEOBJECT_H_
#define _PXF_GAME_GAMEOBJECT_H_

#include <Pxf/Math/Vector.h>
#include <Pxf/Game/PhysicsObject.h>

namespace Pxf 
{
	namespace Game
	{

		// TODO: Extend physicsobject so that a gameobject is independant of physics engine
		class GameObject : public PhysicsObject
		{
		public:
			GameObject();
			GameObject(Math::Vec3f _Position, const char* _ID = 0);
			GameObject(float _X, float _Y, float _Z, const char* _ID = 0);
			
		protected:
			static unsigned				m_ObjectCounter;
			const char*					m_ID;
			
		private:
			void _GenerateNewName();
		};
	} // Game
} // Pxf
#endif