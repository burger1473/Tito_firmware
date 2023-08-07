#ifndef TELECOMMAND_INTERFACE_H_
#define TELECOMMAND_INTERFACE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct TelecommandInterface {
  void *instance;
  bool (*isEnabled)(void *self);
  void (*loadInputArguments)(void *self, uint8_t *arguments, size_t argumentsSize);
  bool (*execute)(void *self);
  void (*getReturnArguments)(void *self, uint8_t *arguments, size_t *argumentsSize);
  uint16_t (*getOperation)(void *self);
} TelecommandInterface;

bool TelecommandInterface_isEnabled(TelecommandInterface *self);

void TelecommandInterface_loadInputArguments(TelecommandInterface *self, uint8_t *arguments, size_t argumentsSize);

bool TelecommandInterface_execute(TelecommandInterface *self);

void TelecommandInterface_getReturnArguments(TelecommandInterface *self, uint8_t *arguments, size_t *argumentsSize);

uint16_t TelecommandInterface_getOperation(TelecommandInterface *self);

#endif  // TELECOMMAND_INTERFACE_H_
