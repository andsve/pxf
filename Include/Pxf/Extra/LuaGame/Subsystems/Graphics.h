#ifndef __PXF_EXTRA_LUAGAME_SUBSYSTEMS_GRAPHICS_H__
#define __PXF_EXTRA_LUAGAME_SUBSYSTEMS_GRAPHICS_H__

#include <Pxf/Extra/LuaGame/Subsystem.h>
#include <Pxf/Game/Sprite.h>

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
                int SetColor(lua_State* _L);
                int SetAlpha(lua_State* _L);
                int Translate(lua_State* _L);
                int Rotate(lua_State* _L);
                int LoadIdentity(lua_State* _L);
                int UnbindTexture(lua_State* _L);
                int GetScreenSize(lua_State* _L);

				/*
				int NewSprite(lua_State* _L);

				class LuaSprite : public Pxf::Game::Sprite
				{	
				public:
					LuaSprite(Graphics::Device* _pDevice, Graphics::Texture* _Texture, int _CellWidth, int _CellHeight,int _Frequency)
						: Pxf::Game::Sprite(_pDevice,NULL,_Texture,_CellWidth,_CellHeight,_Frequency) { }
					static int _Draw(lua_State* _L);
					static int _Update(lua_State* _L);
					static int _AddSequence(lua_State* _L);
				}; */
                
            } /* GraphicsSubsystem */
        } /* LuaGame */
    } /* Extra */
} /* Pxf */



#endif // __PXF_EXTRA_LUAGAME_SUBSYSTEMS_GRAPHICS_H__
