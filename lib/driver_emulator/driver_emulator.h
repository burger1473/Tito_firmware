#ifndef LIB_DRIVER_EMULATOR_H_
#define LIB_DRIVER_EMULATOR_H_

#include "driver_interface.h"

typedef struct DriverEmulator {
  DriverInterface driverInterfaceView;
  uint8_t nodeId;
} DriverEmulator;

void DriverEmulator_create(DriverEmulator *self, uint8_t nodeId);

DriverInterface *DriverEmulator_viewAsDriverInterface(DriverEmulator *self);

#endif  // LIB_DRIVER_EMULATOR_H_
