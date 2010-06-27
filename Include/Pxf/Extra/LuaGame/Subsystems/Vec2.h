#ifndef __PXF_EXTRA_LUAGAME_VEC2_H__
#define __PXF_EXTRA_LUAGAME_VEC2_H__

#include <Pxf/Util/String.h>
#include <Pxf/Extra/LuaGame/Subsystem.h>

namespace Pxf
{
    namespace Extra
    {
        namespace LuaGame
        {
            class Vec2 : Subsystem
            {
            public:
                Vec2 ();
                ~Vec2 ();
                
                static void RegisterClass(lua_State* _L);
                
                // Callbacks
                static int ToString(lua_State* _L);

            private:
                float x,y;
            };
        } /* LuaGameSubsystem */
    } /* Extra */
} /* Pxf */

#endif // __PXF_EXTRA_LUAGAME_VEC2_H__