#ifndef _PXF_INPUT_INPUT_H_
#define _PXF_INPUT_INPUT_H_

namespace Pxf
{
	namespace Graphics { class Window; }

	namespace Input
	{
		enum MOUSE_MODE;

		//! Abstract input class
		class Input
		{
		public:
			Input() { m_Window = NULL; }
			Input(Graphics::Window* _windowptr) { m_Window = _windowptr; }
			virtual void Update() = 0;

			virtual bool IsKeyDown(int _key) = 0;
			virtual bool IsButtonDown(int _button) = 0;

			virtual int GetLastKey() = 0;
			virtual int GetLastChar() = 0;
			virtual int GetLastButton() = 0;

			virtual void ClearLastKey() = 0;
			virtual void ClearLastChar() = 0;
			virtual void ClearLastButton() = 0;

			virtual void GetMousePos(int *x, int *y) = 0;
			virtual void SetMousePos(int x, int y) = 0;

			virtual MOUSE_MODE GetMouseMode() = 0;
			virtual void SetMouseMode(MOUSE_MODE _Mode) = 0;

			virtual void ShowCursor(bool _show) = 0;
		private:
			Graphics::Window* m_Window;
			
		};

		//! Special keyboard keys
		enum
		{
			UNKNOWN       = -1,
			SPACE         = 32,
			SPECIAL       = 256,
			ESC           = (SPECIAL+1),
			F1            = (SPECIAL+2),
			F2            = (SPECIAL+3),
			F3            = (SPECIAL+4),
			F4            = (SPECIAL+5),
			F5            = (SPECIAL+6),
			F6            = (SPECIAL+7),
			F7            = (SPECIAL+8),
			F8            = (SPECIAL+9),
			F9            = (SPECIAL+10),
			F10           = (SPECIAL+11),
			F11           = (SPECIAL+12),
			F12           = (SPECIAL+13),
			F13           = (SPECIAL+14),
			F14           = (SPECIAL+15),
			F15           = (SPECIAL+16),
			F16           = (SPECIAL+17),
			F17           = (SPECIAL+18),
			F18           = (SPECIAL+19),
			F19           = (SPECIAL+20),
			F20           = (SPECIAL+21),
			F21           = (SPECIAL+22),
			F22           = (SPECIAL+23),
			F23           = (SPECIAL+24),
			F24           = (SPECIAL+25),
			F25           = (SPECIAL+26),
			UP            = (SPECIAL+27),
			DOWN          = (SPECIAL+28),
			LEFT          = (SPECIAL+29),
			RIGHT         = (SPECIAL+30),
			LSHIFT        = (SPECIAL+31),
			RSHIFT        = (SPECIAL+32),
			LCTRL         = (SPECIAL+33),
			RCTRL         = (SPECIAL+34),
			LALT          = (SPECIAL+35),
			RALT          = (SPECIAL+36),
			TAB           = (SPECIAL+37),
			ENTER         = (SPECIAL+38),
			BACKSPACE     = (SPECIAL+39),
			INSERT        = (SPECIAL+40),
			DEL           = (SPECIAL+41),
			PAGEUP        = (SPECIAL+42),
			PAGEDOWN      = (SPECIAL+43),
			HOME          = (SPECIAL+44),
			END           = (SPECIAL+45),
			KP_0          = (SPECIAL+46),
			KP_1          = (SPECIAL+47),
			KP_2          = (SPECIAL+48),
			KP_3          = (SPECIAL+49),
			KP_4          = (SPECIAL+50),
			KP_5          = (SPECIAL+51),
			KP_6          = (SPECIAL+52),
			KP_7          = (SPECIAL+53),
			KP_8          = (SPECIAL+54),
			KP_9          = (SPECIAL+55),
			KP_DIVIDE     = (SPECIAL+56),
			KP_MULTIPLY   = (SPECIAL+57),
			KP_SUBTRACT   = (SPECIAL+58),
			KP_ADD        = (SPECIAL+59),
			KP_DECIMAL    = (SPECIAL+60),
			KP_EQUAL      = (SPECIAL+61),
			KP_ENTER      = (SPECIAL+62),
			LAST          = 511
		};

		enum MOUSE_MODE
		{
			MODE_RELATIVE = 0,
			MODE_ABSOLUTE = 1,
		};

		enum MOUSE_BUTTON
		{
			MOUSE_1       = 384,
			MOUSE_2       = MOUSE_1 + 1,
			MOUSE_3       = MOUSE_1 + 2,
			MOUSE_4       = MOUSE_1 + 3,
			MOUSE_5       = MOUSE_1 + 4,
			MOUSE_6       = MOUSE_1 + 5,
			MOUSE_7       = MOUSE_1 + 6,
			MOUSE_8       = MOUSE_1 + 7,

			MOUSE_NONE    = 0,
			MOUSE_LAST    = MOUSE_8,
			MOUSE_LEFT    = MOUSE_1,
			MOUSE_RIGHT   = MOUSE_2,
			MOUSE_MIDDLE  = MOUSE_3
		};
	} // Input
} // Pxf

#endif // _PXF_INPUT_INPUT_H_