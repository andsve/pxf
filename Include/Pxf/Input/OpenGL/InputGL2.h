#ifndef _PXF_INPUT_INPUTGL2_H_
#define _PXF_INPUT_INPUTGL2_H_

#include <Pxf/Input/Input.h>

namespace Pxf
{
	namespace Graphics { class Window; }

	namespace Input
	{
		static Input* Instance;

		class InputGL2 : public Input
		{
		public:
			InputGL2(Graphics::Window* _windowptr);
			~InputGL2();
			void Update();

			bool IsKeyDown(int _key);
			bool IsButtonDown(int _button);

			int GetLastKey();
			int GetLastChar();
			int GetLastButton();

			void ClearLastKey();
			void ClearLastChar();
			void ClearLastButton();

			void GetMousePos(int *x, int *y);
			void SetMousePos(int x, int y);

			MOUSE_MODE GetMouseMode();
			void SetMouseMode(MOUSE_MODE _Mode);

			void ShowCursor(bool _show);

			// State vars
			int last_char;   /* stores last char pressed */
			int last_key;    /* stores last key pressed (redundant? glfw :E) */
			int last_button; /* stores last mouse button pressed */

			MOUSE_MODE mouse_mode;
			int mouse_x, mouse_y, mouse_scroll;
			unsigned int mouse_buttons; /* store status for mouse button, bit #0 = button 1. 1 = pressed */ 
		};
	} // Input
} // Pxf

#endif // _PXF_INPUT_INPUTGL2_H_