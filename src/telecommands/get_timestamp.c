#include "get_timestamp.h"
#include "Telecommands.h"
#include <string.h>
#include <zephyr/kernel.h>

#define GET_TIMESTAMP_OPERATION TC_Operation_GetTimestamp

//******************************************************************************
// Interface implementations
//******************************************************************************
static bool GetTimestamp_isEnabled(void *self) {
  printk("Hello from GetTimestamp_isEnabled\n");
  return true;
}

static void GetTimestamp_loadInputArguments(void *self, uint8_t *arguments, size_t argumentsSize) { }

static bool GetTimestamp_execute(void *self) {
  printk("Hello from GetTimestamp_execute\n");
  GetTimestamp *_self = (GetTimestamp *)self;
  _self->outputArguments.timestamp = (uint32_t)k_uptime_get();
  return true;
}

static void GetTimestamp_getReturnArguments(void *self, uint8_t *arguments,
                                            size_t *argumentsSize) {
  GetTimestamp *_self = (GetTimestamp *)self;
  *argumentsSize = sizeof(GetTimestampOutputArguments_t);
  memcpy(arguments, &_self->outputArguments, *argumentsSize);
}

static uint16_t GetTimestamp_getOperation(void *self) {
  return GET_TIMESTAMP_OPERATION;
}

//******************************************************************************
// Public methods
//******************************************************************************
void GetTimestamp_create(GetTimestamp *self) {
  self->telecommandInterfaceView.instance = self;
  self->telecommandInterfaceView.isEnabled = GetTimestamp_isEnabled;
  self->telecommandInterfaceView.loadInputArguments = GetTimestamp_loadInputArguments;
  self->telecommandInterfaceView.execute = GetTimestamp_execute;
  self->telecommandInterfaceView.getReturnArguments = GetTimestamp_getReturnArguments;
  self->telecommandInterfaceView.getOperation = GetTimestamp_getOperation;
}

TelecommandInterface *GetTimestamp_viewAsTelecommandInterface(GetTimestamp *self) {
  return &self->telecommandInterfaceView;
}
