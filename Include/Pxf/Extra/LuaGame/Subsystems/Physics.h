#ifndef __PXF_EXTRA_LUAGAME_SUBSYSTEMS_PHYSICS_H__
#define __PXF_EXTRA_LUAGAME_SUBSYSTEMS_PHYSICS_H__

#include <Pxf/Extra/LuaGame/Subsystem.h>

namespace Pxf
{
    namespace Extra
    {
        namespace LuaGame
        {
            namespace PhysicsSubsystem
            {
                void RegisterClass(lua_State* _L);
                int NewWorld(lua_State* _L);
            } /* PhysicsSubsystem */
        } /* LuaGame */
    } /* Extra */
} /* Pxf */



#endif // __PXF_EXTRA_LUAGAME_SUBSYSTEMS_GRAPHICS_H__
