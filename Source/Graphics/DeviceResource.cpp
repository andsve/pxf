#include <Pxf/Graphics/DeviceResource.h>

using namespace Pxf;
using namespace Graphics;

unsigned DeviceResource::GLOBAL_RESOURCE_COUNTER = 0;

DeviceResource::DeviceResource(Pxf::Graphics::Device *_pDevice, Pxf::Resource::AbstractResource* _Resource)
{
	m_Resource = _Resource;
	m_pDevice = _pDevice;
	m_ID = GLOBAL_RESOURCE_COUNTER;
	GLOBAL_RESOURCE_COUNTER++;
}