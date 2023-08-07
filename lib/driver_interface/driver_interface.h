#ifndef LIB_DRIVER_INTERFACE_H_
#define LIB_DRIVER_INTERFACE_H_

#include <stdint.h>

typedef struct DriverInterface {
  void *instance;
  uint8_t (*setPosition)(void *self, float newTargetPosition);
} DriverInterface;

uint8_t DriverInterface_setPosition(DriverInterface *self, float newTargetPosition);

#endif  // LIB_DRIVER_INTERFACE_H_
