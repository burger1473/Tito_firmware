#ifndef TELECOMMANDS_TM_Enable_H_
#define TELECOMMANDS_TM_Enable_H_

#include "telecommand_interface.h"
#include "telemetry_interface.h"
#include "telemetry_thread.h"       //Hilo para procesar TM

typedef struct __attribute__((packed)) TMEnableInputArguments {
  int8_t TM;
  int8_t state;
} TMEnableInputArguments_t;

typedef struct __attribute__((packed)) TMEnableOutputArguments {
} TMEnableOutputArguments_t;

typedef struct TMEnable {
  TelecommandInterface telecommandInterfaceView;
  TMEnableInputArguments_t inputArguments;
  TMEnableOutputArguments_t outputArguments;
} TMEnable;

void TMEnable_create(TMEnable *self);

TelecommandInterface *TMEnable_viewAsTelecommandInterface(TMEnable *self);

#endif  // TELECOMMANDS_SET_MODE_H_
