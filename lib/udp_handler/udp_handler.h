#ifndef UDP_HANDLER_H_
#define UDP_HANDLER_H_

#include <zephyr/net/socket.h>

#include "communication_interface.h"

typedef struct UdpHandler {
  CommunicationInterface communicationInterfaceView;
  int socketFileDescriptor;
  struct sockaddr_in landerAddress;
  struct sockaddr_in roverAddress;
} UdpHandler;

void UdpHandler_create(UdpHandler *self, const char *landerAddress, uint16_t landerPort);

void UdpHandler_bind(UdpHandler *self, uint16_t roverPort);

CommunicationInterface *UdpHandler_viewAsCommunicationInterface(UdpHandler *self);

#endif  // UDP_HANDLER_H_
