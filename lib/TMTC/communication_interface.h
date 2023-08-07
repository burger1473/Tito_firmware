#ifndef COMMUNICATION_INTERFACE_H_
#define COMMUNICATION_INTERFACE_H_

#include <stdint.h>
#include <stddef.h>

typedef struct CommunicationInterface {
  void *instance;
  uint8_t (*send)(void *self, const uint8_t *buffer, size_t bufferSize);
  uint8_t (*receive)(void *self, uint8_t *buffer, size_t bufferSize, size_t *receivedSize);
} CommunicationInterface;

uint8_t CommunicationInterface_send(CommunicationInterface *self, const uint8_t *buffer,
                                    size_t bufferSize);

uint8_t CommunicationInterface_receive(CommunicationInterface *self, uint8_t *buffer,
                                       size_t bufferSize, size_t *receivedSize);

#endif  // COMMUNICATION_INTERFACE_H_
