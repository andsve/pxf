#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/QuadBatch.h>

#include <Pxf/Extra/LuaGUI/GUIWidget.h>

using namespace Pxf;
using namespace Pxf::Math;
using namespace Pxf::Graphics;
using namespace Pxf::Extra::LuaGUI;

GUIWidget::GUIWidget(Pxf::Math::Vec4i *_hitbox, Pxf::Graphics::Device *_device)
{
	m_Device = _device;
	m_QuadBatch = m_Device->CreateQuadBatch(PXF_EXTRA_LUAGUI_MAXQUAD_PER_WIDGET);
}

GUIWidget::~GUIWidget()
{
	if (m_QuadBatch)
		delete m_QuadBatch;
}

void GUIWidget::Reset()
{
	m_QuadBatch->Reset();
}

void GUIWidget::AddQuad(Math::Vec4i* _quad, Math::Vec4i* _texcoord)
{
	m_QuadBatch->SetTextureSubset(_texcoord->x, _texcoord->y, _texcoord->z, _texcoord->w);
	m_QuadBatch->AddTopLeft(_quad->x, _quad->y, _quad->z, _quad->w);
}
