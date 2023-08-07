#include "communication_interface.h"

uint8_t CommunicationInterface_send(CommunicationInterface *self, const uint8_t *buffer,
                                    size_t bufferSize) {
  return self->send(self->instance, buffer, bufferSize);
}


/*========================================================================
  Funcion: CommunicationInterface_receive
  Descripcion: Llama a la funcion asocidada con la recepcion de la interface

  Retorna le valor que retorna la funcion de rececion
  ========================================================================*/
uint8_t CommunicationInterface_receive(CommunicationInterface *self, uint8_t *buffer,
                                       size_t bufferSize, size_t *receivedSize) {
  return self->receive(self->instance, buffer, bufferSize, receivedSize); //Retorna el valor que retorna la funcion receive declarada en la interface.
}