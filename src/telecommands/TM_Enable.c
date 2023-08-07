#include "TM_Enable.h"
#include "Telecommands.h"
#include <string.h>

#define TM_Enable_OPERATION TC_Operation_TMEnable    //Numero de operacion o ID de este TC (Esto sirve para dar un ID a cada telecomando y poder manejarlo con el protocolo de aplicacion que viene de tierra)

extern TelemetryThread *TelemetryThread_self;

//******************************************************************************
// Interface implementations
//******************************************************************************

/*========================================================================
  Funcion: TMEnable_isEnabled
  Descripcion: Funcion par habilitar este TC. NO se usa actualmente
  Parametro de entrada:
                        TMEnable *self:            Puntero a la interface de este telecomando
  No retorna nada
  ========================================================================*/
static bool TMEnable_isEnabled(void *self) {
  return true;
}

/*========================================================================
  Funcion: TMEnable_loadInputArguments
  Descripcion: Se asignan los argumentos de entrada de esta interface
  Parametro de entrada:
                        TMEnable *self:            Puntero a la interface de este telecomando
                        uint8_t *arguments:       Puntero de la variable que contiene los argumentos
                        size_t *argumentsSize:    Puntero de la variable que contiene el tamaño de los argumentos
  No retorna nada
  ========================================================================*/
static void TMEnable_loadInputArguments(void *self, uint8_t *arguments, size_t argumentsSize) {
  TMEnable *_self = (TMEnable *)self;
  memcpy(&_self->inputArguments, arguments, argumentsSize);           //Se copian los argumentos desde el puntero a la interface
}

/*========================================================================
  Funcion: TMEnable_execute
  Descripcion: Ejecuta el TC de set mode
  Parametro de entrada:
                        TMEnable *self:            Puntero a la interface de este telecomando
  No retorna nada
  ========================================================================*/
static bool TMEnable_execute(void *self) {
  TMEnable *_self = (TMEnable *)self;
  for (int i = 0; i < TelemetryThread_self->telemetryManager->currentNumberOfTelemetry; i++) {
    TelemetryInterface *telemetry=TelemetryThread_self->telemetryManager->telemetry[i];
    if(TelemetryInterface_getOperation(telemetry)==_self->inputArguments.TM){
      if(_self->inputArguments.state==1){
        TelemetryInterface_setEnabled(telemetry, true);
      }else{
        TelemetryInterface_setEnabled(telemetry, false);
      }
    }
  }
  return true;
}

/*========================================================================
  Funcion: TMEnable_getReturnArguments
  Descripcion: Devuelve los argumentos de salida de esta interface
  Parametro de entrada:
                        TMEnable *self:            Puntero a la interface de este telecomando
                        uint8_t *arguments:       Puntero de la variable donde se desea guardar los argumentos
                        size_t *argumentsSize:    Puntero de la variable donde se desea guardar el tamaño de los argumentos
  No retorna nada
  ========================================================================*/
static void TMEnable_getReturnArguments(void *self, uint8_t *arguments, size_t *argumentsSize) {
  TMEnable *_self = (TMEnable *)self;
  *argumentsSize = sizeof(TMEnableOutputArguments_t);              //Se obtiene tamaño
  memcpy(arguments, &_self->outputArguments, *argumentsSize);     //Se copian los argumentos desde la interface al puntero
}

/*========================================================================
  Funcion: TMEnable_getOperation
  Descripcion: Retorna el numero de operacion o ID de este TC (Esto sirve para dar un ID a cada telecomando y poder manejarlo con el protocolo de aplicacion que viene de tierra)
  Parametro de entrada:
                        TMEnable *self:     Puntero a la interface de este telecomando 
  Retorna el numero de operacion
  ========================================================================*/
static uint16_t TMEnable_getOperation(void *self) {
  return TM_Enable_OPERATION;                                                       //Retorna el numero de operacion
}

//******************************************************************************
// Public methods
//******************************************************************************
/*========================================================================
  Funcion: TMEnable_create
  Descripcion: Crea telecomando para setear el modo de tito
  Parametro de entrada:
                        TMEnable *self:            Puntero a la interface de este telecomando
  No retorna nada
  ========================================================================*/
void TMEnable_create(TMEnable *self) {
  self->telecommandInterfaceView.instance = self;                                   //Puntero a su propia interface
  self->telecommandInterfaceView.isEnabled = TMEnable_isEnabled;                     //Puntero a la funcion para habilitar este TC (no se usa actualmente)
  self->telecommandInterfaceView.loadInputArguments = TMEnable_loadInputArguments;   //Puntero a la funcion para cargar los argumento de entrada de la interface (actualmente el unico argumento de entrada es el modo de operacion de tito)
  self->telecommandInterfaceView.execute = TMEnable_execute;                         //Puntero a la funcion para ejecutar el TC  (Actualmente se llama a TITO_Mov_TMEnable en tito_Mov.c)
  self->telecommandInterfaceView.getReturnArguments = TMEnable_getReturnArguments;   //Puntero a la funcion que retorna los argumentos de salida de la interface
  self->telecommandInterfaceView.getOperation = TMEnable_getOperation;               //Puntero a la funcion que devuelve el numero de operacion o TC (Esto sirve para dar un ID a cada telecomando y poder manejarlo con el protocolo de aplicacion que viene de tierra)
}

TelecommandInterface *TMEnable_viewAsTelecommandInterface(TMEnable *self) {
  return &self->telecommandInterfaceView;
}
