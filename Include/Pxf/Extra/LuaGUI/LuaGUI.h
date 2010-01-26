#ifndef _PXF_EXTRA_LUAGUI_H_
#define _PXF_EXTRA_LUAGUI_H_

#include <Pxf/Base/Config.h>
#include <Pxf/Base/Types.h>
#include <Pxf/Base/ErrorType.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


namespace Pxf
{

	namespace Extra
	{

		namespace LuaGUI
		{
			class GUIWidget;
			class GUIScript;
			class GUIHandler;
		}

	}

}

#endif // _PXF_EXTRA_LUAGUI_H_