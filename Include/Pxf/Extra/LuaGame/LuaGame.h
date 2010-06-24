#ifndef __PXF_EXTRA_LUAGAME_LUAGAME_H__
#define __PXF_EXTRA_LUAGAME_LUAGAME_H__

#include <Pxf/Util/String.h>
#include <Pxf/Graphics/Device.h>

// Lua includes
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace Pxf
{
    
    namespace Extra
    {
        class LuaGame
        {
        public:
            LuaGame(Util::String _gameFilename, Graphics::Device* _device);
            ~LuaGame();
            
            bool Load();
            int  PreLoad();
            bool Update(float dt);
            bool Render();

        private:
            bool m_Running; // Script state
            
            Util::String m_GameFilename;
            Util::String m_GameIdent;
            int m_GameVersion[3]; // Simple mechanism for version handling 1.2.3 -> [0].[1].[2]
            lua_State *L;
            
            // Pxf device
            Graphics::Device* m_Device;
            
            // Private methods
            void _register_lua_libs_callbacks();
        };
        
    } /* Extra */
} /* Pxf */

#endif // __PXF_EXTRA_LUAGAME_LUAGAME_H__