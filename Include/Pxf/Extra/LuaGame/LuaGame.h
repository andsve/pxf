#ifndef __PXF_EXTRA_LUAGAME_LUAGAME_H__
#define __PXF_EXTRA_LUAGAME_LUAGAME_H__

#include <Pxf/Util/String.h>

namespace Pxf
{
    
    namespace Extra
    {
        
        class LuaGame
        {
        public:
            LuaGame (Util::String _gameFilename);
            ~LuaGame ();
            
            bool PreLoad();
            bool Update(float dt);
            bool Render();

        private:
            Util::String m_GameFilename;
            Util::String m_GameIdent;
            int m_GameVersion[3]; // Simple mechanism for version handling 1.2.0 -> [0].[1].[2]
        };
        
    } /* Extra */
} /* Pxf */

#endif // __PXF_EXTRA_LUAGAME_LUAGAME_H__