#include "driver_interface.h"

uint8_t DriverInterface_setPosition(DriverInterface *self, float newTargetPosition) {
  return self->setPosition(self->instance, newTargetPosition);
}
