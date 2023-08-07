#include "driver_emulator.h"

#include "epos4.h"

#define ANGLE_FACTOR 100

static uint8_t DriverEmulator_setPosition(void *self, float newTargetPosition) {
  DriverEmulator *_self = (DriverEmulator *)self;

  uint8_t position[4] = {0};
  float multipliedAngle = newTargetPosition * ANGLE_FACTOR;
  position[3] = (uint8_t)((int)multipliedAngle & 0xFF);
  position[2] = (uint8_t)(((int)multipliedAngle>>8) & 0xFF);
  position[1] = (uint8_t)(((int)multipliedAngle>>16) & 0xFF);
  position[0] = (uint8_t)(((int)multipliedAngle>>24) & 0xFF);
  uint8_t maxVel[4] = {0x00, 0x00, 0x00, 0x32};  // 50 rpm(velocidad al final de la rampa en control de posicion)
  uint8_t rampAccel[4] = {0x00, 0x00, 0x00, 0x0A};  // Aceleracion 10 rpm/s
  uint8_t rampDeaccel[4] = {0x00, 0x00, 0x00, 0x0A};  // Desaceleracion 10 rpm/s

  return Epos4_PROFILE_POSITION_MODE(_self->nodeId, position, maxVel, rampAccel, rampDeaccel);
}

static void DriverEmulator_initializeInterface(DriverEmulator *self) {
  self->driverInterfaceView.instance = self;
  self->driverInterfaceView.setPosition = DriverEmulator_setPosition;
}

void DriverEmulator_create(DriverEmulator *self, uint8_t nodeId) {
  DriverEmulator_initializeInterface(self);
  self->nodeId = nodeId;
}

DriverInterface *DriverEmulator_viewAsDriverInterface(DriverEmulator *self) {
  return &self->driverInterfaceView;
}
