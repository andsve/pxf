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

// Lua includes
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace Pxf
{

	namespace Graphics { class Device; }

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
				void Update(float _delta);
				void Draw();

				// lua callable methods
				void testit(int _i);
			
			private:
				Graphics::Device* m_Device;
				Pxf::Util::String m_Filepath;
				int m_Viewarea[4];

				lua_State *L;

				std::list<GUIWidget*> m_Widgets;
			};
			
		} // LuaGUI

	} // Extra

} // Pxf

#endif // _PXF_EXTRA_GUISCRIPT_H_