#ifndef _PXF_SCENE_SCENEMANAGER_H
#define _PXF_SCENE_SCENEMANAGER_H

#include <vector>
#include <Pxf/Scene/Scene.h>

namespace Pxf {
namespace Scene
{	
	//! Simple scene manager that stores/loads scenes
	//! No fancy scene graph stuff here.. TODO: ?
	class SceneManager
	{
	private:
		//! Scene structures
		std::vector<Pxf::Scene::Scene*> 	m_Scenes;

		//! Statistics
		unsigned long m_NumVerts;
		unsigned long m_NumFaces;

	public:
		SceneManager();
		~SceneManager();

		//! Adds new scene to scene manager
		//! return = false if failed
		Pxf::Scene::Scene* AddScene(Pxf::Scene::Scene* _Scene);

		//! Create new scene
		//! return = 0 if failed
		Pxf::Scene::Scene* NewScene(const char* _Name); 

		//! Statistics getters
		unsigned long GetNumVerts() { return m_NumVerts; }
		unsigned long GetNumFaces() { return m_NumFaces; }

		//! Do a scene lookup 
		//! return = 0 if failed
		//! else return the scene
		Pxf::Scene::Scene* GetScene(const char* _Name);

		void PrintStatistics();
	};
} // Scene
} // Pxf

#endif // _PXF_SCENE_SCENEMANAGER_H