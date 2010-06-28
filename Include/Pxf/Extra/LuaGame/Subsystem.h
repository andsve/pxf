#ifndef __PXF_EXTRA_LUAGAME_SUBSYSTEM_H__
#define __PXF_EXTRA_LUAGAME_SUBSYSTEM_H__

#include <Pxf/Extra/LuaGame/LuaGame.h>

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
        namespace LuaGame
        {
            class Subsystem
            {
            public:
                //Subsystem ();
                //virtual ~Subsystem();
                
                //virtual void RegisterClass(lua_State* _L);
                
                //virtual int ToString(lua_State* _L);

            private:
                
            };
        } /* LuaGame */
    } /* Extra */
} /* Pxf */

#endif //__PXF_EXTRA_LUAGAME_SUBSYSTEM_H__