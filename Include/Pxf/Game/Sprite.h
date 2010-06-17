#ifndef _PXF_GAME_SPRITE_H_
#define _PXF_GAME_SPRITE_H_

namespace Pxf
{	
	namespace Graphics {
		class Texture;
		class Device;
	}

	namespace Math {
		class Vector2D;
	}

	namespace Game  
	{
		// abstract sprite class
		// TODO: extend / implement physics?

		enum SpriteState {
			Running,
			Paused,
			Stopped
		};

		class Sprite
		{
		public:
			Sprite(Graphics::Device* _pDevice, const char* _ID, const char* _Filepath, int _CellWidth, int _CellHeight, int* _CustomSequence = NULL);
			Sprite(Graphics::Device* _pDevice, const char* _ID, Graphics::Texture* _Texture, int _CellWidth, int _CellHeight, int* _CustomSequence = NULL);
			virtual ~Sprite();
			void Draw();

			// animation controls
			void Reset();
			void Pause();
			void Stop();
			void Start();
			void NextFrame();

			bool IsReady() { return m_Ready; }
			int	GetZIndex() { return m_ZIndex; }
			int	GetCurrentFrame() { return m_CurrentFrame; }
			const char* GetID() { return m_ID; }
		private:
			Graphics::Device* 	m_Device;
			Graphics::Texture* 	m_Texture;
			Math::Vector2D 		m_CellSize;		// force power-of-two cell size?

			SpriteState			m_SpriteState;
			const char* 		m_ID;			// name identifier (debugging/logging..)

			int 				m_Frequency;	// animation frequency
			int					m_CurrentFrame;	// frame counter
			int					m_ZIndex;		// depth sort
			int					m_MaxFrames; 	// calculate

			bool				m_Ready;
			int*				m_CustomSequence;
			bool				m_UseCustomSequence;
		};
	}
}

#endif //_PXF_GAME_SPRITE_H_