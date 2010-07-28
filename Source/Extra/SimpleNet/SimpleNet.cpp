#ifdef CONF_WITH_ENET

#include <Pxf/Base/Debug.h>

#include <Pxf/Extra/SimpleNet/SimpleNet.h>

#define LOCAL_MSG "SimpleNet"

using namespace Pxf;
using namespace Pxf::Extra;

SimpleNet::SimpleNet()
{
  if (enet_initialize() != 0)
  {
    Message(LOCAL_MSG, "An error occurred while initializing ENet.");
  }
}

SimpleNet::~SimpleNet()
{
  enet_deinitialize();
}

#endif
