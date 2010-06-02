#ifndef _PXF_SCENE_SCENE_H
#define _PXF_SCENE_SCENE_H

#include <vector>

namespace Pxf {
namespace Scene
{		
	class Camera;
	class MaterialManager;

	class Scene
	{
	private:
		//! Scene Name
		const char*				m_Name;

		//! Current camera
		Camera* 				m_CurrentCamera;

		//! Scene statistics:
		unsigned long int		m_NumVerts;
		unsigned long int		m_NumFaces;
		
		//! BGColor
		float 					m_BGColor[3];

	public:
		Scene(const char* _Name);
		~Scene();

		//! Render scene
		void Render();

		//! Get name
		const char* GetName() { return m_Name; }

		//! Scene Statistics
		void PrintStatistics();

		//! Update scene?
		void Update();
	};
} // Scene
}

#endif // _PXF_SCENE_SCENE_H