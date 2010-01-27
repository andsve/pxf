#ifndef _PXF_EXTRA_LUAGUI_H_
#define _PXF_EXTRA_LUAGUI_H_

#include <Pxf/Base/Config.h>
#include <Pxf/Base/Types.h>
#include <Pxf/Base/ErrorType.h>

#define PXF_EXTRA_LUAGUI_MAXQUAD_PER_WIDGET 256
#define PXF_LUAGUI_MESSAGE_ID "LuaGUI"

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