#ifndef LIB_EPOS_DRIVER_H_
#define LIB_EPOS_DRIVER_H_

#include "driver_interface.h"

typedef struct EposDriver {
  DriverInterface driverInterfaceView;
  uint8_t nodeId;
} EposDriver;

void EposDriver_create(EposDriver *self, uint8_t nodeId);

DriverInterface *EposDriver_viewAsDriverInterface(EposDriver *self);

#endif  // LIB_EPOS_DRIVER_H_
