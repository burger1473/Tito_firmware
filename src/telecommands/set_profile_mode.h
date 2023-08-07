#ifndef TELECOMMANDS_SET_PROFILE_MODE_H_
#define TELECOMMANDS_SET_PROFILE_MODE_H_

#include "telecommand_interface.h"
#include "TITO_Mov.h"

typedef struct __attribute__((packed)) SetProfileModeInputArguments {
  TITO_Mov_ProfileMode_t profileMode;
} SetProfileModeInputArguments_t;

typedef struct __attribute__((packed)) SetProfileModeOutputArguments {
} SetProfileModeOutputArguments_t;

typedef struct SetProfileMode {
  TelecommandInterface telecommandInterfaceView;
  SetProfileModeInputArguments_t inputArguments;
  SetProfileModeOutputArguments_t outputArguments;
} SetProfileMode;

void SetProfileMode_create(SetProfileMode *self);

TelecommandInterface *SetProfileMode_viewAsTelecommandInterface(SetProfileMode *self);

#endif  // TELECOMMANDS_SET_PROFILE_MODE_H_
