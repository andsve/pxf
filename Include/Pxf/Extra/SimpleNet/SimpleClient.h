#ifndef __PXF_EXTRA_SIMPLECLIENT_H_
#define __PXF_EXTRA_SIMPLECLIENT_H_

#include <Pxf/Extra/SimpleNet/SimpleNet.h>

namespace Pxf
{
  namespace Extra
  {
    class SimpleClient : public SimpleNet
    {
    public:
      SimpleClient();
      ~SimpleClient();
      
      void Connect(char* _host, unsigned int _port, unsigned int _channels = 1);
      void Disconnect();
      
      int MessagePump(NetMessage* _message);

    private:
      char* m_address_host;
      unsigned int m_channels;
      // ENet specifics
      ENetHost* m_client;
      ENetAddress m_address;
      ENetPeer* m_peer;
    };
  } /* Extra */
} /* Pxf */

#endif