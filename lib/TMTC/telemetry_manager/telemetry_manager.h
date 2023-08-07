#ifndef TELEMETRY_MANAGER_H_
#define TELEMETRY_MANAGER_H_

#include "telemetry_interface.h"
#include "message_handler.h"

#define MAX_NUMBER_OF_TELEMETRY 64

typedef struct TelemetryManager {
  TelemetryInterface *telemetry[MAX_NUMBER_OF_TELEMETRY]; //Se crea un arreglo de x (MAX_NUMBER_OF_TELEMETRY) interfaces de telemetrias
  size_t currentNumberOfTelemetry;                        //Numero actual de Telemetrias
} TelemetryManager;

//Prototipos de funciones

void TelemetryManager_create(TelemetryManager *self);

bool TelemetryManager_addNewTelemetry(TelemetryManager *self, TelemetryInterface *newTelemetry);


#endif  // TELEMETRY_MANAGER_H_
