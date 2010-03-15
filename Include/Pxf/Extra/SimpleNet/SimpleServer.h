#ifndef __PXF_EXTRA_SIMPLESERVER_H_
#define __PXF_EXTRA_SIMPLESERVER_H_

#include <Pxf/Extra/SimpleNet/SimpleNet.h>

namespace Pxf
{
  namespace Extra
  {
    class SimpleServer : public SimpleNet
    {
    public:
      SimpleServer(char* _host, unsigned int _port, unsigned int _max_clients = 32);
      ~SimpleServer();
      
      void Open();
      void Close();
      void MessagePump();

    private:
      unsigned int m_max_clients;
      char* m_address_host;
      // ENet specifics
      ENetAddress m_address;
      ENetHost *m_server;
      
    };
  } /* Extra */
} /* Pxf */

#endif