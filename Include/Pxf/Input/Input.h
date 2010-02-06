#ifndef _PXF_INPUT_INPUT_H_
#define _PXF_INPUT_INPUT_H_

#include <Pxf/Input/InputDefs.h>

namespace Pxf
{
	namespace Graphics { class Window; }

	namespace Input
	{

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

	} // Input
} // Pxf

#endif // _PXF_INPUT_INPUT_H_