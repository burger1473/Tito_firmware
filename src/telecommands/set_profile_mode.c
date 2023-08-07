#include "set_profile_mode.h"
#include "Telecommands.h"
#include <string.h>

#define SET_PROFILE_MODE_OPERATION TC_Operation_SetProfileMode

//******************************************************************************
// Interface implementations
//******************************************************************************
static bool SetProfileMode_isEnabled(void *self) {
  return true;
}

static void SetProfileMode_loadInputArguments(void *self, uint8_t *arguments, size_t argumentsSize) {
  SetProfileMode *_self = (SetProfileMode *)self;
  memcpy(&_self->inputArguments, arguments, argumentsSize);
}

static bool SetProfileMode_execute(void *self) {
  SetProfileMode *_self = (SetProfileMode *)self;
  TITO_Mov_setProfileMode(_self->inputArguments.profileMode);
  return true;
}

static void SetProfileMode_getReturnArguments(void *self, uint8_t *arguments, size_t *argumentsSize) {
  SetProfileMode *_self = (SetProfileMode *)self;
  *argumentsSize = sizeof(SetProfileModeOutputArguments_t);
  memcpy(arguments, &_self->outputArguments, *argumentsSize);
}

static uint16_t SetProfileMode_getOperation(void *self) {
  return SET_PROFILE_MODE_OPERATION;
}

//******************************************************************************
// Public methods
//******************************************************************************
void SetProfileMode_create(SetProfileMode *self) {
  self->telecommandInterfaceView.instance = self;
  self->telecommandInterfaceView.isEnabled = SetProfileMode_isEnabled;
  self->telecommandInterfaceView.loadInputArguments = SetProfileMode_loadInputArguments;
  self->telecommandInterfaceView.execute = SetProfileMode_execute;
  self->telecommandInterfaceView.getReturnArguments = SetProfileMode_getReturnArguments;
  self->telecommandInterfaceView.getOperation = SetProfileMode_getOperation;
}

TelecommandInterface *SetProfileMode_viewAsTelecommandInterface(SetProfileMode *self) {
  return &self->telecommandInterfaceView;
}
