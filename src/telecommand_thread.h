#ifndef TELECOMMAND_LISTENER_H_
#define TELECOMMAND_LISTENER_H_

#include "communication_interface.h"
#include "message_handler.h"
#include "telecommands_manager.h"

typedef struct TelecommandThread {
  MessageHandler receivedMessage;
  MessageHandler responseMessage;
  TelecommandsManager *telecommandsManager;
} TelecommandThread;

void TelecommandThread_create(TelecommandThread *self,
                              TelecommandsManager *telecommandsManager,
                              CommunicationInterface *messageChannel);

void TelecommandThread_start(void *self, void *, void *);

#endif  // TELECOMMAND_LISTENER_H_
