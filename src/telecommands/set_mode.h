#ifndef TELECOMMANDS_SET_MODE_H_
#define TELECOMMANDS_SET_MODE_H_

#include "telecommand_interface.h"
#include "TITO_Mov.h"

typedef struct __attribute__((packed)) SetModeInputArguments {
  TITO_Mov_Mode_t mode;
} SetModeInputArguments_t;

typedef struct __attribute__((packed)) SetModeOutputArguments {
} SetModeOutputArguments_t;

typedef struct SetMode {
  TelecommandInterface telecommandInterfaceView;
  SetModeInputArguments_t inputArguments;
  SetModeOutputArguments_t outputArguments;
} SetMode;

void SetMode_create(SetMode *self);

TelecommandInterface *SetMode_viewAsTelecommandInterface(SetMode *self);

#endif  // TELECOMMANDS_SET_MODE_H_
