#ifndef __PXF_EXTRA_SIMPLENET_H_
#define __PXF_EXTRA_SIMPLENET_H_

#include <enet/enet.h>

namespace Pxf
{	
	namespace Extra
	{
	  
    typedef ENetEvent NetMessage;
    typedef enum
    {
      PUMP_RESULT_EMPTY = 0,
      PUMP_RESULT_INTERNAL = 1,
      PUMP_RESULT_CONNECT = 2,
      PUMP_RESULT_DISCONNECT = 3,
      PUMP_RESULT_DATA = 4
    } NetPumpResult;

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