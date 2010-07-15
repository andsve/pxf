
#include <Pxf/Extra/LuaGame/Subsystems/Graphics.h>

using namespace Pxf;
using namespace Extra::LuaGame;

void GraphicsSubsystem::RegisterClass(lua_State* _L)
{
    // luagame.graphics
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_newtable(_L);
    lua_setfield(_L, -2, "graphics");
    
    lua_getfield(_L, -1, "graphics");
    
    // luagame.graphics.getscreensize
    lua_pushcfunction(_L, GetScreenSize);
    lua_setfield(_L, -2, "getscreensize");
    
    // luagame.graphics.unbindtexture
    lua_pushcfunction(_L, UnbindTexture);
    lua_setfield(_L, -2, "unbindtexture");
    
    // luagame.graphics.drawquad
    lua_pushcfunction(_L, DrawQuad);
    lua_setfield(_L, -2, "drawquad");
    
    // luagame.graphics.setcolor
    lua_pushcfunction(_L, SetColor);
    lua_setfield(_L, -2, "setcolor");
    
    // luagame.graphics.loadidentity
    lua_pushcfunction(_L, LoadIdentity);
    lua_setfield(_L, -2, "loadidentity");
    
    // luagame.graphics.translate
    lua_pushcfunction(_L, Translate);
    lua_setfield(_L, -2, "translate");
    
    // luagame.graphics.rotate
    lua_pushcfunction(_L, Rotate);
    lua_setfield(_L, -2, "rotate");

	/*
	// luagame.graphics.newsprite
	lua_pushcfunction(_L,NewSprite);
	lua_setfield(_L,-2,"newsprite"); */
}

int GraphicsSubsystem::DrawQuad(lua_State* _L)
{
    // luagame.graphics.drawquad(x, y, w, h)
    int argc = lua_gettop(_L);
    if (argc == 4 &&
        lua_isnumber(_L, 1) && lua_isnumber(_L, 2) && lua_isnumber(_L, 3) && lua_isnumber(_L, 4))
    {
        // Send data to Game instance VBO
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        
        //Math::Vec4f coords = Math::Vec4f(0, 0, 1, 1);//m_Texture->CreateTextureSubset(_texpixels->x, _texpixels->y, _texpixels->z, _texpixels->w);

    	//g->m_QuadBatch->SetTextureSubset(coords.x, coords.y, coords.z, coords.w);
    	//g->GetCurrentQB()->SetTextureSubset(coords.x, coords.y, coords.z, coords.w);
    	g->AddQuad(lua_tonumber(_L, 1), lua_tonumber(_L, 2), lua_tonumber(_L, 3), lua_tonumber(_L, 4));
        
        //g->AddQuad(lua_tonumber(_L, 1), lua_tonumber(_L, 2), lua_tonumber(_L, 3), lua_tonumber(_L, 4));
    
    
    // luagame.graphics.drawquad(x, y, w, h, rotation)
    } else if (argc == 5 &&
            lua_isnumber(_L, 1) && lua_isnumber(_L, 2) && lua_isnumber(_L, 3) && lua_isnumber(_L, 4) && lua_isnumber(_L, 5))
        {
            float rotation = lua_tonumber(_L, 5);
            
            // Send data to Game instance VBO
            lua_getglobal(_L, LUAGAME_TABLE);
            lua_getfield(_L, -1, "Instance");
            Game* g = (Game*)lua_touserdata(_L, -1);

            //Math::Vec4f coords = Math::Vec4f(0, 0, 1, 1);//m_Texture->CreateTextureSubset(_texpixels->x, _texpixels->y, _texpixels->z, _texpixels->w);

            //g->m_QuadBatch->Rotate(rotation);
        	//g->GetCurrentQB()->SetTextureSubset(coords.x, coords.y, coords.z, coords.w);
        	g->AddQuad(lua_tonumber(_L, 1), lua_tonumber(_L, 2), lua_tonumber(_L, 3), lua_tonumber(_L, 4), rotation);
        	//g->m_QuadBatch->Rotate(-rotation);
    
    // luagame.graphics.drawquad(x, y, w, h, s0, t0, s1, t1)
    // s* and t* = texture coords
    } else if (argc == 8 &&
            lua_isnumber(_L, 1) && lua_isnumber(_L, 2) && lua_isnumber(_L, 3) && lua_isnumber(_L, 4) &&
            lua_isnumber(_L, 5) && lua_isnumber(_L, 6) && lua_isnumber(_L, 7) && lua_isnumber(_L, 8))
        {
            
            // Send data to Game instance VBO
            lua_getglobal(_L, LUAGAME_TABLE);
            lua_getfield(_L, -1, "Instance");
            Game* g = (Game*)lua_touserdata(_L, -1);
            
        	g->AddQuad(lua_tonumber(_L, 1), lua_tonumber(_L, 2), lua_tonumber(_L, 3), lua_tonumber(_L, 4),
        	           lua_tonumber(_L, 5), lua_tonumber(_L, 6), lua_tonumber(_L, 7), lua_tonumber(_L, 8));
    
    // luagame.graphics.drawquad(x, y, w, h, rotation, s0, t0, s1, t1)
    // s* and t* = texture coords
    } else if (argc == 9 &&
            lua_isnumber(_L, 1) && lua_isnumber(_L, 2) && lua_isnumber(_L, 3) && lua_isnumber(_L, 4) &&
            lua_isnumber(_L, 5) && lua_isnumber(_L, 6) && lua_isnumber(_L, 7) && lua_isnumber(_L, 8) && lua_isnumber(_L, 9))
        {
            
            // Send data to Game instance VBO
            lua_getglobal(_L, LUAGAME_TABLE);
            lua_getfield(_L, -1, "Instance");
            Game* g = (Game*)lua_touserdata(_L, -1);
            
        	g->AddQuad(lua_tonumber(_L, 1), lua_tonumber(_L, 2), lua_tonumber(_L, 3), lua_tonumber(_L, 4),
        	           lua_tonumber(_L, 5), lua_tonumber(_L, 6), lua_tonumber(_L, 7), lua_tonumber(_L, 8), lua_tonumber(_L, 9));
    
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to drawquad function!");
        lua_error(_L);
    }
    
    return 0;
}

int GraphicsSubsystem::SetColor(lua_State* _L)
{
    // luagame.graphics.setcolor(r, g, b, a)
    int argc = lua_gettop(_L);
    if (argc == 4 && lua_isnumber(_L, 1) && lua_isnumber(_L, 2) && lua_isnumber(_L, 3) && lua_isnumber(_L, 4))
    {
        // Send data to Game instance VBO
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        
        g->SetColor(lua_tonumber(_L, 1), lua_tonumber(_L, 2), lua_tonumber(_L, 3), lua_tonumber(_L, 4));
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to setcolor function!");
        lua_error(_L);
    }
    
    return 0;
}

int GraphicsSubsystem::Translate(lua_State* _L)
{
    // luagame.graphics.translate(dx,dy)
    int argc = lua_gettop(_L);
    if (argc == 2 && lua_isnumber(_L, 1) && lua_isnumber(_L, 2))
    {
        // Send data to Game instance VBO
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        
        g->Translate(lua_tonumber(_L, 1), lua_tonumber(_L, 2));
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to translate function!");
        lua_error(_L);
    }
    
    return 0;
}

int GraphicsSubsystem::Rotate(lua_State* _L)
{
    // luagame.graphics.rotate(angle)
    int argc = lua_gettop(_L);
    if (argc == 1 && lua_isnumber(_L, 1))
    {
        // Send data to Game instance VBO
        lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);
        
        //g->m_QuadBatch->SetRotation(lua_tonumber(_L, 1));
        g->Rotate(lua_tonumber(_L, 1));
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to rotate function!");
        lua_error(_L);
    }
    
    return 0;
}

int GraphicsSubsystem::LoadIdentity(lua_State* _L)
{
    // luagame.graphics.loadidentity()
    
    // Send data to Game instance VBO
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_getfield(_L, -1, "Instance");
    Game* g = (Game*)lua_touserdata(_L, -1);
    g->LoadIdentity();
    
    return 0;
}

int GraphicsSubsystem::UnbindTexture(lua_State* _L)
{
    // luagame.graphics.unbindtexture()
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_getfield(_L, -1, "Instance");
    Game* g = (Game*)lua_touserdata(_L, -1);
    g->BindTexture(0);
    
    return 0;
}

int GraphicsSubsystem::GetScreenSize(lua_State* _L)
{
    int w,h;
    
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_getfield(_L, -1, "Instance");
    Game* g = (Game*)lua_touserdata(_L, -1);
    
    g->m_Device->GetSize(&w, &h);
    lua_pushnumber(_L, w);
    lua_pushnumber(_L, h);
    
    return 2;
}

/*
int GraphicsSubsystem::NewSprite(lua_State* _L)
{
	GraphicsSubsystem::LuaSprite* _NewSprite;

	// luagame.graphics.newsprite(texture,cell_width,cell_height,frequency)

	int argc = lua_gettop(_L);
    if (argc == 4 && lua_isnumber(_L, 2) && lua_isnumber(_L, 3) && lua_isnumber(_L, 4))
    {
		if (!lua_istable(_L, 1))
        {
            lua_pushstring(_L, "Nil texture sent to newsprite!");
            lua_error(_L);
            return 0;
        }

		lua_getfield(_L, 1, "instance");

        if (!lua_isuserdata(_L, -1))
        {
            lua_pushstring(_L, "'instance' field is not a valid userdata!");
            lua_error(_L);
            return 0;
        }
		Graphics::Texture* tex = 0;
		tex = (Graphics::Texture*)lua_touserdata(_L, -1);
        
		int _CellW	= lua_tonumber(_L, 2);
		int _CellH	= lua_tonumber(_L, 3);
		int _Freq	= lua_tonumber(_L, 4);

		if(_CellW <= 0 || _CellW <= 0 || _Freq <= 0)
		{
			lua_pushstring(_L, "Invalid parameters passed to newsprite function!");
			lua_error(_L);

			return 0;
		}

		lua_getglobal(_L, LUAGAME_TABLE);
        lua_getfield(_L, -1, "Instance");
        Game* g = (Game*)lua_touserdata(_L, -1);

		if(g->m_Device && tex)
		{
		    // Input is alriiiiight
			_NewSprite = new GraphicsSubsystem::LuaSprite(g->m_Device,tex,_CellW,_CellH,_Freq);
			
			// Create sprite-table and object-methods
			lua_newtable(_L);
            lua_pushlightuserdata(_L, _NewSprite);
            lua_setfield(_L, -2, "instance");

        	
        	lua_pushcfunction(_L, LuaSprite::_Draw);
        	lua_setfield(_L,-2, "draw"); 
        	
            return 1;
        }
		else
		{
			lua_pushstring(_L, "Invalid device or texture passed to newsprite function!");
			lua_error(_L);

			return 0;
		}
        
    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to newsprite function!");
        lua_error(_L);

		return 0;
    }

	return 0;
}


int GraphicsSubsystem::LuaSprite::_Draw(lua_State* _L)
{
    
    // spriteobj:draw()
    int argc = lua_gettop(_L);
    if (argc == 1 && lua_istable(_L, 1))
    {
        lua_getfield(_L, 1, "instance");
        
        if (lua_isuserdata(_L, -1))
        {
            GraphicsSubsystem::LuaSprite* sprite = (GraphicsSubsystem::LuaSprite*)lua_touserdata(_L, -1);
            sprite->Draw();
            
        } else {
            lua_pushstring(_L, "'instance' field is not a valid userdata!");
            lua_error(_L);
            return 0;
        }
        
    } else {
        lua_pushstring(_L, "Invalid arguments passed to draw function (of a sprite object)!");
		lua_error(_L);
        
		return 0;
    }
    
    return 0;
}

*/

