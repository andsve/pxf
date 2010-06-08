#include <Pxf/Game/GameState.h>

using namespace Pxf;
using namespace Game;

GameState* GameState::m_Instance = NULL;

static GameState* GameState::Instance() 
{
	if(!m_Instance)
		m_Instance = new GameState;

	return m_Instance;
}

