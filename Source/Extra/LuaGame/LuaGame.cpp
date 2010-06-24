
#include <Pxf/Extra/LuaGame/LuaGame.h>

using namespace Pxf;
using namespace Pxf::Extra;

LuaGame::LuaGame(Util::String _gameFilename)
{
    m_GameFilename = _gameFilename;
}

LuaGame::~LuaGame()
{
    // Destructor of LuaGame
    // Do nothing?
}

bool LuaGame::PreLoad()
{
    // Preload game data
    return true;
}

bool LuaGame::Update(float dt)
{
    // Update game
    
    return true;
}

bool LuaGame::Render()
{
    // Render game
    
    return true;
}


