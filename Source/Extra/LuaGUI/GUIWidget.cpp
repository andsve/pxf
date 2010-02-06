#include <Pxf/Graphics/Device.h>
//#include <Pxf/Graphics/QuadBatch.h>

#include <Pxf/Extra/LuaGUI/GUIWidget.h>

using namespace Pxf;
using namespace Pxf::Math;
using namespace Pxf::Graphics;
using namespace Pxf::Extra::LuaGUI;

GUIWidget::GUIWidget(const char* _name, Math::Vec4i* _hitbox, Graphics::Device* _device)
{
	m_Name = _name;
//	m_Device = _device;
//	m_QuadBatch = m_Device->CreateQuadBatch(PXF_EXTRA_LUAGUI_MAXQUAD_PER_WIDGET);

	m_Draging = false;
	m_MouseDownLast = false;
	m_MouseOver = false;
	m_MousePushed = false;
	m_MouseClicked = false;
	m_MousePushedLast = false;

	m_Position.x = _hitbox->x;
	m_Position.y = _hitbox->y;
	m_Size.x = _hitbox->z;
	m_Size.y = _hitbox->w;
}

GUIWidget::~GUIWidget()
{
	//if (m_QuadBatch)
	//	delete m_QuadBatch;
}

void GUIWidget::SetPosition(Math::Vec2f _pos)
{
	m_Position = Math::Vec3f(_pos.x, _pos.y, 0.0f);
}

Math::Vec3f* GUIWidget::GetPosition()
{
	return &m_Position;
}

void GUIWidget::AddState(const char* _stateid)
{
	m_States.push_front(Util::String(_stateid));
}

void GUIWidget::SetState(const char* _stateid)
{
	m_ActiveState = _stateid;
}

const char* GUIWidget::GetState()
{
	return m_ActiveState.c_str();
}

void GUIWidget::Update(Math::Vec2f* _mouse, bool _mouse_down)
{

	m_MouseHit = Math::Vec2f(_mouse->x - m_Position.x, _mouse->y - m_Position.y);

	if (m_Draging && m_MouseDownLast && _mouse_down)
	{
		m_Draging = true;
	} else {
		m_Draging = false;
	}

	m_MouseDownLast = _mouse_down;

	m_MouseOver = false;
	m_MousePushed = false;
	m_MouseClicked = false;

	if (_mouse->x < m_Position.x)
	{
		m_MousePushedLast = false;
		return;
	}

	if (_mouse->x > m_Position.x + m_Size.x)
	{
		m_MousePushedLast = false;
		return;
	}

	if (_mouse->y < m_Position.y)
	{
		m_MousePushedLast = false;
		return;
	}

	if (_mouse->y > m_Position.y + m_Size.y)
	{
		m_MousePushedLast = false;
		return;
	}

	// Start draging
	if (_mouse_down)
		m_Draging = true;

	m_MouseOver = true;
	m_MousePushed = _mouse_down;

	if (!m_MousePushed && m_MousePushedLast)
	{
		m_MouseClicked = true;
	}

	m_MousePushedLast = m_MousePushed;
}

Math::Vec2f GUIWidget::GetMouseHit()
{
	return m_MouseHit;
}

bool GUIWidget::IsMouseOver()
{
	return m_MouseOver;
}

bool GUIWidget::IsDown()
{
	return m_MousePushed;
}

bool GUIWidget::IsClicked()
{
	return m_MouseClicked;
}

bool GUIWidget::IsDraging()
{
	return m_Draging;
}