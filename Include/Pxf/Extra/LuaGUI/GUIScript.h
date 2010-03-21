#ifndef _PXF_EXTRA_GUISCRIPT_H_
#define _PXF_EXTRA_GUISCRIPT_H_

// std things
#include <list>

// pxf
#include <Pxf/Util/String.h>
#include <Pxf/Math/Vector.h>

#include <Pxf/Extra/LuaGUI/LuaGUI.h>
#include <Pxf/Extra/LuaGUI/GUIHandler.h>
#include <Pxf/Extra/LuaGUI/GUIWidget.h>

#include <Pxf/Extra/SimpleFont/SimpleFont.h>

// Lua includes
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace Pxf
{

	namespace Graphics { class Device; class Texture; class QuadBatch; }

	namespace Extra
	{

		namespace LuaGUI
		{
		  
			class GUIScript
			{
			public:
				GUIScript(const char* _filepath, Math::Vec4i* _viewarea, Graphics::Device* _device);
				~GUIScript();

				void Load();
				void Unload();
				void Reload();
				void Update(Math::Vec2f* _mouse, bool _mouse_down, float _delta);
				void Draw();

				// lua callable methods
				GUIWidget* AddWidget(const char* _name, Math::Vec4i _hitbox);
				void AddQuad(GUIWidget* _widget, Math::Vec4i* _quad, Math::Vec4i* _texpixels);
				void AddText(GUIWidget* _widget, Util::String _text, Math::Vec3f _pos);
				void AddTextCentered(GUIWidget* _widget, Util::String _text, Math::Vec3f _pos);
				void SendMessage(GUIWidget* _widget, int _messageid, void* _data);
				
				// Messages from script to engine
        bool MessagePump(ScriptMessage* _pmessage);
			
			private:
				Graphics::Device* m_Device;
				Graphics::QuadBatch* m_QuadBatch;
				Graphics::Texture* m_Texture;
				SimpleFont* m_Font;
				Util::String m_Filepath;
				int m_Viewarea[4];

				lua_State *L; // Lua state!

				std::list<GUIWidget*> m_Widgets;

				bool m_Running;
				bool m_ShouldReload;
				
				// Messages from script to engine
				std::list<ScriptMessage*> m_Messages;

				// Lua specific functions
				void CallLuaFunc(const char* _funcname);
				bool HandleLuaErrors(int _error);
			};
			
		} // LuaGUI

	} // Extra

} // Pxf

#endif // _PXF_EXTRA_GUISCRIPT_H_