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

void SimpleServer::MessagePump()
{
  ENetEvent m_event;

  /* Wait up to 1000 milliseconds for an event. */
  while (enet_host_service (m_server, &m_event, 0) > 0)
  {
    switch (m_event.type)
    {
      case ENET_EVENT_TYPE_CONNECT:
        Message(LOCAL_MSG, "A new client connected from %x:%u.", 
        m_event.peer->address.host,
        m_event.peer->address.port);

        /* Store any relevant client information here. */
        m_event.peer->data = (void*)"Client information";

        break;

      case ENET_EVENT_TYPE_RECEIVE:
        Message(LOCAL_MSG, "A packet of length %u containing %s was received from %s on channel %u.",
        m_event.packet->dataLength,
        m_event.packet->data,
        m_event.peer->data,
        m_event.channelID);

        /* Clean up the packet now that we're done using it. */
        enet_packet_destroy(m_event.packet);

        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        Message(LOCAL_MSG, "%s disconected.\n", (const char*)m_event.peer->data);

        /* Reset the peer's client information. */
        m_event.peer->data = NULL;
    }
  }
}

