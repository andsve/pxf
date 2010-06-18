#ifndef _PXF_GAME_SPRITE_H_
#define _PXF_GAME_SPRITE_H_

#define SPRITE_NO_SORT -1

#include <Pxf/Math/Vector.h>
#include <vector.h>
#include <stdarg.h>

namespace Pxf
{	
	namespace Graphics {
		class Texture;
		class Device;
	}

	namespace Game  
	{		
		struct sprite_sequence {
			~sprite_sequence();
			int* sequence;
			int size;
			bool valid;
		};

		enum SpriteState {
			Running,
			Paused,
			Stopped
		};

		/*	abstract sprite class
			TODO: extend / implement physics?
			TODO: add timing somehow, update function with a dt argument perhaps?
			TODO: switchable sequences - might be useful to store a set of sequences for each sprite,
				  such as different sprite intervals for death animations, jump animations etc.
		 */
		class Sprite
		{
		public:
			// TODO: remove customsequence from constructor
			Sprite(Graphics::Device* _pDevice, const char* _ID, Graphics::Texture* _Texture, int _CellWidth, int _CellHeight,int _Frequency, int _ZIndex = SPRITE_NO_SORT);
			virtual ~Sprite();
			void Draw();
			void Update();

			// animation controls
			void Reset();
			void Pause();
			void Stop();
			void Start();
			void NextFrame();
			
			void AddSequence(int _SequenceLength, ...);
			void SetScale(float _Value) { m_DrawScale = _Value; }
			
			bool IsReady() { return m_Ready; }
			int	GetZIndex() { return m_ZIndex; }
			int	GetCurrentFrame() { return m_CurrentFrame; }
			const char* GetID() { return m_ID; }
		protected:
			static unsigned		m_SpriteCounter;	// generate new ID's
		private:
			void _CalculateUV();
			
			Graphics::Device*		m_Device;
			Graphics::Texture*		m_Texture;
			int						m_CellSize[2];		// force power-of-two cell size? 
			float					m_UVStep[2];		// step sizes
			float					m_UVCoords[4];
			float					(*m_UVValues)[4];

			SpriteState			m_SpriteState;
			const char* 		m_ID;			// name identifier (debugging/logging..)

			int 				m_Frequency;	// animation frequency
			int					m_CurrentFrame;	// frame counter
			int					m_ZIndex;		// depth sort
			int					m_MaxFrames; 	// calculate
			
			float				m_TimeStep;		// 60 / frequency
			float				m_SwitchTime;	// += dt, call nextframe when >= timestep
			float				m_DrawScale;	// draw scale

			bool				m_Ready;
			bool				m_UseCustomSequence;
			
			std::vector<sprite_sequence> m_SequenceList;
		};
	}
}

#endif //_PXF_GAME_SPRITE_H_