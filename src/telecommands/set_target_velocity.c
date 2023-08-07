#include "set_target_velocity.h"
#include "Telecommands.h"
#include <string.h>

#define SET_TARGET_VELOCITY_OPERATION  TC_Operation_SetTargetVelocity

//******************************************************************************
// Interface implementations
//******************************************************************************
static bool SetTargetVelocity_isEnabled(void *self) {
  return true;
}

static void SetTargetVelocity_loadInputArguments(void *self, uint8_t *arguments, size_t argumentsSize) {
  SetTargetVelocity *_self = (SetTargetVelocity *)self;
  memcpy(&_self->inputArguments, arguments, argumentsSize);
}

static bool SetTargetVelocity_execute(void *self) {
  SetTargetVelocity *_self = (SetTargetVelocity *)self;
  TITO_Mov_setTargetVelocity(_self->inputArguments.targetVelocity);
  return true;
}

static void SetTargetVelocity_getReturnArguments(void *self, uint8_t *arguments, size_t *argumentsSize) {
  SetTargetVelocity *_self = (SetTargetVelocity *)self;
  *argumentsSize = sizeof(SetTargetVelocityOutputArguments_t);
  memcpy(arguments, &_self->outputArguments, *argumentsSize);
}

static uint16_t SetTargetVelocity_getOperation(void *self) {
  return SET_TARGET_VELOCITY_OPERATION;
}

//******************************************************************************
// Public methods
//******************************************************************************
void SetTargetVelocity_create(SetTargetVelocity *self) {
  self->telecommandInterfaceView.instance = self;
  self->telecommandInterfaceView.isEnabled = SetTargetVelocity_isEnabled;
  self->telecommandInterfaceView.loadInputArguments = SetTargetVelocity_loadInputArguments;
  self->telecommandInterfaceView.execute = SetTargetVelocity_execute;
  self->telecommandInterfaceView.getReturnArguments = SetTargetVelocity_getReturnArguments;
  self->telecommandInterfaceView.getOperation = SetTargetVelocity_getOperation;
}

TelecommandInterface *SetTargetVelocity_viewAsTelecommandInterface(SetTargetVelocity *self) {
  return &self->telecommandInterfaceView;
}
