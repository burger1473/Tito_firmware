#include "epos_driver.h"

#include "epos4.h"

static uint8_t EposDriver_setPosition(void *self, float newTargetPosition) {
  EposDriver *_self = (EposDriver *)self;

  uint8_t position[4] = {0};
  position[3] = (uint8_t)((int)newTargetPosition & 0xFF);
  position[2] = (uint8_t)(((int)newTargetPosition>>8) & 0xFF);
  position[1] = (uint8_t)(((int)newTargetPosition>>16) & 0xFF);
  position[0] = (uint8_t)(((int)newTargetPosition>>24) & 0xFF);
  uint8_t maxVel[4] = {0x00, 0x00, 0x00, 0x32};  // 50 rpm(velocidad al final de la rampa en control de posicion)
  uint8_t rampAccel[4] = {0x00, 0x00, 0x00, 0x0A};  // Aceleracion 10 rpm/s
  uint8_t rampDeaccel[4] = {0x00, 0x00, 0x00, 0x0A};  // Desaceleracion 10 rpm/s

  return Epos4_PROFILE_POSITION_MODE(_self->nodeId, position, maxVel, rampAccel, rampDeaccel);
}

static void EposDriver_initializeInterface(EposDriver *self) {
  self->driverInterfaceView.instance = self;
  self->driverInterfaceView.setPosition = EposDriver_setPosition;
}

void EposDriver_create(EposDriver *self, uint8_t nodeId) {
  EposDriver_initializeInterface(self);
  self->nodeId = nodeId;
}

DriverInterface *EposDriver_viewAsDriverInterface(EposDriver *self) {
  return &self->driverInterfaceView;
}
