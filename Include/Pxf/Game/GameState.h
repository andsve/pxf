#ifndef _PXF_GAME_GAMESTATE_H_
#define _PXF_GAME_GAMESTATE_H_

namespace Pxf
{	
	namespace Game  
	{
		// abstract state class
		class GameState
		{
		public:
			// Initialize state resources etc.
			virtual bool Init() = 0;
			// update state 
			virtual void Update() = 0;
			// render this game state
			virtual void Render() = 0;
			// called when exiting a state
			virtual void Halt() = 0;
			// pause this state
			virtual void Pause() = 0;
			// resume this state
			virtual void Resume() = 0;

			static GameState* Instance();
		protected:
			GameState();
		private:
			GameState(GameState const&) {};
			GameState& operator=(GameState const&) {};
			static GameState* m_Instance;
		};
	}
}

#endif //_PXF_GAME_GAMESTATE_H_