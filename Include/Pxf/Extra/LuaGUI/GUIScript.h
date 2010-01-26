#ifndef _PXF_EXTRA_GUISCRIPT_H_
#define _PXF_EXTRA_GUISCRIPT_H_

#include <Pxf/Extra/LuaGUI/LuaGUI.h>
#include <Pxf/Util/String.h>

namespace Pxf
{

	namespace Extra
	{

		namespace LuaGUI
		{
			class GUIScript
			{
			public:
				GUIScript(const char* _filepath);
				~GUIScript();

				void Update(float _delta);
				void Draw();
			
			private:
				Pxf::Util::String m_Filepath;
			};
			
		} // LuaGUI

	} // Extra

} // Pxf

#endif // _PXF_EXTRA_GUISCRIPT_H_