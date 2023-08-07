#include "telecommand_interface.h"

bool TelecommandInterface_isEnabled(TelecommandInterface *self) {
  return self->isEnabled(self->instance);
}

void TelecommandInterface_loadInputArguments(TelecommandInterface *self, uint8_t *arguments, size_t argumentsSize) {
  return self->loadInputArguments(self->instance, arguments, argumentsSize);
}

bool TelecommandInterface_execute(TelecommandInterface *self) {
  return self->execute(self->instance);
}

void TelecommandInterface_getReturnArguments(TelecommandInterface *self, uint8_t *arguments, size_t *argumentsSize) {
  return self->getReturnArguments(self->instance, arguments, argumentsSize);
}

uint16_t TelecommandInterface_getOperation(TelecommandInterface *self) {
  return self->getOperation(self->instance);
}
