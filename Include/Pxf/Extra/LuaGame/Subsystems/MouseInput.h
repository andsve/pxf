#ifndef __PXF_EXTRA_LUAGAME_SUBSYSTEMS_MOUSEINPUT_H__
#define __PXF_EXTRA_LUAGAME_SUBSYSTEMS_MOUSEINPUT_H__

#include <Pxf/Extra/LuaGame/Subsystem.h>
#include <Pxf/Input/Input.h>

namespace Pxf
{
    namespace Extra
    {
        namespace LuaGame
        {
            namespace MouseInputSubsystem
            {
                void RegisterClass(lua_State* _L);
                void Update(Game* g, lua_State* _L);
                
                int GetMousePos(lua_State* _L);
                
            } /* MouseInputSubsystem */
        } /* LuaGame */
    } /* Extra */
} /* Pxf */



#endif // __PXF_EXTRA_LUAGAME_SUBSYSTEMS_MOUSEINPUT_H__
