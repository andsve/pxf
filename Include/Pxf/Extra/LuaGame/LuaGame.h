#ifndef __PXF_EXTRA_LUAGAME_LUAGAME_H__
#define __PXF_EXTRA_LUAGAME_LUAGAME_H__

#include <Pxf/Util/String.h>
#include <Pxf/Graphics/Device.h>
#include <Pxf/Graphics/Texture.h>
#include <Pxf/Graphics/QuadBatch.h>

#if !defined(TARGET_OS_IPHONEDEV)
#include <Pxf/Input/Input.h>
#endif

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
                void CleanUp();
            
                bool Reload();
                bool Load();
                bool Update(float dt);
                bool Render();
                
                // Lua state
                lua_State *L;
                
                // Pxf device
                Graphics::Device* m_Device;
                
                // Graphic data
                QBTConnection* m_QBT[8];
                int m_CurrentQBT, m_QBTCount;
                Graphics::QuadBatch* GetCurrentQB();
                Graphics::Texture* GetCurrentTexture();
                
                // Graphics manipulation (luagame.graphics ...)
                void BindTexture(Graphics::Texture* _texture);
                
                void AddQuad(float x, float y, float w, float h);
                void AddQuad(float x, float y, float w, float h, float s0, float t0, float s1, float t1);
                void AddQuad(float x, float y, float w, float h, float rotation);
                void AddQuad(float x, float y, float w, float h, float rotation, float s0, float t0, float s1, float t1);
                
                void SetColor(float r, float g, float b);
                void SetAlpha(float a);
                
                // Depth/Z control
                float m_CurrentDepth;
                float m_DepthStep;
                float m_DepthFar, m_DepthNear;
                
                // World orientation
                void Translate(float x, float y);
                void Rotate(float angle);
                void LoadIdentity();
				
				// Public preload stuff
				Graphics::Texture* AddPreload(Util::String _filepath);
				
				// Safely call a lua function from outside the "game"
                void PrefixStack(const char* _method);
                void RunScriptMethod(int _param_num = 0);
                
#if !defined(TARGET_OS_IPHONEDEV)
                // Mouse input handling for non-iPhone games
                void SetInputDevice(Input::Input* _InputDevice);
                Input::Input* m_InputDevice;                
#endif

#if defined(TARGET_OS_IPHONEFAKEDEV)
                void SetHitArea(float x, float y, float w, float h);
                float m_HitArea[4];
#endif

            private:
                bool m_Running; // Script state
                bool m_HasInitialized;
                bool m_GracefulFail;
            
                Util::String m_GameFilename;
                Util::String m_GameIdent;
                Util::String m_GameVersion; // Simple mechanism for version handling
                
            
                // Error/panic handling
                int m_CrashRetries;
                
                // Private LuaGame quadbatch and texture
                // (internal usage, such as preload-bar etc)
                Graphics::Texture* m_CoreTexture;
                Graphics::QuadBatch* m_CoreQB;
                
            
                // Private methods
                void _register_lua_libs_callbacks();
                void _register_own_callbacks();
                bool HandleLuaErrors(int _error);
                bool CallGameMethod(int _param_num = 0);
                bool CallGameMethod(const char* _method);
                static void* GetInstance(lua_State *_L);
                
                // Preload stuff
                Graphics::Texture* m_PreLoadQueue_Textures[8]; // TODO: Change to some dynamic type of list?
                int m_PreLoadQueue_Textures_Counter;
                int m_PreLoadQueue_Total;
                int PreLoad(); // Called within Render() in the starting phase of the game
                
            
                // Callback methods
                static int Print(lua_State *_L);
                static int RunString(lua_State *_L);
            };
        } /* LuaGame */
    } /* Extra */
} /* Pxf */

#endif // __PXF_EXTRA_LUAGAME_LUAGAME_H__