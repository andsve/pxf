#ifndef __PXF_EXTRA_LUAGAME_SUBSYSTEMS_GRAPHICS_H__
#define __PXF_EXTRA_LUAGAME_SUBSYSTEMS_GRAPHICS_H__

#include <Pxf/Extra/LuaGame/Subsystem.h>

namespace Pxf
{
    namespace Extra
    {
        namespace LuaGame
        {
            namespace GraphicsSubsystem
            {
                void RegisterClass(lua_State* _L);
                int DrawQuad(lua_State* _L);
                int Translate(lua_State* _L);
                int Rotate(lua_State* _L);
                
                int GetScreenSize(lua_State* _L);
                
                
            } /* GraphicsSubsystem */
        } /* LuaGame */
    } /* Extra */
} /* Pxf */



#endif // __PXF_EXTRA_LUAGAME_SUBSYSTEMS_GRAPHICS_H__
