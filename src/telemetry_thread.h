#ifndef TELEMETRY_THREAD_H_
#define TELEMETRY_THREAD_H_

#include <zephyr/kernel.h>

#include "network_parameters.h"
#include "message_handler.h"
#include "telemetry_interface.h"
#include "communication_interface.h"
#include "telemetry_manager.h"
#include "udp_handler.h"

typedef struct TelemetryThread {
  UdpHandler udpHandler;
  MessageHandler messageHandler;
  TelemetryManager *telemetryManager;
} TelemetryThread;

void TelemetryThread_create(TelemetryThread *self,
                            TelemetryManager *telemetryManager,
                            CommunicationInterface *messageChannel);

void TelemetryThread_start(void *self, void *, void *);

#endif  // TELEMETRY_THREAD_H_
