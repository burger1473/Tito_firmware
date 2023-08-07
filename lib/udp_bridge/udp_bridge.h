#ifndef UDP_BRIDGE_H_
#define UDP_BRIDGE_H_

#include "communication_interface.h"

typedef struct UdpBridge {
  CommunicationInterface communicationInterfaceView;
  uint16_t destinationPort;
} UdpBridge;

void UdpBridge_create(UdpBridge *self, uint16_t destinationPort);

CommunicationInterface *UdpBridge_viewAsCommunicationInterface(UdpBridge *self);

#endif  // UDP_BRIDGE_H_
