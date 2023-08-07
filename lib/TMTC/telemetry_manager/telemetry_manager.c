#include "telemetry_manager.h"

#include <stdbool.h>

//******************************************************************************
// Public methods
//******************************************************************************
/*========================================================================
  Funcion: TelemetryManager_create
  Descripcion: Inicializa la interface del manejador de telemetria. Resetea a 0 la cantidad de telemetrias actuales
  Parametro de entrada:
                        TelemetryManagere *self:            Puntero a esta interface
  No retorna nada
  ========================================================================*/
void TelemetryManager_create(TelemetryManager *self) {
  self->currentNumberOfTelemetry = 0;
}

/*========================================================================
  Funcion: TelemetryManager_addNewTelemetry
  Descripcion: Agrega una nueva telemetria a la interface del manejador de telemetria
  Parametro de entrada:
                        TelemetryManager *self:                Puntero a la interface del manejador de telemetria
                        TelemetryInterface *newTelemetry:      Puntero a la interface de la telemetria a agregar al manejador
  Retorna:              False:  Si no se pudo realizar
                        True:   Si se puedo realizar con exito
  ========================================================================*/
bool TelemetryManager_addNewTelemetry(TelemetryManager *self, TelemetryInterface *newTelemetry) {
  if (self->currentNumberOfTelemetry == MAX_NUMBER_OF_TELEMETRY) { return false; }       //Si la cantidad de telemetrias agregadas al manejador ya es el numero maximo, se devuelve error
  self->telemetry[self->currentNumberOfTelemetry++] = newTelemetry;                         //Agrega telemetria en el arreglo de telemetrias e incrementa la cantidad en este manejador de telemetria
  return true;
}
