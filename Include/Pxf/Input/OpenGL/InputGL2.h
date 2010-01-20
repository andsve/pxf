#ifndef _PXF_INPUT_INPUTGL2_H_
#define _PXF_INPUT_INPUTGL2_H_

#include <Pxf/Input/Input.h>

namespace Pxf
{
	namespace Graphics { class Window; }

	namespace Input
	{
		class InputGL2 : public Input
		{
		public:
			InputGL2(Graphics::Window* _windowptr);
			~InputGL2();
			void Update();
			
		};
	} // Input
} // Pxf

#endif // _PXF_INPUT_INPUTGL2_H_