#include <Pxf/Extra/LuaGUI/GUIHandler.h>
#include <Pxf/Extra/LuaGUI/GUIScript.h>
#include <Pxf/Graphics/Device.h>


using namespace Pxf;
using namespace Pxf::Graphics;
using namespace Pxf::Extra::LuaGUI;

//////////////////////////////////////////////////////////////////////////


GUIHandler::GUIHandler(const char* _theme_filepath, Device* _device)
{
	m_Device = _device;
	m_Theme = m_Device->CreateTexture(_theme_filepath);
}

GUIHandler::~GUIHandler()
{
	GUIScript* script;
	for ( std::list<GUIScript*>::iterator it = m_Scripts.begin() ; it != m_Scripts.end(); it++ )
	{
		script = *it;
		delete script;
	}

	m_Scripts.clear();
}

void GUIHandler::AddScript(const char* _filepath, Math::Vec4i* _viewarea)
{
	GUIScript* script = new GUIScript(_filepath, _viewarea, m_Device);
	m_Scripts.push_front(script);
}

void GUIHandler::Update(float _delta)
{
	// TODO: Take care of mouse inputs etc and forward to scripts

	// Update each GUIScript
	for ( std::list<GUIScript*>::iterator it = m_Scripts.begin() ; it != m_Scripts.end(); it++ )
	{
		((GUIScript*)*it)->Update(_delta);
	}
}

void GUIHandler::Draw()
{
	// Draw each GUIScript
	for ( std::list<GUIScript*>::iterator it = m_Scripts.begin() ; it != m_Scripts.end(); it++ )
	{
		((GUIScript*)*it)->Draw();
	}
}


