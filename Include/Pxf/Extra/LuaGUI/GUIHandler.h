#ifndef _PXF_EXTRA_GUIHANDLER_H_
#define _PXF_EXTRA_GUIHANDLER_H_

#include <list>

#include <Pxf/Math/Vector.h>
#include <Pxf/Extra/LuaGUI/LuaGUI.h>

// Lua includes
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace Pxf
{

	namespace Graphics {
		class Texture;
		class Device;
	}

	namespace Extra
	{

		namespace LuaGUI
		{
			class GUIHandler
			{
			public:
				GUIHandler(const char* _theme_filepath, Pxf::Graphics::Device* _device);
				~GUIHandler();

				void AddScript(const char* _filepath, Math::Vec4i* _viewarea);

				void Update(float _delta);
				void Draw();
			private:
				Pxf::Graphics::Texture* m_Theme;
				Pxf::Graphics::Device* m_Device;
				std::list<GUIScript*> m_Scripts;
			};
			
		} // LuaGUI

	} // Extra

} // Pxf

#endif // _PXF_EXTRA_GUIHANDLER_H_