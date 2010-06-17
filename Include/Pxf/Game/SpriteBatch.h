#ifndef _PXF_GAME_SPRITEBATCH_H_
#define _PXF_GAME_SPRITEBATCH_H_

namespace Pxf
{	
	namespace Game  
	{
		/* 
		Abstract spritebatch class
		---
		The purpose of this class is to manage and draw a set of sprites at the same time,
		so that the user wont need to explicitly call draw methods on each sprite. 

		Todo: Sort sprites? 

		*/
		class SpriteBatch
		{
		public:
			void Begin();
			void End();
		private:

		};
	}
}

#endif //_PXF_GAME_SPRITEBATCH_H_