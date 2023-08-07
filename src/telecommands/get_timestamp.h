#ifndef TELECOMMANDS_GET_TIMESTAMP_H_
#define TELECOMMANDS_GET_TIMESTAMP_H_

#include "telecommand_interface.h"

typedef struct __attribute__((packed)) GetTimestampOutputArguments {
  uint32_t timestamp;
} GetTimestampOutputArguments_t;

typedef struct GetTimestamp {
  TelecommandInterface telecommandInterfaceView;
  GetTimestampOutputArguments_t outputArguments;
} GetTimestamp;

void GetTimestamp_create(GetTimestamp *self);

TelecommandInterface *GetTimestamp_viewAsTelecommandInterface(GetTimestamp *self);

#endif  // TELECOMMANDS_GET_TIMESTAMP_H_
