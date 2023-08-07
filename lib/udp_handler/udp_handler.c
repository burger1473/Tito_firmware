#include "udp_handler.h"

//******************************************************************************
// Interface implementations prototypes
//******************************************************************************
static uint8_t UdpHandler_send(void *self, const uint8_t *buffer, size_t bufferSize);
static uint8_t UdpHandler_receive(void *self, uint8_t *buffer, size_t bufferSize,
                                  size_t *receivedSize);

//******************************************************************************
// Private methods
//******************************************************************************
static void UdpHandler_initializeLanderAddress(UdpHandler *self,
                                               const char *landerAddress,
                                               uint16_t destinationPort) {
  self->landerAddress.sin_family = AF_INET;
  net_addr_pton(AF_INET, landerAddress, &self->landerAddress.sin_addr);
  self->landerAddress.sin_port = htons(destinationPort);
}

static void UdpHandler_initializeSocket(UdpHandler *self) {
  self->socketFileDescriptor = zsock_socket(AF_INET, SOCK_DGRAM, 0);
  if (self->socketFileDescriptor < 0) { printk("Error: socket: %d\n", errno); }
}

static void UdpHandler_initializeInterface(UdpHandler *self) {
  self->communicationInterfaceView.instance = self;
  self->communicationInterfaceView.send = UdpHandler_send;
  self->communicationInterfaceView.receive = UdpHandler_receive;
}

//******************************************************************************
// Interface implementations
//******************************************************************************
static uint8_t UdpHandler_send(void *self, const uint8_t *buffer, size_t bufferSize) {
  UdpHandler *_self = (UdpHandler *)self;
  zsock_sendto(_self->socketFileDescriptor, buffer, bufferSize, 0,
               (struct sockaddr *)&_self->landerAddress,
               sizeof(_self->landerAddress));
  return 0;
}

static uint8_t UdpHandler_receive(void *self, uint8_t *buffer, size_t bufferSize,
                                  size_t *receivedSize) {
  UdpHandler *_self = (UdpHandler *)self;
  socklen_t client_length = sizeof(_self->landerAddress);
  size_t returnValue = zsock_recvfrom(_self->socketFileDescriptor, buffer, bufferSize,
                                      MSG_WAITALL, (struct sockaddr *)&_self->landerAddress,
                                      &client_length);
  if (returnValue != -1) {
    *receivedSize = returnValue;
    return 1;
  }
  return 0;
}

//******************************************************************************
// Public methods
//******************************************************************************
void UdpHandler_create(UdpHandler *self, const char *landerAddress, uint16_t landerPort) {
  UdpHandler_initializeInterface(self);
  UdpHandler_initializeSocket(self);
  UdpHandler_initializeLanderAddress(self, landerAddress, landerPort);
}

void UdpHandler_bind(UdpHandler *self, uint16_t port) {
  memset(&self->roverAddress, 0, sizeof(self->roverAddress));
  self->roverAddress.sin_family = AF_INET;
  self->roverAddress.sin_addr.s_addr = INADDR_ANY;
  self->roverAddress.sin_port = htons(port);
  int bindStatus = zsock_bind(self->socketFileDescriptor,
                              (struct sockaddr *)&self->roverAddress,
                              sizeof(self->roverAddress));
  if (bindStatus < 0) { printk("Error binding socket to port %d\n", port); }
}

CommunicationInterface *UdpHandler_viewAsCommunicationInterface(UdpHandler *self) {
  return &self->communicationInterfaceView;
}
