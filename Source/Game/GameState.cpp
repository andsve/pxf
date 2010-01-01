#include <Pxf/Game/GameState.h>
#include <Pxf/Base/Debug.h>
#include <stdio.h>

#define LOCAL_MSG "GameState"

using namespace Pxf;
using namespace Game;

GameState* GameState::m_Instance = 0;

GameState* GameState::Instance() 
{
	if(!m_Instance)
		Pxf::Message(LOCAL_MSG,"Class has not been created");

	return m_Instance;
}

