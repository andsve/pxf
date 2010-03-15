#ifndef __PXF_EXTRA_SIMPLENET_H_
#define __PXF_EXTRA_SIMPLENET_H_

#include <enet/enet.h>

namespace Pxf
{	
	namespace Extra
	{

    // Used to wrap ENet lib.
    // Will be extended by SimpleClient and SimpleServer
    class SimpleNet
    {
  
    public:
      SimpleNet();
      ~SimpleNet();

    private:
  
    };
  } // Extra
} // Pxf

#endif