#ifndef _PXF_EXTRA_GUIWIDGET_H_
#define _PXF_EXTRA_GUIWIDGET_H_

#include <Pxf/Math/Vector.h>
#include <Pxf/Extra/LuaGUI/LuaGUI.h>


namespace Pxf
{
	namespace Graphics { class Device; class QuadBatch; }

	namespace Extra
	{

		namespace LuaGUI
		{
			class GUIWidget
			{
			public:
				GUIWidget(Math::Vec4i* _hitbox, Graphics::Device* _device);
				~GUIWidget();

				void Reset();
				void AddQuad(Math::Vec4i* _quad, Math::Vec4i* _texcoord);

			private:
				Graphics::QuadBatch* m_QuadBatch;
				Graphics::Device* m_Device;
			};
			
		} // LuaGUI

	} // Extra

} // Pxf

#endif // _PXF_EXTRA_GUIWIDGET_H_