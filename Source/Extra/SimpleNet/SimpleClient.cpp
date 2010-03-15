#include <Pxf/Base/Debug.h>

#include <Pxf/Extra/SimpleNet/SimpleClient.h>

#define LOCAL_MSG "SimpleClient"

using namespace Pxf;
using namespace Pxf::Extra;

SimpleClient::SimpleClient()
{
  m_client = enet_host_create(NULL, 1, 0, 0);

  if (m_client == NULL)
  {
    Message(LOCAL_MSG, "An error occurred while trying to create an ENet client host.");
    return;
  }
}

SimpleClient::~SimpleClient()
{
  Disconnect();
}

void SimpleClient::Connect(char* _host, unsigned int _port, unsigned int _channels)
{
  m_channels = _channels;
  if (_host != NULL)
    m_address_host = _host;
  else
    m_address_host = "*";
  
  // Resolve host to ip
  enet_address_set_host(&m_address, _host);
  m_address.port = _port;

  m_peer = enet_host_connect(m_client, &m_address, m_channels);    

  if (m_peer == NULL)
  {
    Message(LOCAL_MSG, "No available peers for initiating an ENet connection.");
    return;
  }

  /* Wait up to 5 seconds for the connection attempt to succeed. */
  if (enet_host_service (m_client, &m_event, 5000) > 0 &&
  m_event.type == ENET_EVENT_TYPE_CONNECT)
  {
    Message(LOCAL_MSG, "Connection to %s:%i succeeded.", m_address_host, m_address.port);
  }
  else
  {
    /* Either the 5 seconds are up or a disconnect event was */
    /* received. Reset the peer in the event the 5 seconds   */
    /* had run out without any significant event.            */
    enet_peer_reset (m_peer);
    Message(LOCAL_MSG, "Connection to %s:%i failed.", m_address_host, m_address.port);
  }

}

void SimpleClient::Disconnect()
{
  if (m_client)
    enet_host_destroy(m_client);
}

void SimpleClient::MessagePump()
{
  ENetEvent m_event;

  while (enet_host_service (m_client, &m_event, 0) > 0)
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
