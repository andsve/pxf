#ifndef _TESTGAMESTATE_H_
#define _TESTGAMESTATE_H_

#include <Pxf/Game/GameState.h>


class TestGameState : public Pxf::Game::GameState
{
public:
	TestGameState();
	~TestGameState();
	
	bool Init();
	void Update();
	void Render();
	void Halt();
	void Pause();
	void Resume();
	
private:
	
};

#endif