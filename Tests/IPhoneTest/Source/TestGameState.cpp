#include "../Include/TestGameState.h"
#include <Pxf/Base/Debug.h>

#define LOCAL_MSG "TestGameState"

TestGameState::TestGameState()
{
	if(!Init())
		Pxf::Message(LOCAL_MSG,"Unable to initialize");
}

bool TestGameState::Init()
{
	bool _RetVal = true;
	
	
	return _RetVal;
}

void TestGameState::Update()
{
}
void TestGameState::Render()
{
}
void TestGameState::Halt()
{
}
void TestGameState::Pause()
{
}
void TestGameState::Resume()
{
}