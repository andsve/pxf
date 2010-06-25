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
            LuaGame(Util::String _gameFilename, Graphics::Device* _device, bool _gracefulFail = false);
            ~LuaGame();
            
            bool Load();
            bool Update(float dt);
            bool Render();

        private:
            bool m_Running; // Script state
            bool m_GracefulFail;
            
            Util::String m_GameFilename;
            Util::String m_GameIdent;
            Util::String m_GameVersion; // Simple mechanism for version handling
            lua_State *L;
            
            // Pxf device
            Graphics::Device* m_Device;
            
            // Private methods
            void _register_lua_libs_callbacks();
            bool HandleLuaErrors(int _error);
            bool CallGameMethod(const char* _method);
            int  PreLoad(); // Called within Render() in the starting phase of the game
            static void* GetInstance(lua_State *_L);
            
            // Callback methods
            static int Print(lua_State *_L);
        };
        
    } /* Extra */
} /* Pxf */

#endif // __PXF_EXTRA_LUAGAME_LUAGAME_H__