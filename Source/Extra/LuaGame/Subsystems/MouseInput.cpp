#if !defined(TARGET_OS_IPHONEDEV)

#include <Pxf/Extra/LuaGame/Subsystems/MouseInput.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Input/Input.h>

using namespace Pxf;
using namespace Graphics;
using namespace Extra::LuaGame;

void MouseInputSubsystem::RegisterClass(lua_State* _L)
{
    // luagame.mouse
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_newtable(_L);
    lua_setfield(_L, -2, "mouse");
    
    lua_getfield(_L, -1, "mouse");
    
    // luagame.mouse.getmousepos
    lua_pushcfunction(_L, GetMousePos);
    lua_setfield(_L, -2, "getmousepos");
    
}

// 0 = up, 1 = down
int MouseInputSubsystem::m_LastMouseState = 0;

void MouseInputSubsystem::Update(Game* g, lua_State* _L)
{
    //g->m_InputDevice->Update();
    /*Math::Vec2i mousepos_i;
	Math::Vec2f mousepos_f;
	pInput->GetMousePos(&mousepos_i.x, &mousepos_i.y);
	mousepos_f.x = mousepos_i.x;
	mousepos_f.y = mousepos_i.y;*/
	
	if (g->m_InputDevice->IsButtonDown(Input::MOUSE_LEFT))
	{
	    // Call pushed
	    g->PrefixStack("mousedown");
        g->RunScriptMethod(0);
        
        m_LastMouseState = 1;
	} else {
	    
	    if (m_LastMouseState)
	    {
	        // Mouse button was released
	        g->PrefixStack("mouseup");
            g->RunScriptMethod(0);
	    }
        m_LastMouseState = 0;
	}
}

int MouseInputSubsystem::GetMousePos(lua_State* _L)
{
    int x,y;
    
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_getfield(_L, -1, "Instance");
    Game* g = (Game*)lua_touserdata(_L, -1);
    
	g->m_InputDevice->GetMousePos(&x, &y);
	
#if defined(TARGET_OS_IPHONEFAKEDEV)
    x = x - (int)g->m_HitArea[0];
    y = y - (int)g->m_HitArea[1];
    if (x < 0)
        x = 0;
    else if (x > (int)g->m_HitArea[2])
        x = (int)g->m_HitArea[2];
    
    if (y < 0)
        y = 0;
    else if (y > (int)g->m_HitArea[3])
        y = (int)g->m_HitArea[3];
#endif
	
    lua_pushnumber(_L, x);
    lua_pushnumber(_L, y);
    return 2;
}

#endif
