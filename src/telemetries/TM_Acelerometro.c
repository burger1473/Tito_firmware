#include "TM_Acelerometro.h"
#include "Telemetrys.h"
#include <string.h>

#define TM_Acelerometro_OPERATION TM_Operation_Acelerometro    //Numero de operacion o ID de esta TM (Esto sirve para dar un ID a cada telemetria)

//******************************************************************************
// Interface implementations
//******************************************************************************

bool TM_Acelerometro_state = TM_Acelerometro_Inizialize_state;

/*========================================================================
  Funcion: TM_Acelerometro_isEnabled
  Descripcion: Funcion para verificar el estado de la telemetria.
  Parametro de entrada:
                        void *self:            Puntero a la interface de esta telemetria
  Retorna:
          True si esta activa
          False si esta desactivada
  ========================================================================*/
static bool TM_Acelerometro_isEnabled(void *self) {
  return TM_Acelerometro_state;
}

/*========================================================================
  Funcion: TM_Acelerometro_setEnabled
  Descripcion: Funcion para cambiar el estado de esta Telemetria
  Parametro de entrada:
                        void *self:            Puntero a la interface de esta telemetria
  No retorna nada
  ========================================================================*/
static void TM_Acelerometro_setEnabled(void *self, bool state) {
  TM_Acelerometro_state=state;
}

/*========================================================================
  Funcion: TM_Acelerometro_execute
  Descripcion: Ejecuta el envio de esta TM 
  Parametro de entrada:
                        void *self:            Puntero a la interface de esta telemetria
  No retorna nada
  ========================================================================*/
static bool TM_Acelerometro_execute(void *self, MessageHandler *messageHandler) {
  //TM_Acelerometro *_self = (TM_Acelerometro *)self;
  //Codigo de envio de TM
  char *message = "Hola mundo desde Acelerometro telemetry";
  uint16_t operation = 0xBEAF;
  MessageHandler_setOperation(messageHandler, operation);
  MessageHandler_send(messageHandler, message, strlen(message));
  return true;
}


/*========================================================================
  Funcion: TM_Acelerometro_getOperation
  Descripcion: Retorna el numero de operacion o ID de esta TM (Esto sirve para dar un ID a cada telemetria)
  Parametro de entrada:
                        void *self:     Puntero a la interface de esta telemetria
  Retorna el numero de operacion
  ========================================================================*/
static uint16_t TM_Acelerometro_getOperation(void *self) {
  return TM_Acelerometro_OPERATION;                                                       //Retorna el numero de operacion
}

//******************************************************************************
// Public methods
//******************************************************************************
/*========================================================================
  Funcion: TM_Acelerometro_inicialize
  Descripcion: Inicializa TM
  Parametro de entrada:
                        TM_Acelerometro *self:            Puntero a la interface de esta telemetria
  No retorna nada
  ========================================================================*/
void TM_Acelerometro_inicialize(TM_Acelerometro *self) {
  self->telemetryInterfaceView.instance = self;                                   //Puntero a su propia interface
  self->telemetryInterfaceView.isEnabled = TM_Acelerometro_isEnabled;             //Puntero a la funcion para devolver el estado de la TM
  self->telemetryInterfaceView.setEnabled = TM_Acelerometro_setEnabled;           //Puntero a la funcion para configurar el estado de la TM
  self->telemetryInterfaceView.execute = TM_Acelerometro_execute;                 //Puntero a la funcion para ejecutar enviar la TM
  self->telemetryInterfaceView.getOperation = TM_Acelerometro_getOperation;               //Puntero a la funcion que devuelve el numero de operacion o Tm (Esto sirve para dar un ID a cada telemetria)
}

TelemetryInterface *TM_Acelerometro_viewAsTelemetryInterface(TM_Acelerometro *self) {
  return &self->telemetryInterfaceView;
}
