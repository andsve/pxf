#ifndef __PXF_EXTRA_LUAGAME_LUAGAME_H__
#define __PXF_EXTRA_LUAGAME_LUAGAME_H__

#include <Pxf/Util/String.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/QuadBatch.h>

// Lua includes
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#define LUAGAME_TABLE "luagame"

namespace Pxf
{
    
    namespace Extra
    {
        namespace LuaGame
        {
            struct QBTConnection
            {
                Util::String m_TextureName;
                Graphics::QuadBatch* m_QuadBatch;
                Graphics::Texture* m_Texture;
                QBTConnection(Graphics::Texture* _texture, Graphics::Device* _device) {
                    m_TextureName = _texture->m_Filepath;
                    m_QuadBatch = _device->CreateQuadBatch(1024);
                    m_Texture = _texture;
                }
            };
            
            class Game
            {
            public:
                Game(Util::String _gameFilename, Graphics::Device* _device, bool _gracefulFail = false);
                ~Game();
            
                bool Load();
                bool Update(float dt);
                bool Render();
                
                // Pxf device
                Graphics::Device* m_Device;
                
                // Graphic data
                //Graphics::QuadBatch* m_QuadBatch;
                QBTConnection* m_QBT[8];
                int m_CurrentQBT, m_QBTCount;
                Graphics::QuadBatch* GetCurrentQB();
                Graphics::Texture* GetCurrentTexture();
                void BindTexture(Graphics::Texture* _texture);
                
                void AddQuad(float x, float y, float w, float h);
                void AddQuad(float x, float y, float w, float h, float s0, float t0, float s1, float t1);
                void AddQuad(float x, float y, float w, float h, float rotation);
                void AddQuad(float x, float y, float w, float h, float rotation, float s0, float t0, float s1, float t1);
                
                float m_CurrentDepth;
                float m_DepthStep;
                float m_DepthFar, m_DepthNear;
                
                // World orientation
                void Translate(float x, float y);
                void Rotate(float angle);
                void LoadIdentity();
				
				// Public preload stuff
				Graphics::Texture* AddPreload(Util::String _filepath);

            private:
                bool m_Running; // Script state
                bool m_GracefulFail;
            
                Util::String m_GameFilename;
                Util::String m_GameIdent;
                Util::String m_GameVersion; // Simple mechanism for version handling
                lua_State *L;
            
                Graphics::Texture* m_PanicTexture;
                Graphics::QuadBatch* m_PanicQB;
            
                // Private methods
                void _register_lua_libs_callbacks();
                void _register_own_callbacks();
                bool HandleLuaErrors(int _error);
                bool CallGameMethod(const char* _method);
                static void* GetInstance(lua_State *_L);
                
                // Preload stuff
                Graphics::Texture* m_PreLoadQueue_Textures[8]; // TODO: Change to some dynamic type of list?
                int m_PreLoadQueue_Textures_Counter;
                int m_PreLoadQueue_Total;
                int PreLoad(); // Called within Render() in the starting phase of the game
                
            
                // Callback methods
                static int Print(lua_State *_L);
            };
        } /* LuaGame */
    } /* Extra */
} /* Pxf */

#endif // __PXF_EXTRA_LUAGAME_LUAGAME_H__