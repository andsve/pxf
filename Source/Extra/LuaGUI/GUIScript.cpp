#include <Pxf/Extra/LuaGUI/GUIScript.h>

using namespace Pxf::Extra::LuaGUI;

GUIScript::GUIScript(const char* _filepath)
{
	m_Filepath = _filepath;
}

GUIScript::~GUIScript()
{
	// clean up
}

void GUIScript::Update(float _delta)
{
	printf("updating: %s\n", m_Filepath.c_str());
}

void GUIScript::Draw()
{

}