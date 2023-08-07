#ifndef TELEMETRY_INTERFACE_H_
#define TELEMETRY_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "message_handler.h"

//Variables
typedef struct TelemetryInterface {
  void *instance;
  bool (*isEnabled)(void *self);
  void (*setEnabled)(void *self, bool state);
  bool (*execute)(void *self, MessageHandler *messageHandler);
  uint16_t (*getOperation)(void *self);
} TelemetryInterface;

//Prototipos
bool TelemetryInterface_isEnabled(TelemetryInterface *self);

bool TelemetryInterface_execute(TelemetryInterface *self, MessageHandler *messageHandler);

uint16_t TelemetryInterface_getOperation(TelemetryInterface *self);

void Telemetrynterface_setEnabled(TelemetryInterface *self, bool state);
#endif  // TELEMETRY_INTERFACE_H_