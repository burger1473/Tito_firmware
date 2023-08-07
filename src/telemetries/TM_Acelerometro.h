#ifndef TELEMETRIES_TM_ACELEROMETRO_H_
#define TELEMETRIES_TM_ACELEROMETRO_H_

#include "telemetry_interface.h"
#include "network_parameters.h"
#include "message_handler.h"
#include "udp_handler.h"
#include "telemetry_thread.h"

typedef struct TM_Acelerometro {
  TelemetryInterface telemetryInterfaceView;
} TM_Acelerometro;

//Prototipos
void TM_Acelerometro_inicialize(TM_Acelerometro *self);

TelemetryInterface *TM_Acelerometro_viewAsTelemetryInterface(TM_Acelerometro *self);

#endif  // TELEMETRIES_TM_ACELEROMETRO_H_
