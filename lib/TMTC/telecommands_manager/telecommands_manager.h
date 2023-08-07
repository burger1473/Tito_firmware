#ifndef TELECOMMANDS_MANAGER_H_
#define TELECOMMANDS_MANAGER_H_

#include "telecommand_interface.h"
#include "message_handler.h"

#define MAX_NUMBER_OF_TELECOMMANDS 64

typedef struct TelecommandsManager {
  TelecommandInterface *telecommands[MAX_NUMBER_OF_TELECOMMANDS];
  size_t currentNumberOfTelecommands;
} TelecommandsManager;

void TelecommandsManager_create(TelecommandsManager *self);

bool TelecommandsManager_addNewTelecommand(TelecommandsManager *self,
                                           TelecommandInterface *newTelecommand);

bool TelecommandsManager_isKnownOperation(TelecommandsManager *self, Message_t *message);

TelecommandInterface *TelecommandsManager_getTelecommandFromMessage(
    TelecommandsManager *self, Message_t *message);

#endif  // TELECOMMANDS_MANAGER_H_
