#ifndef __PXF_EXTRA_LUAGAME_SUBSYSTEMS_IPHONEINPUT_H__
#define __PXF_EXTRA_LUAGAME_SUBSYSTEMS_IPHONEINPUT_H__

#include <Pxf/Extra/LuaGame/Subsystem.h>

namespace Pxf
{
    namespace Extra
    {
        namespace LuaGame
        {
            namespace IPhoneInputSubsystem
            {
                void RegisterClass(lua_State* _L);
                void Update(Game* g, lua_State* _L);
                
                int GetText(lua_State* _L);
                
                
            } /* IPhoneInputSubsystem */
        } /* LuaGame */
    } /* Extra */
} /* Pxf */



#endif // __PXF_EXTRA_LUAGAME_SUBSYSTEMS_IPHONEINPUT_H__
