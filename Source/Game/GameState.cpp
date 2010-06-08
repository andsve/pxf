#include <Pxf/Game/GameState.h>
#include <stdio.h>

using namespace Pxf;
using namespace Game;

GameState* GameState::m_Instance = 0;

GameState* GameState::Instance() 
{
	if(!m_Instance)
		printf("Class has not been created\n");

	return m_Instance;
}

