#include "telecommands_manager.h"

#include <stdbool.h>

//******************************************************************************
// Public methods
//******************************************************************************
/*========================================================================
  Funcion: TelecommandsManager_create
  Descripcion: Inicializa la interface del manejador de telecomando. Resetea a 0 la cantidad de telecomandos actuales
  Parametro de entrada:
                        TelecommandsManagere *self:            Puntero a esta interface
  No retorna nada
  ========================================================================*/
void TelecommandsManager_create(TelecommandsManager *self) {
  self->currentNumberOfTelecommands = 0;
}

/*========================================================================
  Funcion: TelecommandsManager_addNewTelecommand
  Descripcion: Agrega un nuevo telecomando a la interface del manejador de telecomandos
  Parametro de entrada:
                        TelecommandsManagere *self:            Puntero a esta interface
                        TelecommandInterface *newTelecommand:  Puntero a la interface del telecomando a agregar al manejador
  Retorna:              False:  Si no se pudo realizar
                        True:   Si se puedo realizar con exito
  ========================================================================*/
bool TelecommandsManager_addNewTelecommand(TelecommandsManager *self,
                                           TelecommandInterface *newTelecommand) {
  if (self->currentNumberOfTelecommands == MAX_NUMBER_OF_TELECOMMANDS) { return false; }    //Si la cantidad de telecomandos agregados al manejador ya es el numero maximo, se devuelve error
  self->telecommands[self->currentNumberOfTelecommands++] = newTelecommand;                 //Agrega telecomando e incrementa la cantidad en esta interface
  return true;
}

/*========================================================================
  Funcion: TelecommandsManager_addNewTelecommand
  Descripcion: Verifica si el telecomando obtenido existe en el manejador de telecomandos
  Parametro de entrada:
                        TelecommandsManagere *self:            Puntero a esta interface
                        Message_t *message:                    Puntero al mensaje recibido
  Retorna:              False:  Si no es un TC existente en el manejador de telecomadnos
                        True:   Si es un TC existente en el manejador de telecomadnos
  ========================================================================*/
bool TelecommandsManager_isKnownOperation(TelecommandsManager *self, Message_t *message) {
  for (int i = 0; i < self->currentNumberOfTelecommands; i++) {       //Recorre todos los telecomandos del manejador
    TelecommandInterface *testTelecommand = self->telecommands[i];    //Obtiene el numero o ID del telecomando actual del bucle
    if (TelecommandInterface_getOperation(testTelecommand) == message->header.operation) { //Si el numero corresponde con el ID del mensaje recibido
      return true;
    }
  }
  return false;
}

/*========================================================================
  Funcion: TelecommandsManager_getTelecommandFromMessage
  Descripcion: Obtiene el TC a ejecutar segun el header del mensaje
  Parametro de entrada:
                        TelecommandsManagere *self:            Puntero a esta interface
                        Message_t *message:                    Puntero al mensaje recibido
  Retorna el TC a ejecutar
  ========================================================================*/
TelecommandInterface *TelecommandsManager_getTelecommandFromMessage(
    TelecommandsManager *self, Message_t *message) {
  for (int i = 0; i < self->currentNumberOfTelecommands; i++) {
    TelecommandInterface *testTelecommand = self->telecommands[i];
    if (TelecommandInterface_getOperation(testTelecommand) == message->header.operation) {
      return testTelecommand;
    }
  }
  return self->telecommands[0];
}
