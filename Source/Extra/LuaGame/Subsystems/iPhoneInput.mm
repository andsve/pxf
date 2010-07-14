#if defined(TARGET_OS_IPHONEDEV) || defined(TARGET_OS_IPHONEFAKEDEV)

#include <Pxf/Extra/LuaGame/Subsystems/iPhoneInput.h>
#include <Pxf/Graphics/OpenGL/DeviceGLES11.h>

using namespace Pxf;
using namespace Graphics;
using namespace Extra::LuaGame;

void IPhoneInputSubsystem::RegisterClass(lua_State* _L)
{
    // luagame.iphone
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_newtable(_L);
    lua_setfield(_L, -2, "iphone");
    
    lua_getfield(_L, -1, "iphone");
    
    // luagame.iphone.gettext
    lua_pushcfunction(_L, GetText);
    lua_setfield(_L, -2, "gettext");
}

int IPhoneInputSubsystem::GetText(lua_State* _L)
{
    // luagame.graphics.drawquad(x, y, w, h)
    int argc = lua_gettop(_L);
    if (argc == 3 &&
        lua_isstring(_L, 1) && lua_isstring(_L, 2) && lua_isstring(_L, 3))
    {
        // Send data to Game instance VBO
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        
        ((DeviceGLES11*)g->m_Device)->RequestTextInput(lua_tostring(_L, 1), lua_tostring(_L, 2), lua_tostring(_L, 3));
    
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to gettext function!");
        lua_error(_L);
    }
    
    return 0;
}

void IPhoneInputSubsystem::Update(Game* g, lua_State* _L)
{
    // TODO: Move this to getter-functions in lua, and do the checks in a core-update lua method?
    // Check if we have got a input response
    if (((DeviceGLES11*)g->m_Device)->InputHasRespondedText())
    {
        char t_text[2048];
        ((DeviceGLES11*)g->m_Device)->InputGetResponseText(t_text);
        g->PrefixStack("TextInput");
        lua_pushstring(g->L, t_text);
        g->RunScriptMethod(1);
        ((DeviceGLES11*)g->m_Device)->InputClearResponse();
        
    }
    
    // Check taps and drag events
    InputTapData t_tap;
    if (((DeviceGLES11*)g->m_Device)->InputTap(&t_tap))
    {
        // Call tap callback
        g->PrefixStack("EventTap");
        lua_pushnumber(g->L, t_tap.pos[0]);
        lua_pushnumber(g->L, t_tap.pos[1]);
        g->RunScriptMethod(2);
    }
    
    if (((DeviceGLES11*)g->m_Device)->InputDoubleTap(&t_tap))
    {
        // Call double tap callback
        g->PrefixStack("EventDoubleTap");
        lua_pushnumber(g->L, t_tap.pos[0]);
        lua_pushnumber(g->L, t_tap.pos[1]);
        g->RunScriptMethod(2);
    }
    
    InputDragData t_drag;
    if (((DeviceGLES11*)g->m_Device)->InputDrag(&t_drag))
    {
        // Call drag callback
        g->PrefixStack("EventDrag");
        lua_pushnumber(g->L, t_drag.start[0]);
        lua_pushnumber(g->L, t_drag.start[1]);
        lua_pushnumber(g->L, t_drag.end[0]);
        lua_pushnumber(g->L, t_drag.end[1]);
        g->RunScriptMethod(4);
    }
}

#endif
