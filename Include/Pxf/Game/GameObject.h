#ifndef _PXF_GAME_GAMEOBJECT_H_
#define _PXF_GAME_GAMEOBJECT_H_

#include <Pxf/Math/Vector.h>

namespace Pxf 
{
	namespace Game
	{
		// TODO: Extend physicsobject so that a gameobject is independant of physics engine
		class GameObject 
		{
		public:
			GameObject();
			GameObject(Pxf::Math::Vector3D<float> _Position, const char* _ID = 0);
			GameObject(float _X, float _Y, float _Z, const char* _ID = 0);
			
			Math::Vector3D<float>* GetPosition() { return m_Position; }
			void SetPosition(Math::Vector3D<float> _Position);
		protected:
			static unsigned				m_ObjectCounter;
			const char*					m_ID;
			
			// NOTE: should we diffrentiate between 2D and 3D objects or treat them the same at this level
			Math::Vector3D<float>* m_Position;
			
		private:
			void _GenerateNewName();
		};
	} // Game
} // Pxf
#endif