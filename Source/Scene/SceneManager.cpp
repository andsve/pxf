#include <Pxf/Scene/SceneManager.h>

using namespace Pxf;
using namespace Scene;

SceneManager::~SceneManager()
{
	m_Scenes.clear();
}

SceneManager::SceneManager()
{

}