#ifndef TELECOMMANDS_SET_TARGET_VELOCITY_H_
#define TELECOMMANDS_SET_TARGET_VELOCITY_H_

#include "telecommand_interface.h"
#include "TITO_Mov.h"

typedef struct __attribute__((packed)) SetTargetVelocityInputArguments {
    float targetVelocity;
} SetTargetVelocityInputArguments_t;

typedef struct __attribute__((packed)) SetTargetVelocityOutputArguments {
} SetTargetVelocityOutputArguments_t;

typedef struct SetTargetVelocity {
  TelecommandInterface telecommandInterfaceView;
  SetTargetVelocityInputArguments_t inputArguments;
  SetTargetVelocityOutputArguments_t outputArguments;
} SetTargetVelocity;

void SetTargetVelocity_create(SetTargetVelocity *self);

TelecommandInterface *SetTargetVelocity_viewAsTelecommandInterface(SetTargetVelocity *self);

#endif  // TELECOMMANDS_SET_TARGET_VELOCITY_H_
