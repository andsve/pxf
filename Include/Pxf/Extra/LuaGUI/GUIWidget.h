#ifndef _PXF_EXTRA_GUIWIDGET_H_
#define _PXF_EXTRA_GUIWIDGET_H_

#include <list>

#include <Pxf/Util/String.h>
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
				GUIWidget(const char* _name, Math::Vec4i* _hitbox, Graphics::Device* _device);
				~GUIWidget();

				void SetPosition(Math::Vec2f _pos);
				void Reset();
				void AddState(const char* _stateid);
				void AddQuad(Math::Vec4i* _quad, Math::Vec4i* _texcoord);
				void Draw();
				void Update(Math::Vec2f* _mouse, bool _mouse_down);

				void SetState(const char* _stateid);
				const char* GetState();

				bool IsMouseOver();
				bool IsDown();
				bool IsClicked();

			private:
				Graphics::QuadBatch* m_QuadBatch;
				Graphics::Device* m_Device;
				Util::String m_Name;
				Math::Vec4i m_HitBox;

				Math::Vec3f m_Position;

				std::list<Util::String> m_States;
				Util::String m_ActiveState;

				// state info
				bool m_MouseOver;
				bool m_MousePushed;
				bool m_MouseClicked;
				bool m_MousePushedLast;
			};
			
		} // LuaGUI

	} // Extra

} // Pxf

#endif // _PXF_EXTRA_GUIWIDGET_H_