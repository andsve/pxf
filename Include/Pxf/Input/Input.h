#ifndef _PXF_INPUT_INPUT_H_
#define _PXF_INPUT_INPUT_H_

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
		private:
			Graphics::Window* m_Window;
			
		};
	} // Input
} // Pxf

#endif // _PXF_INPUT_INPUT_H_