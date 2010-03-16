#include <Pxf/Base/Debug.h>

#include <Pxf/Extra/SimpleNet/SimpleServer.h>

#define LOCAL_MSG "SimpleServer"

using namespace Pxf;
using namespace Pxf::Extra;

SimpleServer::SimpleServer(char* _host, unsigned int _port, unsigned int _max_clients)
{
  if (_host != NULL)
  {
    enet_address_set_host (&m_address, _host);
    m_address_host = _host;
  } else {
    m_address.host = ENET_HOST_ANY;
    m_address_host = "*";
  }
  
  m_address.port = _port;
  m_max_clients = _max_clients;
  
}

SimpleServer::~SimpleServer()
{
  // Close server if open
  Close();
}

void SimpleServer::Open()
{
  m_server = enet_host_create(&m_address, m_max_clients, 0, 0);
  if (m_server == NULL)
  {
    Message(LOCAL_MSG, "An error occurred while trying to create an ENet server host.");
    return;
  }
  
  Message(LOCAL_MSG, "Server started on %s:%i", m_address_host, m_address.port);
  
  // TODO: Any more setup for servers?
}

void SimpleServer::Close()
{
  if (m_server != NULL)
  {
    enet_host_destroy(m_server);
  }
}

int SimpleServer::MessagePump(NetMessage* _message)
{
  
  if (enet_host_service (m_server, (ENetEvent*)_message, 0) > 0)
  {
    switch (_message->type)
    {
      case ENET_EVENT_TYPE_CONNECT:
        return PUMP_RESULT_CONNECT;
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        return PUMP_RESULT_DISCONNECT;
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        
        // Take care of internal data
        // return NetPumpResult.INTERNAL;
        
        return PUMP_RESULT_DATA;
        break;
    }
  }
  
  return PUMP_RESULT_EMPTY;
}

