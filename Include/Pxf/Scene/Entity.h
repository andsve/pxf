#ifndef _PXF_SCENE_ENTITY_H
#define _PXF_SCENE_ENTITY_H

#include <Pxf/Scene/EntityDefs.h>

namespace Pxf
{		
	namespace Scene
	{
		//! Abstract class for items in the engine
		class Entity
		{
		private:
			//! Name
			const char* m_Name;

			//! Type
			EntityType m_Type;
		public:
			Entity(const char* _Name,EntityType _Type) 
				: m_Name(_Name)
				, m_Type(_Type)
			{

			}

			const char* GetName() 	{ return m_Name; }
			EEntType GetType() 		{ return m_Type; }
		};
	} // Scene
} // Pxf

#endif // _PXF_SCENE_ENTITY_H