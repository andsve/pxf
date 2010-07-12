#ifndef __PXF_EXTRA_LUAGAME_SUBSYSTEMS_RESOURCES_H__
#define __PXF_EXTRA_LUAGAME_SUBSYSTEMS_RESOURCES_H__

#include <Pxf/Extra/LuaGame/Subsystem.h>

namespace Pxf
{
    namespace Extra
    {
        namespace LuaGame
        {
            namespace ResourcesSubsystem
            {
                void RegisterClass(lua_State* _L);
                int LoadTexture(lua_State* _L);
                int BindTexture(lua_State* _L);
                
                
            } /* ResourcesSubsystem */
        } /* LuaGame */
    } /* Extra */
} /* Pxf */



#endif // __PXF_EXTRA_LUAGAME_SUBSYSTEMS_RESOURCES_H__
