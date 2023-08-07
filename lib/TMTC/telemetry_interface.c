#include "telemetry_interface.h"

bool TelemetryInterface_isEnabled(TelemetryInterface *self) {
  return self->isEnabled(self->instance);
}

bool TelemetryInterface_execute(TelemetryInterface *self, MessageHandler *messageHandler) {
  return self->execute(self->instance, messageHandler);
}

uint16_t TelemetryInterface_getOperation(TelemetryInterface *self) {
  return self->getOperation(self->instance);
}

void TelemetryInterface_setEnabled(TelemetryInterface *self, bool state) {
  return self->setEnabled(self->instance, state);
}