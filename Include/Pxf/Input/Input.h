#ifndef _PXF_INPUT_INPUT_H_
#define _PXF_INPUT_INPUT_H_

namespace Pxf
{
	namespace Input
	{
		//! Abstract input class
		class Input
		{
		public:
			virtual void Update() = 0;
			
		};
	} // Input
} // Pxf

#endif // _PXF_INPUT_INPUT_H_